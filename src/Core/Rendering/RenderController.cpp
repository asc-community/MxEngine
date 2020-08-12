// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "RenderController.h"
#include "Utilities/Format/Format.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Utilities/Profiler/Profiler.h"

namespace MxEngine
{
	void RenderController::PrepareShadowMaps()
	{
		MAKE_SCOPE_PROFILER("RenderController::PrepareShadowMaps()");

		this->ToggleReversedDepth(false);
		this->ToggleDepthOnlyMode(true);

		{
			MAKE_SCOPE_PROFILER("RenderController::PrepareDirectionalLightMaps()");
			const auto& directionalLightShader = *this->Pipeline.Environment.ShadowMapShader;
			for (const auto& directionalLight : this->Pipeline.Lighting.DirectionalLights)
			{
				this->AttachDepthMap(directionalLight.ShadowMap);
				directionalLightShader.SetUniformMat4("LightProjMatrix", directionalLight.ProjectionMatrix);

				for (const auto& renderUnit : this->Pipeline.OpaqueRenderUnits)
				{
					this->DrawShadowMap(renderUnit, directionalLight, directionalLightShader);
				}
			}
		}

		{
			MAKE_SCOPE_PROFILER("RenderController::PrepareSpotLightMaps()");
			const auto& spotLightShader = *this->Pipeline.Environment.ShadowMapShader;
			for (const auto& spotLight : this->Pipeline.Lighting.SpotLights)
			{
				this->AttachDepthMap(spotLight.ShadowMap);
				spotLightShader.SetUniformMat4("LightProjMatrix", spotLight.ProjectionMatrix);

				for (const auto& renderUnit : this->Pipeline.OpaqueRenderUnits)
				{
					this->DrawShadowMap(renderUnit, spotLight, spotLightShader);
				}
			}
		}

		{
			MAKE_SCOPE_PROFILER("RenderController::PreparePointLightMaps()");
			const auto& pointLightShader = *this->Pipeline.Environment.ShadowCubeMapShader;
			for (const auto& pointLight : this->Pipeline.Lighting.PointLights)
			{
				this->AttachDepthMap(pointLight.ShadowMap);
				pointLightShader.SetUniformMat4("LightProjMatrix[0]", pointLight.ProjectionMatrices[0]);
				pointLightShader.SetUniformMat4("LightProjMatrix[1]", pointLight.ProjectionMatrices[1]);
				pointLightShader.SetUniformMat4("LightProjMatrix[2]", pointLight.ProjectionMatrices[2]);
				pointLightShader.SetUniformMat4("LightProjMatrix[3]", pointLight.ProjectionMatrices[3]);
				pointLightShader.SetUniformMat4("LightProjMatrix[4]", pointLight.ProjectionMatrices[4]);
				pointLightShader.SetUniformMat4("LightProjMatrix[5]", pointLight.ProjectionMatrices[5]);
				pointLightShader.SetUniformFloat("zFar", pointLight.FarDistance);
				pointLightShader.SetUniformVec3("lightPos", pointLight.Position);

				for (const auto& renderUnit : this->Pipeline.OpaqueRenderUnits)
				{
					this->DrawShadowMap(renderUnit, pointLight, pointLightShader);
				}
			}
		}
		this->ToggleDepthOnlyMode(false);
	}

	void RenderController::DrawObjects(const CameraUnit& camera, const MxVector<RenderUnit>& objects)
	{
		MAKE_SCOPE_PROFILER("RenderController::DrawGBuffer()");

		if (objects.empty()) return;
		auto& shader = *this->Pipeline.Environment.GBufferShader;
		shader.SetUniformMat4("ViewProjMatrix", camera.ProjectionMatrix * camera.ViewMatrix);

		for (const auto& unit : objects)
		{
			bool isUnitVisible = unit.InstanceCount > 0 || camera.Culler.IsAABBVisible(unit.MinAABB, unit.MaxAABB);

			if (isUnitVisible) this->DrawObject(unit, shader);
		}
	}

