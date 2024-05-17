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

/*
Class: RigidbodyComponent
Timestamp: 9/30/2020 2:46:27 AM
*/

#pragma once

#ifndef PhysicsbodyComponent_HPP
#define PhysicsbodyComponent_HPP

#include "Core/CommonPhysics.hpp"
#include "ECS/Component.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Math/Vector.hpp"
#include "Resources/ResourceHandle.hpp"
namespace Lina
{
    namespace Editor
    {
        class ComponentDrawer;
    }

    namespace Physics
    {
        class PhysicsMaterial;

#ifdef LINA_PHYSICS_BULLET
        class BulletPhysicsEngine;
#elif LINA_PHYSICS_PHYSX
        class PhysXPhysicsEngine;
#endif
    } // namespace Physics

    namespace World
    {
        class Level;
    }

}; // namespace Lina

namespace Lina::ECS
{
    class RigidbodySystem;
    class Registry;

    LINA_COMPONENT("Physics Component", "ICON_FA_APPLE_ALT", "", "false", "false")
    struct PhysicsComponent : public Component
    {
        virtual void SetIsEnabled(bool isEnabled) override;

        Vector3 GetVelocity()
        {
            return m_velocity;
        }
        Vector3 GetAngularVelocity()
        {
            return m_angularVelocity;
        }
        Vector3 GetHalfExtents()
        {
            return m_halfExtents;
        }
        float GetRadius()
        {
            return m_radius;
        }
        float GetCapsuleHalfHeight()
        {
            return m_capsuleHalfHeight;
        }
        float GetBodySurfaceArea()
        {
            return m_surfaceArea;
        }
        float GetBodyCrossArea()
        {
            return m_crossArea;
        }
        float GetMaxThrust()
        {
            return m_maxThrust;
        }
        float GetLiftCoef()
        {
            return m_liftCoef;
        }
        float GetDragCoef()
        {
            return m_dragCoef;
        }
        float GetAirDensity()
        {
            return m_airDensity;
        }
        bool GetIsKinematic()
        {
            return m_isKinematic;
        }
        void                    AddForce(Vector3 force);
        void                    AddTorque(Vector3 torque);
        Physics::CollisionShape GetCollisionShape()
        {
            return m_collisionShape;
        }
        Physics::SimulationType GetSimType()
        {
            return m_simType;
        }

        StringIDType m_attachedModelID = 0;

    private:
        friend class cereal::access;
        friend class World::Level;
        friend class ECS::Registry;
        friend class Editor::ComponentDrawer;
        friend class ECS::RigidbodySystem;

#ifdef LINA_PHYSICS_BULLET
        friend class Physics::BulletPhysicsEngine;
#elif LINA_PHYSICS_PHYSX
        friend class Physics::PhysXPhysicsEngine;
#endif

        Physics::SimulationType                             m_simType           = Physics::SimulationType::None;
        Physics::CollisionShape                             m_collisionShape    = Physics::CollisionShape::Box;
        Vector3                                             m_halfExtents       = Vector3(0.5f, 0.5f, 0.5f);
        Vector3                                             m_velocity          = Vector3::Zero;
        Vector3                                             m_angularVelocity   = Vector3::Zero;
        float                                               m_mass              = 1.0f;
        float                                               m_radius            = 1.0f;
        float                                               m_surfaceArea       = 1.0f;
        float                                               m_crossArea         = 1.0f;
        float                                               m_maxThrust         = 1.0f;
        float                                               m_liftCoef          = 1.0f;
        float                                               m_dragCoef          = 1.0f;
        float                                               m_airDensity        = 1.0f;
        float                                               m_capsuleHalfHeight = 1.0f;
        Vector3                                             m_torque            = Vector3::Zero;
        Vector3                                             m_totalForce        = Vector3::Zero;
        bool                                                m_isKinematic       = true;
        Resources::ResourceHandle<Physics::PhysicsMaterial> m_material;

        void ResetRuntimeState()
        {
            m_velocity        = Vector3::Zero;
            m_angularVelocity = Vector3::Zero;
            m_torque          = Vector3::Zero;
            m_totalForce      = Vector3::Zero;
            m_simType         = Physics::SimulationType::None;
        }

        void Reset()
        {
            m_mass              = 1.0f;
            m_radius            = 1.0f;
            m_capsuleHalfHeight = 2.0f;
            m_collisionShape    = Physics::CollisionShape::Box;
            ResetRuntimeState();
        }

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_collisionShape, m_material, m_simType, m_halfExtents, m_torque, m_totalForce, m_mass, m_radius, m_surfaceArea, m_crossArea, m_maxThrust, m_liftCoef, m_dragCoef, m_airDensity, m_capsuleHalfHeight, m_isKinematic, m_isEnabled);
        }
    };
} // namespace Lina::ECS

#endif
