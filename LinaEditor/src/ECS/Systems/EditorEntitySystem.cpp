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

            Vector3 location = data.GetLocation();
            Quaternion rotation = data.GetRotation();

            RotateBehavior();

            GenerateLift();
            GenerateDrag();
        }
    }

    glm::mat4 EditorEntitySystem::getTransform()
    {
        auto&     data = ECS::Registry::Get()->get<ECS::EntityDataComponent>(m_editorEntity);
        glm::mat4 T    = glm::translate(glm::mat4(1.0), glm::vec3(data.GetLocation()));
        glm::mat4 Rx   = glm::rotate(glm::mat4(1.0), data.GetRotationAngles().x, glm::vec3(1, 0, 0));
        glm::mat4 Ry   = glm::rotate(glm::mat4(1.0), data.GetRotationAngles().y, glm::vec3(0, 1, 0));
        glm::mat4 Rz   = glm::rotate(glm::mat4(1.0), data.GetRotationAngles().z, glm::vec3(0, 0, 1));
        glm::mat4 S    = glm::scale(glm::mat4(1.0), data.GetScale());
        return T * Rx * Ry * Rz * S;
    }

    void EditorEntitySystem::moveForward()
    {
        auto&   phy       = ECS::Registry::Get()->get<ECS::PhysicsComponent>(m_editorEntity);
        auto&   data      = ECS::Registry::Get()->get<ECS::EntityDataComponent>(m_editorEntity);
        Vector3 heading   = glm::vec4(1, 0, 0, 0) * getTransform();
        Vector3 direction = data.GetLocation() + heading;
        std::cout << "Location: " << data.GetLocation().ToString() << " Local loc: " << data.GetLocalLocation().ToString() << std::endl
                  << "Heading: " << heading.ToString() << std::endl;
        phy.AddForce(direction.Normalized() * phy.GetMaxThrust());
    }

    void EditorEntitySystem::GenerateLift()
    {
         //Magnitude of lift generated from the object, given in positive Y axis relative to the object
        auto&   phy       = ECS::Registry::Get()->get<ECS::PhysicsComponent>(m_editorEntity);
        auto&   data      = ECS::Registry::Get()->get<ECS::EntityDataComponent>(m_editorEntity);
        Vector3 top       = data.GetLocalLocation() + Vector3::Up;
        double  lift      = phy.GetLiftCoef() * (phy.GetAirDensity() * pow(phy.GetVelocity().Magnitude(), 2) / 2) * phy.GetBodySurfaceArea();
        Vector3 liftForce = top * lift;
        phy.AddForce(liftForce);
    }

    void EditorEntitySystem::GenerateDrag()
    {
        // Magnitude of drag, applied in the vector opposite of movement
        auto&   phy       = ECS::Registry::Get()->get<ECS::PhysicsComponent>(m_editorEntity);
        auto&   data      = ECS::Registry::Get()->get<ECS::EntityDataComponent>(m_editorEntity);
        double  drag      = phy.GetAirDensity() * (pow(phy.GetVelocity().Magnitude(), 2) / 2) * phy.GetBodyCrossArea() * phy.GetDragCoef();
        Vector3 dragForce = -1 * phy.GetVelocity();
        phy.AddForce(dragForce.Normalized() * drag);
    }

    void EditorEntitySystem::RotateBehavior()
    {
        auto&             phy  = ECS::Registry::Get()->get<ECS::PhysicsComponent>(m_editorEntity);
        auto&             data = ECS::Registry::Get()->get<ECS::EntityDataComponent>(m_editorEntity);

        if (m_inputEngine->GetKeyDown(LINA_KEY_RIGHT) || m_inputEngine->GetKey(LINA_KEY_RIGHT))
        {
            phy.AddTorque(Vector3(m_rotationSpeeds, 0, 0));
            std::cout << "Rotation angles: " << data.GetRotationAngles().ToString() << std::endl;
            std::cout << "Local Rotation angles: " << data.GetLocalRotationAngles().ToString() << std::endl;
        }
        if (m_inputEngine->GetKeyDown(LINA_KEY_LEFT) || m_inputEngine->GetKey(LINA_KEY_LEFT))
        {
            phy.AddTorque(Vector3(-m_rotationSpeeds, 0, 0));
        }
        if (m_inputEngine->GetKeyDown(LINA_KEY_UP) || m_inputEngine->GetKey(LINA_KEY_UP))
        {
            phy.AddTorque(Vector3(0, 0, m_rotationSpeeds));
        }
        if (m_inputEngine->GetKeyDown(LINA_KEY_DOWN) || m_inputEngine->GetKey(LINA_KEY_DOWN))
        {
            phy.AddTorque(Vector3(0, 0, -m_rotationSpeeds));
        }
    }
}