	void RenderController::DrawObject(const RenderUnit& unit, const Shader& shader)
	{
		MAKE_SCOPE_PROFILER("RenderController::SubmitShaderRenderPrimitiveUniforms()");

		Texture::TextureBindId textureBindIndex = 0;

		unit.RenderMaterial.AlbedoMap->Bind(textureBindIndex);
		shader.SetUniformInt("map_albedo", textureBindIndex);
		textureBindIndex++;

		unit.RenderMaterial.SpecularMap->Bind(textureBindIndex);
		shader.SetUniformInt("map_specular", textureBindIndex);
		textureBindIndex++;

		unit.RenderMaterial.EmmisiveMap->Bind(textureBindIndex);
		shader.SetUniformInt("map_emmisive", textureBindIndex);
		textureBindIndex++;

		unit.RenderMaterial.NormalMap->Bind(textureBindIndex);
		shader.SetUniformInt("map_normal", textureBindIndex);
		textureBindIndex++;

		unit.RenderMaterial.HeightMap->Bind(textureBindIndex);
		shader.SetUniformInt("map_height", textureBindIndex);
		textureBindIndex++;

		shader.SetUniformFloat("material.specularFactor", unit.RenderMaterial.SpecularFactor);
		shader.SetUniformFloat("material.specularIntensity", unit.RenderMaterial.SpecularIntensity);
		shader.SetUniformFloat("material.emmisive", unit.RenderMaterial.Emmision);
		shader.SetUniformFloat("material.reflection", unit.RenderMaterial.Reflection);
		shader.SetUniformFloat("displacement", unit.RenderMaterial.Displacement);

		this->GetRenderEngine().SetDefaultVertexAttribute(5, unit.ModelMatrix); //-V807
		this->GetRenderEngine().SetDefaultVertexAttribute(9, unit.NormalMatrix);
		this->GetRenderEngine().SetDefaultVertexAttribute(12, unit.RenderMaterial.BaseColor);
		
		this->GetRenderEngine().DrawTrianglesInstanced(*unit.VAO, *unit.IBO, shader, unit.InstanceCount);
	}

	void RenderController::DrawShadowMap(const RenderUnit& unit, const DirectionalLigthUnit& dirLight, const Shader& shader)
	{
		shader.SetUniformFloat("displacement", unit.RenderMaterial.Displacement);
		unit.RenderMaterial.HeightMap->Bind(0);
		shader.SetUniformInt("map_height", 0);

		this->GetRenderEngine().SetDefaultVertexAttribute(5, unit.ModelMatrix); //-V807
		this->GetRenderEngine().SetDefaultVertexAttribute(9, unit.NormalMatrix);
		this->GetRenderEngine().DrawTrianglesInstanced(*unit.VAO, *unit.IBO, shader, unit.InstanceCount);
	}

	void RenderController::DrawShadowMap(const RenderUnit& unit, const SpotLightUnit& spotLight, const Shader& shader)
	{
		shader.SetUniformFloat("displacement", unit.RenderMaterial.Displacement);
		unit.RenderMaterial.HeightMap->Bind(0);
		shader.SetUniformInt("map_height", 0);

		this->GetRenderEngine().SetDefaultVertexAttribute(5, unit.ModelMatrix); //-V807
		this->GetRenderEngine().SetDefaultVertexAttribute(9, unit.NormalMatrix);
		this->GetRenderEngine().DrawTrianglesInstanced(*unit.VAO, *unit.IBO, shader, unit.InstanceCount);
	}

	void RenderController::DrawShadowMap(const RenderUnit& unit, const PointLightUnit& pointLight, const Shader& shader)
	{
		shader.SetUniformFloat("displacement", unit.RenderMaterial.Displacement);
		unit.RenderMaterial.HeightMap->Bind(0);
		shader.SetUniformInt("map_height", 0);

		this->GetRenderEngine().SetDefaultVertexAttribute(5, unit.ModelMatrix);
		this->GetRenderEngine().DrawTrianglesInstanced(*unit.VAO, *unit.IBO, shader, unit.InstanceCount);
	}

