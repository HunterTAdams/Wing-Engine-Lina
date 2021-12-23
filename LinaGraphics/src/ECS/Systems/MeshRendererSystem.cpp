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

#include "ECS/Systems/MeshRendererSystem.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "Rendering/Model.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Core/RenderDeviceBackend.hpp"
#include "Rendering/Material.hpp"
#include "Animation/Skeleton.hpp"

namespace Lina::ECS
{

	void MeshRendererSystem::Initialize()
	{
		BaseECSSystem::Initialize();
		m_renderEngine = Graphics::RenderEngineBackend::Get();
		m_renderDevice = m_renderEngine->GetRenderDevice();
		m_ecs->on_destroy<ModelRendererComponent>().connect<&MeshRendererSystem::OnModelRendererRemoved>(this);
	}


	void MeshRendererSystem::UpdateComponents(float delta)
	{
		auto view = m_ecs->view<EntityDataComponent, MeshRendererComponent>();

		for (auto entity : view)
		{
			MeshRendererComponent& renderer = view.get<MeshRendererComponent>(entity);
			if (!renderer.m_isEnabled || renderer.m_excludeFromDrawList || renderer.m_materialID < 0) continue;
			
			auto& data = view.get<EntityDataComponent>(entity);

			// We get the materials, then according to their surface types we add the model
			// data into either opaque queue or the transparent queue.
			Graphics::Model& model = Lina::Graphics::Model::GetModel(renderer.m_modelID);

			for (uint32 i = 0; i < renderer.m_subMeshes.size(); i++)
			{
				auto& mesh = model.GetMeshes()[renderer.m_subMeshes[i]];
				uint32 materialSlot = mesh.GetMaterialSlotIndex();

				if (!Graphics::Material::MaterialExists(renderer.m_materialID)) continue;

				Graphics::Material& mat = Lina::Graphics::Material::GetMaterial(renderer.m_materialID);
				Matrix finalMatrix = data.ToMatrix();

				if (mat.GetSurfaceType() == Graphics::MaterialSurfaceType::Opaque)
					RenderOpaque(mesh.GetVertexArray(), model.GetSkeleton(), mat, finalMatrix);
				else
				{
					// Transparent queue is a priority queue unlike the opaque one, so we set the priority as distance to the camera.
					float priority = (m_renderEngine->GetCameraSystem()->GetCameraLocation() - data.GetLocation()).MagnitudeSqrt();
					RenderTransparent(mesh.GetVertexArray(), model.GetSkeleton(), mat, finalMatrix, priority);

				}
			}
		}

	}

	void MeshRendererSystem::RenderOpaque(Graphics::VertexArray& vertexArray, Lina::Graphics::Skeleton& skeleton, Graphics::Material& material, const Matrix& transformIn)
	{
		// Render commands basically add the necessary
		// draw data into the maps/lists etc.
		Graphics::BatchDrawData drawData;
		drawData.m_vertexArray = &vertexArray;
		drawData.m_material = &material;
		m_opaqueRenderBatch[drawData].m_models.push_back(transformIn);

		if (skeleton.IsLoaded())
		{

		}

	}

	void MeshRendererSystem::RenderTransparent(Graphics::VertexArray& vertexArray, Lina::Graphics::Skeleton& skeleton, Graphics::Material& material, const Matrix& transformIn, float priority)
	{
		// Render commands basically add the necessary
		// draw data into the maps/lists etc.
		Graphics::BatchDrawData drawData;
		drawData.m_vertexArray = &vertexArray;
		drawData.m_material = &material;
		drawData.m_distance = priority;

		Graphics::BatchModelData modelData;
		modelData.m_models.push_back(transformIn);

		if (skeleton.IsLoaded())
		{

		}
	}

