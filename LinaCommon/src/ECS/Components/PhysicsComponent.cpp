/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ECS/Components/PhysicsComponent.hpp"
#include "Log/Log.hpp"

namespace Lina::ECS
{
    void PhysicsComponent::SetIsEnabled(bool isEnabled)
    {
        LINA_WARN("Setting the enabled state of PhysicsComponent does nothing. If you are trying to disable/enable simulation, use PhysicsEngineBackend::SetBodySimulation()");
    }

    void PhysicsComponent::SetBodyWingArea(float surfaceArea)
    {
        m_surfaceArea = surfaceArea;
    }
    
    void PhysicsComponent::SetBodyCrossArea(float crossArea)
    {
        m_crossArea = crossArea;
    }

    void PhysicsComponent::SetBodyThrust(float maxThrust)
    {
        m_maxThrust = maxThrust;
    }

    void PhysicsComponent::SetLiftCoef(float liftCoef)
    {
        m_liftCoef = liftCoef;
    }

    void PhysicsComponent::SetDragCoef(float dragCoef)
    {
        m_dragCoef = dragCoef;
    }

    void PhysicsComponent::moveForward(EntityDataComponent& d)
    {
        Vector3 heading = (d.GetLocalLocation() + Vector3::Forward);
        Vector3 direction = d.GetLocalLocation() - heading;
        m_totalForce += direction.Normalized() * m_maxThrust;
    }

    void PhysicsComponent::GenerateLift(EntityDataComponent& d)
    {
        // Magnitude of lift generated from the object, given in positive Y axis relative to the object
        Vector3 top = d.GetLocalLocation() + Vector3::Up;
        //double lift = GetLiftCoef() * (airDensity * pow(GetVelocity().Magnitude(), 2) / 2) * GetBodySurfaceArea();
        Vector3 liftForce;
    }

    void PhysicsComponent::GenerateDrag(EntityDataComponent& d)
    {
        // Magnitude of drag, applied in the vector opposite of movement
        //double drag = airDensity * (pow(GetVelocity().Magnitude(), 2) / 2) * GetBodyCrossArea() * GetDragCoef();
        Vector3 dragForce = -1 * m_velocity;
        //m_totalForce += dragForce.Normalized() * drag;
    }
} // namespace Lina::ECS