	TextureHandle RenderController::PerformBloomIterations(const TextureHandle& inputBloom, uint8_t iterations)
	{
		if (iterations == 0) return this->Pipeline.Environment.DefaultBlackMap;
		MAKE_SCOPE_PROFILER("RenderController::PerformBloomIterarations()");

		auto& bloomBuffers = this->Pipeline.Environment.BloomBuffers;

		MX_ASSERT(bloomBuffers[0].IsValid());
		MX_ASSERT(bloomBuffers[1].IsValid());
		MX_ASSERT(bloomBuffers[0]->HasTextureAttached());
		MX_ASSERT(bloomBuffers[1]->HasTextureAttached());
		MX_ASSERT(iterations % 2 == 0);

		auto& bloomShaderHandle = this->Pipeline.Environment.BloomShader;
		auto& bloomShader = *bloomShaderHandle.GetUnchecked();

		bloomShader.SetUniformInt("BloomTexture", 0);
		for (uint8_t i = 0; i < iterations; i++)
		{
			auto& target = bloomBuffers[ (i & 1)];
			auto& source = bloomBuffers[!(i & 1)];
			bloomShader.SetUniformInt("horizontalKernel", int(i & 1));

			if (i == 0)
				inputBloom->Bind(0);
			else
				GetAttachedTexture(source)->Bind(0);

			this->RenderToFrameBuffer(target, bloomShaderHandle);
		}
		auto result = GetAttachedTexture(bloomBuffers.back());
		result->GenerateMipmaps();
		return result;
	}

	void RenderController::PerformGlobalIlluminationPass(CameraUnit& camera)
	{
		auto& illumShader = this->Pipeline.Environment.GlobalIlluminationShader;

		camera.AlbedoTexture->GenerateMipmaps();
		camera.NormalTexture->GenerateMipmaps();
		camera.MaterialTexture->GenerateMipmaps();
		camera.DepthTexture->GenerateMipmaps();

		Texture::TextureBindId textureId = 0;
		
		// bind G buffer channels
		camera.AlbedoTexture->Bind(textureId);
		illumShader->SetUniformInt("albedoTex", textureId);
		textureId++;

		camera.NormalTexture->Bind(textureId);
		illumShader->SetUniformInt("normalTex", textureId);
		textureId++;

		camera.MaterialTexture->Bind(textureId);
		illumShader->SetUniformInt("materialTex", textureId);
		textureId++;

		camera.DepthTexture->Bind(textureId);
		illumShader->SetUniformInt("depthTex", textureId);
		textureId++;

		// submit camera info
		illumShader->SetUniformVec3("viewPosition", camera.ViewportPosition);
		illumShader->SetUniformMat4("invProjMatrix", Inverse(camera.ProjectionMatrix));
		illumShader->SetUniformMat4("invViewMatrix", Inverse(camera.ViewMatrix));
		illumShader->SetUniformMat3("skyboxTransform", camera.InversedSkyboxRotation);

		camera.SkyboxMap->Bind(textureId);
		illumShader->SetUniformInt("skyboxTex", textureId);
		textureId++;

		// fod info
		illumShader->SetUniformFloat("fogDistance", this->Pipeline.Environment.FogDistance);
		illumShader->SetUniformFloat("fogDensity", this->Pipeline.Environment.FogDensity);
		illumShader->SetUniformVec3("fogColor", this->Pipeline.Environment.FogColor);

		// submit directional light information
		constexpr size_t MaxDirLightCount = 2;
		const auto& dirLights = this->Pipeline.Lighting.DirectionalLights;
		size_t lightCount = Min(MaxDirLightCount, dirLights.size());

		illumShader->SetUniformInt("lightCount", lightCount);
		illumShader->SetUniformInt("pcfDistance", this->Pipeline.Environment.ShadowBlurIterations);

		for (size_t i = 0; i < lightCount; i++)
		{
			auto& dirLight = this->Pipeline.Lighting.DirectionalLights[i];

			dirLight.ShadowMap->Bind(textureId);
			illumShader->SetUniformInt(MxFormat("lightDepthMaps[{}]", i), textureId);
			textureId++;

			illumShader->SetUniformMat4(MxFormat("lights[{}].transform", i), dirLight.BiasedProjectionMatrix);
			illumShader->SetUniformVec3(MxFormat("lights[{}].ambient", i), dirLight.AmbientColor);
			illumShader->SetUniformVec3(MxFormat("lights[{}].diffuse", i), dirLight.DiffuseColor);
			illumShader->SetUniformVec3(MxFormat("lights[{}].specular", i), dirLight.SpecularColor);
			illumShader->SetUniformVec3(MxFormat("lights[{}].direction", i), dirLight.Direction);
		}
		for (size_t i = lightCount; i < MaxDirLightCount; i++)
		{
			this->Pipeline.Environment.DefaultBlackMap->Bind(textureId);
			illumShader->SetUniformInt(MxFormat("lightDepthMaps[{}]", i), textureId);
			textureId++;
		}
		// render global illumination
		this->RenderToTexture(camera.HDRTexture, illumShader);
		
		// render skybox & debug buffer (HDR texture already attached)
		this->Pipeline.Environment.PostProcessFrameBuffer->AttachTexture(camera.DepthTexture, Attachment::DEPTH_ATTACHMENT);
		this->GetRenderEngine().UseDepthBufferMask(false);
		this->DrawSkybox(camera);
		this->DrawDebugBuffer(camera);
		this->GetRenderEngine().UseDepthBufferMask(true);
		this->Pipeline.Environment.PostProcessFrameBuffer->DetachExtraTarget(Attachment::DEPTH_ATTACHMENT);

		auto& HDRToLDRShader = this->Pipeline.Environment.HDRToLDRShader;
		camera.HDRTexture->Bind(0);
		HDRToLDRShader->SetUniformInt("HDRTex", 0);
		HDRToLDRShader->SetUniformFloat("exposure", camera.Exposure);
		
		this->RenderToTexture(camera.OutputTexture, HDRToLDRShader);
	}