	void MeshRendererSystem::FlushOpaque(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
	{
		// When flushed, all the data is delegated to the render device to do the actual
		// drawing. Then the data is cleared if complete flush is requested.

		for (std::map<Graphics::BatchDrawData, Graphics::BatchModelData>::iterator it = m_opaqueRenderBatch.begin(); it != m_opaqueRenderBatch.end(); ++it)
		{
			// Get references.
			Graphics::BatchDrawData drawData = it->first;
			Graphics::BatchModelData& modelData = it->second;
			size_t numTransforms = modelData.m_models.size();
			if (numTransforms == 0) continue;

			Graphics::VertexArray* vertexArray = drawData.m_vertexArray;
			Matrix* models = &modelData.m_models[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? drawData.m_material : overrideMaterial;


			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(7, models, numTransforms * sizeof(Matrix));

			if (modelData.m_boneTransformations.size() == 0)
				mat->SetBool(UF_BOOL_SKINNED, false);
			else
				mat->SetBool(UF_BOOL_SKINNED, true);

			for (int i = 0; i < modelData.m_boneTransformations.size(); i++)
				mat->SetMatrix4(std::string(UF_BONE_MATRICES) + "[" + std::to_string(i) + "]", modelData.m_boneTransformations[i]);

			m_renderEngine->UpdateShaderData(mat);
	
			m_renderDevice->Draw(vertexArray->GetID(), drawParams, numTransforms, vertexArray->GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.m_models.clear();
				modelData.m_boneTransformations.clear();
			}
		}
	}

	void MeshRendererSystem::FlushSingleRenderer(ECS::MeshRendererComponent& mrc, ECS::EntityDataComponent& data, Graphics::DrawParams drawParams)
	{
		if (!Graphics::Model::ModelExists(mrc.m_modelID))
		{
			LINA_WARN("Mesh or material does not exists for this renderer, aborting single flush.");
			return;
		}


		Graphics::Model& model = Graphics::Model::GetModel(mrc.m_modelID);
		for (int i = 0; i < model.GetMeshes().size(); i++)
		{
			if (!Graphics::Material::MaterialExists(mrc.m_materialID)) continue;

			Graphics::Material& mat = Graphics::Material::GetMaterial(mrc.m_materialID);

			auto& mesh = model.GetMeshes()[i];
			auto& va = mesh.GetVertexArray();
			const Matrix model = data.ToMatrix();
			va.UpdateBuffer(7, &model[0][0], sizeof(Matrix));
			va.UpdateBuffer(8, &data.ToMatrix().Inverse().Transpose()[0][0], sizeof(Matrix));
			m_renderEngine->UpdateShaderData(&mat);
			m_renderDevice->Draw(va.GetID(), drawParams, 1, va.GetIndexCount(), false);
		}


	}

	void MeshRendererSystem::OnModelRendererRemoved(entt::registry& reg, entt::entity ent)
	{
		m_ecs->DestroyAllChildren(ent);
	}

	void MeshRendererSystem::FlushTransparent(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
	{
		// When flushed, all the data is delegated to the render device to do the actual
		// drawing. Then the data is cleared if complete flush is requested.

		// Empty out the queue
		while (!m_transparentRenderBatch.empty())
		{
			BatchPair pair = m_transparentRenderBatch.top();

			Graphics::BatchDrawData& drawData = std::get<0>(pair);
			Graphics::BatchModelData& modelData = std::get<1>(pair);
			size_t numTransforms = modelData.m_models.size();
			if (numTransforms == 0) continue;

			Graphics::VertexArray* vertexArray = drawData.m_vertexArray;
			Matrix* models = &modelData.m_models[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? drawData.m_material : overrideMaterial;

			// Draw call.
			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(7, models, numTransforms * sizeof(Matrix));

			if (modelData.m_boneTransformations.size() == 0)
				mat->SetBool(UF_BOOL_SKINNED, false);
			else
				mat->SetBool(UF_BOOL_SKINNED, true);

			for (int i = 0; i < modelData.m_boneTransformations.size(); i++)
				mat->SetMatrix4(std::string(UF_BONE_MATRICES) + "[" + std::to_string(i) + "]", modelData.m_boneTransformations[i]);
			m_renderEngine->UpdateShaderData(mat);
			m_renderDevice->Draw(vertexArray->GetID(), drawParams, numTransforms, vertexArray->GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.m_models.clear();
				modelData.m_boneTransformations.clear();
			}

			m_transparentRenderBatch.pop();
		}

	}

}