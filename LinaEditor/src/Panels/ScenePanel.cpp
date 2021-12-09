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

#include "Panels/ScenePanel.hpp"
#include "Core/GUILayer.hpp"
#include "Input/InputMappings.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "Core/EditorApplication.hpp"
#include "Input/InputEngine.hpp"
#include "Core/Application.hpp"
#include "imgui/imgui.h"
#include <imgui/imguizmo/ImGuizmo.h>

static ImGuizmo::OPERATION currentTransformGizmoOP = ImGuizmo::OPERATION::TRANSLATE;
static ImGuizmo::MODE currentTransformGizmoMode = ImGuizmo::MODE::WORLD;
static Matrix gridLineMatrix = Matrix::Identity();
static Matrix modelMatrix = Matrix::Identity();
static ImVec2 previousWindowSize;
#define GRID_SIZE 1000

namespace LinaEditor
{

	void ScenePanel::Setup()
	{
		EditorApplication::GetEditorDispatcher().SubscribeAction<LinaEngine::ECS::ECSEntity>("##lina_scenePanel_entity", LinaEngine::Action::ActionType::EntitySelected,
			std::bind(&ScenePanel::EntitySelected, this, std::placeholders::_1));

		EditorApplication::GetEditorDispatcher().SubscribeAction<void*>("##lina_scenePanel_unselect", LinaEngine::Action::ActionType::Unselect,
			std::bind(&ScenePanel::Unselected, this));

		Application::GetEngineDispatcher().SubscribeAction<int>("#lina_scenePanel_uninstall", LinaEngine::Action::ActionType::LevelUninstalled, std::bind(&ScenePanel::Unselected, this));
	}

	void ScenePanel::Draw()
	{
		ImGuizmo::BeginFrame();

		if (m_show)
		{

			LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
			ImGui::SetNextWindowBgAlpha(1.0f);

			if (ImGui::Begin(SCENE_ID, &m_show, flags))
			{
				// Set Focus
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImVec2 min = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
					ImVec2 max = ImVec2(min.x + ImGui::GetWindowSize().x, min.y + ImGui::GetWindowSize().y);

					if (ImGui::IsMouseHoveringRect(min, max))
						m_isFocused = true;
					else
						m_isFocused = false;
				}

				if (renderEngine.GetCameraSystem()->GetActiveCameraComponent() == nullptr)
				{
					ImGui::Text("NO CAMERA AVAILABLE");
				}

				ImGui::BeginChild("finalImage");
				WidgetsUtility::DrawShadowedLine(5);


				// Get game viewport aspect.
				Vector2 vpSize = renderEngine.GetViewportSize();
				float aspect = (float)vpSize.x / (float)vpSize.y;

				// Mins & max for scene panel area.
				float currentWindowX = ImGui::GetWindowSize().x;
				float currentWindowY = ImGui::GetWindowSize().y;
				ImVec2 pMin = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);
				ImVec2 pMax = ImVec2(ImGui::GetCursorScreenPos().x + currentWindowX - 12, ImGui::GetCursorScreenPos().y + currentWindowY - 42);
				ImVec2 size = ImGui::GetCurrentWindow()->Size;
				ImVec2 currentPanelSize = ImVec2(pMax.x - pMin.x, pMax.y - pMin.y);

				// Resize scene panel.
				if ((size.x != previousWindowSize.x || size.y != previousWindowSize.y))
				{
						LinaEngine::Application::GetRenderEngine().SetViewportDisplay(Vector2(0,0), Vector2((int)(size.x), (int)(size.y)));
					//LinaEngine::Application::GetRenderEngine().OnWindowResized((uint32)ImGui::GetCurrentWindow()->Size.x, (uint32)ImGui::GetCurrentWindow()->Size.y);
					previousWindowSize = size;
				}

				// Desired window height.
				float desiredHeight = currentPanelSize.x / aspect;


				// Calculate desired drawing rect for the image.
				ImVec2 imageRectMin = ImVec2(pMin.x, pMin.y + (currentPanelSize.y - desiredHeight) / 2.0f);
				ImVec2 imageRectMax = ImVec2(pMax.x, pMax.y - (currentPanelSize.y - desiredHeight) / 2.0f);


				if (m_drawMode == DrawMode::FinalImage)
					ImGui::GetWindowDrawList()->AddImage((void*)renderEngine.GetFinalImage(), imageRectMin, imageRectMax, ImVec2(0, 1), ImVec2(1, 0));
				else if (m_drawMode == DrawMode::ShadowMap)
					ImGui::GetWindowDrawList()->AddImage((void*)renderEngine.GetShadowMapImage(), imageRectMin, imageRectMax, ImVec2(0, 1), ImVec2(1, 0));

				ImGuiIO& io = ImGui::GetIO();
				ImGuizmo::Enable(true);
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(imageRectMin.x, imageRectMin.y, imageRectMax.x - imageRectMin.x, imageRectMax.y - imageRectMin.y);
				ImGui::PushClipRect(imageRectMin, imageRectMax, false);

				ProcessInput();
				DrawGizmos();

				

				ImGui::EndChild();

			}