	const Renderer& RenderController::GetRenderEngine() const
	{
		return this->renderer;
	}

	Renderer& RenderController::GetRenderEngine()
	{
		return this->renderer;
	}

	void RenderController::Render() const
	{
		this->GetRenderEngine().Finish();
	}

	void RenderController::Clear() const
	{
		this->GetRenderEngine().Clear();
	}

	void RenderController::AttachDepthMap(const TextureHandle& texture)
	{
		auto& framebuffer = this->Pipeline.Environment.DepthFrameBuffer;
		framebuffer->AttachTexture(texture, Attachment::DEPTH_ATTACHMENT);
		this->AttachFrameBuffer(framebuffer);
	}

	void RenderController::AttachDepthMap(const CubeMapHandle& cubemap)
	{
		auto& framebuffer = this->Pipeline.Environment.DepthFrameBuffer;
		framebuffer->AttachCubeMap(cubemap, Attachment::DEPTH_ATTACHMENT);
		this->AttachFrameBuffer(framebuffer);
	}

	void RenderController::AttachFrameBuffer(const FrameBufferHandle& framebuffer)
	{
		framebuffer->Bind();
		this->SetViewport(0, 0, (int)framebuffer->GetWidth(), (int)framebuffer->GetHeight());
		this->Clear();
	}

	void RenderController::AttachDefaultFrameBuffer()
	{
		// we can unbind any buffer to set target framebuffer to default one (id = 0)
		// doing this allows us to render to application window
		this->Pipeline.Environment.DepthFrameBuffer->Unbind();
		this->SetViewport(0, 0, this->Pipeline.Environment.Viewport.x, this->Pipeline.Environment.Viewport.y);
		this->Clear();
	}

	void RenderController::RenderToFrameBuffer(const FrameBufferHandle& framebuffer, const ShaderHandle& shader)
	{
		this->AttachFrameBuffer(framebuffer);
		auto& rectangle = this->Pipeline.Environment.RectangularObject;
		this->GetRenderEngine().DrawTriangles(rectangle.GetVAO(), rectangle.VertexCount, *shader);
	}

	void RenderController::RenderToTexture(const TextureHandle& texture, const ShaderHandle& shader, Attachment attachment)
	{
		this->Pipeline.Environment.PostProcessFrameBuffer->AttachTexture(texture, attachment);
		this->RenderToFrameBuffer(this->Pipeline.Environment.PostProcessFrameBuffer, shader);
	}

