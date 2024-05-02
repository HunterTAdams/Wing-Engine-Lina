
#include "ECS/Components/AtmosphereComponent.hpp"
#include "Log/Log.hpp"
#include "ECS/Registry.hpp"

namespace Lina::ECS
{
    void AtmosphereComponent::setFluidDensity(const float& density)
    {
        m_fluidDensity = density;
    }

    void AtmosphereComponent::setFluidSpeed(const float& speed)
    {
        m_fluidSpeed = speed;
    }

    void AtmosphereComponent::setAltitude(const float& altitude) 
    {
        m_altitude = altitude;
    }

    void AtmosphereComponent::setGravity(const float& gravity)
    {
        m_gravity = gravity;
    }
}