			// Mesh drag & drop.
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(RESOURCES_MOVEMESH_ID))
				{
					IM_ASSERT(payload->DataSize == sizeof(uint32));

					auto& ecs = LinaEngine::Application::GetECSRegistry();
					auto& model = LinaEngine::Graphics::Model::GetModel(*(uint32*)payload->m_data);
					auto entity = ecs.CreateEntity(Utility::GetFileNameOnly(model.GetPath()));
					auto& mr = ecs.emplace<ECS::ModelRendererComponent>(entity);
					mr.SetModel(ecs, entity, model);

					auto& mat = Graphics::Material::GetMaterial("resources/engine/materials/DefaultLit.mat");

					for (int i = 0; i < model.GetMaterialSpecs().size(); i++)
						mr.SetMaterial(ecs, entity, i, mat);
				}
				ImGui::EndDragDropTarget();
			}


			ImGui::End();

		}
	}


	void ScenePanel::EntitySelected(LinaEngine::ECS::ECSEntity entity)
	{
		m_selectedTransform = entity;
	}

	void ScenePanel::Unselected()
	{
		m_selectedTransform = entt::null;
	}

	void ScenePanel::ProcessInput()
	{
		if (ImGui::IsWindowFocused())
		{
			if (ImGui::IsKeyPressed(LINA_KEY_Q))
				currentTransformGizmoOP = ImGuizmo::TRANSLATE;
			if (ImGui::IsKeyPressed(LINA_KEY_E))
				currentTransformGizmoOP = ImGuizmo::ROTATE;
			if (ImGui::IsKeyPressed(LINA_KEY_R))
				currentTransformGizmoOP = ImGuizmo::SCALE;
			if (ImGui::IsKeyPressed(LINA_KEY_T))
				currentTransformGizmoMode = currentTransformGizmoMode == ImGuizmo::MODE::WORLD ? ImGuizmo::MODE::LOCAL : ImGuizmo::MODE::WORLD;
		}
		
	}

	void ScenePanel::DrawGizmos()
	{
		LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();

		Matrix& view = renderEngine.GetCameraSystem()->GetViewMatrix();
		Matrix& projection = renderEngine.GetCameraSystem()->GetProjectionMatrix();

		//ImGui::GetWindowDrawList()->AddLine(ImVec2(coord.x, coord.y), ImVec2(coord2.x, coord2.y), col, 2);
		if (m_selectedTransform != entt::null)
		{
			ECS::EntityDataComponent& data = LinaEngine::Application::GetECSRegistry().get<ECS::EntityDataComponent>(m_selectedTransform);
			// Get required matrices.
			glm::mat4 object = data.ToMatrix();


			// Draw transformation handle.
			ImGuizmo::Manipulate(&view[0][0], &projection[0][0], currentTransformGizmoOP, currentTransformGizmoMode, &object[0][0]);

			float matrixTranslation[3], matrixRotation[3], matrixScale[3];
			ImGuizmo::DecomposeMatrixToComponents(&object[0][0], matrixTranslation, matrixRotation, matrixScale);			

			if (ImGuizmo::IsUsing())
			{
			
				glm::vec3 rot = data.GetRotationAngles();
				glm::vec3 deltaRotation = glm::vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]) - rot;
				data.SetRotationAngles(rot + deltaRotation);
			}

			data.SetLocation(Vector3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
			data.SetScale(Vector3(matrixScale[0], matrixScale[1], matrixScale[2]));

		}


		// ImGuizmo::DrawGrid(&view[0][0], &projection[0][0], &gridLineMatrix[0][0], GRID_SIZE);

	}


}