    void RenderController::ToggleDepthOnlyMode(bool value)
    {
		bool useColor = !value;
		this->GetRenderEngine().UseColorMask(useColor, useColor, useColor, useColor);
    }

	void RenderController::ToggleReversedDepth(bool value)
	{
		this->GetRenderEngine().UseReversedDepth(value);
	}

	void RenderController::ToggleFaceCulling(bool value, bool counterClockWise, bool cullBack)
	{
		this->GetRenderEngine().UseCulling(value, counterClockWise, cullBack);
	}

	void RenderController::SetAnisotropicFiltering(float value)
	{
		this->GetRenderEngine().UseAnisotropicFiltering(value);
	}

	void RenderController::SetViewport(int x, int y, int width, int height)
	{
		this->GetRenderEngine().SetViewport(x, y, width, height);
	}

	void RenderController::DrawSkybox(const CameraUnit& camera)
	{
		MAKE_SCOPE_PROFILER("RenderController::DrawSkybox()");

		auto& shader = *this->Pipeline.Environment.SkyboxShader;
		auto& skybox = this->Pipeline.Environment.SkyboxCubeObject;

		shader.SetUniformMat4("StaticViewProjection", camera.ProjectionMatrix * camera.StaticViewMatrix);
		shader.SetUniformMat3("Rotation", Transpose(camera.InversedSkyboxRotation));
		shader.SetUniformFloat("fogDistance", this->Pipeline.Environment.FogDistance);
		shader.SetUniformFloat("fogDensity", this->Pipeline.Environment.FogDensity);
		shader.SetUniformVec3("fogColor", this->Pipeline.Environment.FogColor);

		camera.SkyboxMap->Bind(0);
		shader.SetUniformInt("skybox", 0);

		this->GetRenderEngine().DrawTriangles(skybox.GetVAO(), skybox.VertexCount, shader);
	}

	void RenderController::DrawDebugBuffer(const CameraUnit& camera)
	{
		if (this->Pipeline.Environment.DebugBufferObject.VertexCount == 0) return;
		MAKE_SCOPE_PROFILER("RenderController::DrawDebugBuffer()");

		this->GetRenderEngine().UseDepthBuffer(!this->Pipeline.Environment.OverlayDebugDraws); //-V807

		auto& shader = *this->Pipeline.Environment.DebugDrawShader;
		shader.SetUniformMat4("ViewProjMatrix", camera.ProjectionMatrix * camera.ViewMatrix);

		this->GetRenderEngine().DrawLines(*this->Pipeline.Environment.DebugBufferObject.VAO, this->Pipeline.Environment.DebugBufferObject.VertexCount, shader);

		this->GetRenderEngine().UseDepthBuffer(true);
	}

	EnvironmentUnit& RenderController::GetEnvironment()
	{
		return this->Pipeline.Environment;
	}

	const EnvironmentUnit& RenderController::GetEnvironment() const
	{
		return this->Pipeline.Environment;
	}

	void RenderController::ResetPipeline()
	{
		this->Pipeline.Lighting.DirectionalLights.clear();
		this->Pipeline.Lighting.PointLights.clear();
		this->Pipeline.Lighting.SpotLights.clear();
		this->Pipeline.OpaqueRenderUnits.clear();
		this->Pipeline.TransparentRenderUnits.clear();
		this->Pipeline.Cameras.clear();
	}

	void RenderController::SubmitLightSource(const DirectionalLight& light, const TransformComponent& parentTransform)
	{
		auto& dirLight = this->Pipeline.Lighting.DirectionalLights.emplace_back();

		dirLight.AmbientColor = light.AmbientColor;
		dirLight.DiffuseColor = light.DiffuseColor;
		dirLight.SpecularColor = light.SpecularColor;
		dirLight.Direction = light.Direction;
		dirLight.ShadowMap = light.GetDepthTexture();
		dirLight.ProjectionMatrix = light.GetMatrix(parentTransform.GetPosition());
		dirLight.BiasedProjectionMatrix = MakeBiasMatrix() * light.GetMatrix(parentTransform.GetPosition());
	}

