#pragma once

#ifndef EditorEntitySystem_HPP
#define EditorEntitySystem_HPP

#include "Core/CommonECS.hpp"
#include "Core/InputBackendFwd.hpp"
#include "ECS/System.hpp"
#include "Math/Vector.hpp"

namespace Lina
{
    class Quaternion;
}

namespace Lina::Editor
{
    class LevelPanel;
}

namespace Lina::ECS
{
    class EditorEntitySystem : public System
    {
    public:
        void         Initialize(const std::string& name, Editor::LevelPanel& scenePanel);
        virtual void UpdateComponents(float delta) override;
        void         SetEditorEntity(Entity entity)
        {
            m_editorEntity = entity;
        }
        inline Vector2 GetRotationSpeeds()
        {
            return m_rotationSpeeds;
        }

        void moveForward();
        void GenerateLift();
        void GenerateDrag();

        void RotateBehavior();

    private:
        float             m_rotationSpeeds = 1;
        Entity              m_editorEntity = entt::null;
        Editor::LevelPanel* m_levelPanel;
        Input::InputEngine* m_inputEngine;
        float               m_targetXAngle = 0.0f;
        float               m_targetYAngle = 0.0f;
    };
}
#endif // !#define EditorEntitySystem_HPP