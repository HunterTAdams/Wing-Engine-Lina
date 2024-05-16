#include "ECS/Systems/EditorEntitySystem.hpp"

#include "Core/Application.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/InputBackend.hpp"
#include "Core/PhysicsCommon.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/AtmosphereComponent.hpp"
#include "ECS/Registry.hpp"
#include "Math/Math.hpp"
#include "Panels/LevelPanel.hpp"
#include "imgui/imgui.h"

namespace Lina::ECS
{
    void EditorEntitySystem::Initialize(const std::string& name, Editor::LevelPanel& scenePanel)
    {
        System::Initialize(name);
        m_levelPanel  = &scenePanel;
        m_inputEngine = Input::InputEngineBackend::Get();
        m_poolSize    = 1;
    }

    void ECS::EditorEntitySystem::UpdateComponents(float delta)
    {
        if (m_editorEntity != entt::null)
        {
            EntityDataComponent& data = ECS::Registry::Get()->get<EntityDataComponent>(m_editorEntity);
            Vector2              entityAngles;
            entityAngles.x = data.GetLocalRotationAngles().x;
            entityAngles.y = data.GetLocalRotationAngles().z;

            Vector3 location = data.GetLocation();
            Quaternion rotation = data.GetRotation();

            //RotateBehavior();
            //data.SetLocalRotation(rotation);

            //GenerateLift();
            //GenerateDrag();
        }
    }
    
    void EditorEntitySystem::moveForward()
    {
        auto&   phy       = ECS::Registry::Get()->get<ECS::PhysicsComponent>(m_editorEntity);
        auto&   data      = ECS::Registry::Get()->get<ECS::EntityDataComponent>(m_editorEntity);
        Vector3 heading   = (data.GetLocation() + Vector3::Forward);
        Vector3 direction = data.GetLocation() - heading;
        phy.AddForce(direction.Normalized() * phy.GetMaxThrust());
    }

    void EditorEntitySystem::GenerateLift()
    {
        // Magnitude of lift generated from the object, given in positive Y axis relative to the object
        auto&   phy       = ECS::Registry::Get()->get<ECS::PhysicsComponent>(m_editorEntity);
        auto&   data      = ECS::Registry::Get()->get<ECS::EntityDataComponent>(m_editorEntity);
        auto&   atm       = ECS::Registry::Get()->get<ECS::AtmosphereComponent>(m_editorEntity);
        Vector3 top       = data.GetLocalLocation() + Vector3::Up;
        double  lift      = phy.GetLiftCoef() * (atm.getFluidDensity() * pow(phy.GetVelocity().Magnitude(), 2) / 2) * phy.GetBodySurfaceArea();
        Vector3 liftForce = top * lift;
        phy.AddForce(liftForce);
    }

    void EditorEntitySystem::GenerateDrag()
    {
        // Magnitude of drag, applied in the vector opposite of movement
        auto&   phy       = ECS::Registry::Get()->get<ECS::PhysicsComponent>(m_editorEntity);
        auto&   data      = ECS::Registry::Get()->get<ECS::EntityDataComponent>(m_editorEntity);
        auto&   atm       = ECS::Registry::Get()->get<ECS::AtmosphereComponent>(m_editorEntity);
        double  drag      = atm.getFluidDensity() * (pow(phy.GetVelocity().Magnitude(), 2) / 2) * phy.GetBodyCrossArea() * phy.GetDragCoef();
        Vector3 dragForce = -1 * phy.GetVelocity();
        phy.AddForce(dragForce.Normalized() * drag);
    }

    void EditorEntitySystem::RotateBehavior()
    {
        auto&             phy  = ECS::Registry::Get()->get<ECS::PhysicsComponent>(m_editorEntity);
        auto&             data = ECS::Registry::Get()->get<ECS::EntityDataComponent>(m_editorEntity);
        //static Quaternion qStart;
        if (m_inputEngine->GetKeyDown(LINA_KEY_RIGHT) || m_inputEngine->GetKeyDown(LINA_KEY_LEFT) || m_inputEngine->GetKeyDown(LINA_KEY_UP) || m_inputEngine->GetKeyDown(LINA_KEY_DOWN))
        {
            m_targetXAngle = 0;
            m_targetYAngle = 0;
        }

        if (m_inputEngine->GetKey(LINA_KEY_RIGHT))
        {
            Vector3 rotation = data.GetRotationAngles() + Vector3(m_rotationSpeeds, 0, 0);
            data.SetRotationAngles(rotation);
        }
        if (m_inputEngine->GetKey(LINA_KEY_LEFT))
        {
            Vector3 rotation = Vector3(0, 0, -1) * m_rotationSpeeds;
            phy.AddTorque(rotation);
        }
        if (m_inputEngine->GetKey(LINA_KEY_UP))
        {
            Vector3 rotation = Vector3(0, 1, 0) * m_rotationSpeeds;
            phy.AddTorque(rotation);
        }
        if (m_inputEngine->GetKey(LINA_KEY_DOWN))
        {
            Vector3 rotation = Vector3(0, -1, 0) * m_rotationSpeeds;
            phy.AddTorque(rotation);
        }
    }
}