	void RenderController::SubmitLightSource(const PointLight& light, const TransformComponent& parentTransform)
	{
		auto& pointLight = this->Pipeline.Lighting.PointLights.emplace_back();

		pointLight.AmbientColor = light.AmbientColor;
		pointLight.DiffuseColor = light.DiffuseColor;
		pointLight.SpecularColor = light.SpecularColor;
		pointLight.Position = parentTransform.GetPosition();
		pointLight.ShadowMap = light.GetDepthCubeMap();
		pointLight.Factors = light.GetFactors();
		pointLight.FarDistance = light.FarDistance;

		for (size_t i = 0; i < std::size(pointLight.ProjectionMatrices); i++)
			pointLight.ProjectionMatrices[i] = light.GetMatrix(i, parentTransform.GetPosition());
	}

	void RenderController::SubmitLightSource(const SpotLight& light, const TransformComponent& parentTransform)
	{
		auto& spotLight = this->Pipeline.Lighting.SpotLights.emplace_back();

		spotLight.AmbientColor = light.AmbientColor;
		spotLight.DiffuseColor = light.DiffuseColor;
		spotLight.SpecularColor = light.SpecularColor;
		spotLight.Position = parentTransform.GetPosition();
		spotLight.Direction = light.Direction;
		spotLight.ProjectionMatrix = light.GetMatrix(parentTransform.GetPosition());
		spotLight.BiasedProjectionMatrix = MakeBiasMatrix() * light.GetMatrix(parentTransform.GetPosition());
		spotLight.InnerAngleCos = light.GetInnerCos();
		spotLight.OuterAngleCos = light.GetOuterCos();
		spotLight.ShadowMap = light.GetDepthTexture();
	}

	void RenderController::SubmitCamera(const CameraController& controller, const TransformComponent& parentTransform, const Skybox& skybox)
	{
		auto& camera = this->Pipeline.Cameras.emplace_back();

		camera.ViewportPosition       = parentTransform.GetPosition();
		camera.Culler                 = controller.GetFrustrumCuller();
		camera.ViewMatrix             = controller.GetViewMatrix(parentTransform.GetPosition());
		camera.StaticViewMatrix       = controller.GetStaticViewMatrix();
		camera.ProjectionMatrix       = controller.GetProjectionMatrix();
		camera.IsPerspective          = controller.GetCameraType() == CameraType::PERSPECTIVE;
		camera.GBuffer                = controller.GetGBuffer();
		camera.AlbedoTexture          = controller.GetAlbedoTexture();
		camera.NormalTexture          = controller.GetNormalTexture();
		camera.MaterialTexture        = controller.GetMaterialTexture();
		camera.DepthTexture           = controller.GetDepthTexture();
		camera.HDRTexture             = controller.GetHDRTexture();
		camera.BloomIterations        = (uint8_t)controller.GetBloomIterations();
		camera.BloomWeight            = controller.GetBloomWeight();
		camera.Exposure               = controller.GetExposure();
		camera.SkyboxMap              = skybox.Texture.IsValid() ? skybox.Texture : this->Pipeline.Environment.DefaultBlackCubeMap;
		camera.InversedSkyboxRotation = Transpose(ToMatrix(parentTransform.GetRotation()));
		camera.OutputTexture          = controller.GetRenderTexture();
		camera.RenderToTexture        = controller.IsRendered();
	}

