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
Class: EditorApplication

This class can also be derived from Lina::Application to create a standalone editor build.
Probably will be deprecated in the future as the editor is now a linked-library from Sandbox.

Timestamp: 10/15/2020 10:44:39 PM
*/

#pragma once

#ifndef EditorApplication_HPP
#define EditorApplication_HPP

#include "Core/GUILayer.hpp"
#include "ECS/Systems/EditorCameraSystem.hpp"
#include "ECS/Systems/EditorEntitySystem.hpp"

namespace Lina
{
    namespace World
    {
        class Level;
    }

    namespace Graphics
    {
        class Material;
        class RenderTarget;
        class Texture;
        class RenderBuffer;
    } // namespace Graphics

    namespace Event
    {
        struct EPlayModeChanged;
        struct ELevelInstalled;
        struct EPreSerializingLevel;
        struct ESerializedLevel;
        struct EWindowResized;
        struct EResourceLoadCompleted;
        struct EResourceReloaded;
        struct EResourcePathUpdated;

    } // namespace Event
} // namespace Lina

namespace Lina::Editor
{
    // Used for taking fixed-size snapshots of resources such as materials/models/ shaders
    struct SnapshotBuffer
    {
        Graphics::RenderTarget* m_rt           = nullptr;
        Graphics::Texture*      m_rtTexture    = nullptr;
        Graphics::RenderBuffer* m_renderBuffer = nullptr;
    };

    class EditorApplication
    {
    public:
        EditorApplication() = default;
        ~EditorApplication();
        void Initialize();

        inline static EditorApplication* Get()
        {
            return s_editorApplication;
        }

        inline ECS::EditorCameraSystem& GetCameraSystem()
        {
            return m_editorCameraSystem;
        }
        

        inline ECS::EditorEntitySystem& GetEntitySystem()
        {
            return m_editorEntitySystem;
        }

        Graphics::RenderTarget* AddSnapshotBuffer(StringIDType sid);
        uint32                  GetSnapshotTexture(StringIDType sid);

    private:
        void PlayModeChanged(const Event::EPlayModeChanged& playmode);
        void TakeModelSnapshot(StringIDType bufferSid, StringIDType modelSid, StringIDType materialSid);
        void OnLevelInstalled(const Event::ELevelInstalled& ev);
        void OnPreSerializingLevel(const Event::EPreSerializingLevel& ev);
        void OnSerializedLevel(const Event::ESerializedLevel& ev);
        void OnWindowResized(const Event::EWindowResized& ev);
        void OnResourceLoadCompleted(const Event::EResourceLoadCompleted& ev);
        void OnResourceReloaded(const Event::EResourceReloaded& ev);
        void OnResourcePathUpdated(const Event::EResourcePathUpdated& ev);

    private:
        static EditorApplication*                        s_editorApplication;
        ECS::EditorCameraSystem                          m_editorCameraSystem;
        ECS::EditorEntitySystem                          m_editorEntitySystem;
        GUILayer                                         m_guiLayer;
        std::unordered_map<StringIDType, SnapshotBuffer> m_previewBuffers;
        bool                                             m_snapshotsTaken = false;
    };
} // namespace Lina::Editor

#endif
