#pragma once

#ifndef AtmosphereComponent_HPP
#define AtmosphereComponent_HPP

#include "ECS/Component.hpp"

#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>

namespace Lina::ECS
{
    LINA_COMPONENT("Atmosphere Component", "ICON_FA_CLOUD", "", "false", "false")
    struct AtmosphereComponent : public Component
    {
        AtmosphereComponent(float density, float speed, float altitude, float gravity)
            : m_fluidDensity(density), m_fluidSpeed(speed), m_altitude(altitude), m_gravity(gravity)
        {
        }

        AtmosphereComponent() = default;

        void setFluidDensity(const float& density);
        void setFluidSpeed(const float& speed);
        void setAltitude(const float& altitude);
        void setGravity(const float& gravity);

        const float getFluidDensity()
        {
            return m_fluidDensity;
        }

        const float getFluidSpeed()
        {
            return m_fluidSpeed;
        }

        const float getAltitude()
        {
            return m_altitude;
        }

        const float getGravity()
        {
            return m_gravity;
        }

    private:
        float m_fluidDensity = 1.0f;
        float m_fluidSpeed   = 0.0f;
        float m_altitude     = 0.0f;
        float m_gravity      = 9.8f;
        friend class cereal::access;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_fluidDensity, m_fluidSpeed, m_altitude, m_gravity);
        }
    };
} // namespace Lina::ECS

#endif // !AtmosphereComponent_HPP