    void RenderController::SubmitPrimitive(const SubMesh& object, const Material& material, const TransformComponent& parentTransform, size_t instanceCount)
    {
		RenderUnit* primitivePtr = nullptr;
		// filter transparent object to render in separate order
		if (material.Transparency < 1.0f || material.TransparencyMap.IsValid())
			primitivePtr = &this->Pipeline.TransparentRenderUnits.emplace_back();
		else
			primitivePtr = &this->Pipeline.OpaqueRenderUnits.emplace_back();
		auto& primitive = *primitivePtr;

		primitive.VAO = object.Data.GetVAO();
		primitive.IBO = object.Data.GetIBO();
		primitive.RenderMaterial = material;
		primitive.ModelMatrix  = parentTransform.GetMatrix() * object.GetTransform()->GetMatrix(); //-V807
		primitive.NormalMatrix = parentTransform.GetNormalMatrix() * object.GetTransform()->GetNormalMatrix();
		primitive.InstanceCount = instanceCount;

		// compute aabb of primitive object for later frustrum culling
		auto aabb = object.Data.GetBoundingBox() * primitive.ModelMatrix;
		primitive.MinAABB = aabb.Min;
		primitive.MaxAABB = aabb.Max;

		// we need to change displacement to account object scale, so we take average of object scale components as multiplier
		primitive.RenderMaterial.Displacement *= Dot(parentTransform.GetScale() * object.GetTransform()->GetScale(), MakeVector3(1.0f / 3.0f));
		// set default textures if they are not exist
		if (!primitive.RenderMaterial.AlbedoMap.IsValid())       primitive.RenderMaterial.AlbedoMap       = this->Pipeline.Environment.DefaultMaterialMap;
		if (!primitive.RenderMaterial.SpecularMap.IsValid())     primitive.RenderMaterial.SpecularMap     = this->Pipeline.Environment.DefaultMaterialMap;
		if (!primitive.RenderMaterial.EmmisiveMap.IsValid())     primitive.RenderMaterial.EmmisiveMap     = this->Pipeline.Environment.DefaultMaterialMap;
		if (!primitive.RenderMaterial.TransparencyMap.IsValid()) primitive.RenderMaterial.TransparencyMap = this->Pipeline.Environment.DefaultMaterialMap;
		if (!primitive.RenderMaterial.NormalMap.IsValid())       primitive.RenderMaterial.NormalMap       = this->Pipeline.Environment.DefaultNormalMap;
		if (!primitive.RenderMaterial.HeightMap.IsValid())       primitive.RenderMaterial.HeightMap       = this->Pipeline.Environment.DefaultBlackMap;
    }

	void RenderController::SubmitFinalImage(const TextureHandle& texture)
	{
		auto& finalShader = *this->Pipeline.Environment.ImageForwardShader;
		auto& rectangle = this->Pipeline.Environment.RectangularObject;

		finalShader.SetUniformInt("tex", 0);
		texture->Bind(0);

		this->GetRenderEngine().DrawTriangles(rectangle.GetVAO(), rectangle.VertexCount, finalShader);
	}

	void RenderController::StartPipeline()
	{
		MAKE_SCOPE_PROFILER("RenderController::StartPipeline()");
		if (this->Pipeline.Cameras.empty())
		{
			if(this->Pipeline.Environment.RenderToDefaultFrameBuffer)
				this->AttachDefaultFrameBuffer();
			return;
		}

		this->PrepareShadowMaps();

		for (auto& camera : this->Pipeline.Cameras)
		{
			if (!camera.RenderToTexture) continue;

			this->GetRenderEngine().UseBlending(BlendFactor::NONE, BlendFactor::NONE);
			this->ToggleReversedDepth(camera.IsPerspective);
			this->AttachFrameBuffer(camera.GBuffer);

			this->DrawObjects(camera, this->Pipeline.OpaqueRenderUnits);

			this->PerformGlobalIlluminationPass(camera);

			camera.OutputTexture->GenerateMipmaps();

			//  if (!this->Pipeline.TransparentRenderUnits.empty())
			//  {
			//  	this->GetRenderEngine().UseDepthBufferMask(false);
			//  	this->ToggleFaceCulling(false);
			//  	this->DrawObjects(camera, this->Pipeline.TransparentRenderUnits);
			//  	this->ToggleFaceCulling(true);
			//  	this->GetRenderEngine().UseDepthBufferMask(true);
			//  }
			// 
			// this->DrawDebugBuffer(camera);
		}
	}

	void RenderController::EndPipeline()
	{
		MAKE_SCOPE_PROFILER("RenderController::SubmitFinalImage");
		this->AttachDefaultFrameBuffer();
		if (this->Pipeline.Environment.RenderToDefaultFrameBuffer && this->Pipeline.Environment.MainCameraIndex < this->Pipeline.Cameras.size())
		{
			const auto& mainCamera = this->Pipeline.Cameras[this->Pipeline.Environment.MainCameraIndex];
			MX_ASSERT(mainCamera.OutputTexture.IsValid());

			this->SubmitFinalImage(mainCamera.OutputTexture);
		}
	}
}