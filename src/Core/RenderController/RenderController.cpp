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
			const auto& directionalLightShader = this->Pipeline.Environment.ShadowMapShader;
			for (const auto& directionalLight : this->Pipeline.Lighting.DirectionalLights)
			{
				this->AttachDepthMap(directionalLight.ShadowMap);
				for (const auto& renderUnit : this->Pipeline.RenderUnits)
				{
					this->DrawShadowMap(renderUnit, directionalLight, *directionalLightShader);
				}
			}
		}

		{
			MAKE_SCOPE_PROFILER("RenderController::PrepareSpotLightMaps()");
			const auto& spotLightShader = this->Pipeline.Environment.ShadowMapShader;
			for (const auto& spotLight : this->Pipeline.Lighting.SpotLights)
			{
				this->AttachDepthMap(spotLight.ShadowMap);
				for (const auto& renderUnit : this->Pipeline.RenderUnits)
				{
					this->DrawShadowMap(renderUnit, spotLight, *spotLightShader);
				}
			}
		}

		{
			MAKE_SCOPE_PROFILER("RenderController::PreparePointLightMaps()");
			const auto& pointLightShader = this->Pipeline.Environment.ShadowCubeMapShader;
			for (const auto& pointLight : this->Pipeline.Lighting.PointLights)
			{
				this->AttachDepthMap(pointLight.ShadowMap);
				for (const auto& renderUnit : this->Pipeline.RenderUnits)
				{
					this->DrawShadowMap(renderUnit, pointLight, *pointLightShader);
				}
			}
		}
		this->ToggleDepthOnlyMode(false);
	}

	void RenderController::DrawObjects(const CameraUnit& camera)
	{
		MAKE_SCOPE_PROFILER("RenderController::DrawObjects()");

		auto& shader = *this->Pipeline.Environment.MainShader;
		Texture::TextureBindId textureBindIndex = 0;

		{
			MAKE_SCOPE_PROFILER("RenderController::SubmitShaderGlobalUniforms()");
			// camera parameters
			shader.SetUniformMat4("ViewProjMatrix", camera.ViewProjMatrix);
			shader.SetUniformVec3("viewPos", camera.ViewportPosition);

			// fog parameters
			shader.SetUniformVec3("fogColor", this->Pipeline.Environment.FogColor);
			shader.SetUniformFloat("fogDistance", this->Pipeline.Environment.FogDistance);
			shader.SetUniformFloat("fogDensity", this->Pipeline.Environment.FogDensity);

			// shadow smoothing
			shader.SetUniformInt("PCFdistance", this->Pipeline.Environment.ShadowBlurIterations);

			// skybox parameters
			shader.SetUniformMat3("skyboxModelMatrix", camera.InversedSkyboxRotation);
			camera.SkyboxMap->Bind(textureBindIndex);
			shader.SetUniformInt("map_skybox", textureBindIndex);
			textureBindIndex++;

			// directional lights
			{
				MAKE_SCOPE_PROFILER("RenderController::SubmitShaderDirectionalLightUniforms()");
				shader.SetUniformInt("dirLightCount", (int)this->Pipeline.Lighting.DirectionalLights.size());
				for (size_t i = 0; i < this->Pipeline.Lighting.DirectionalLights.size(); i++)
				{
					const auto& directionalLight = this->Pipeline.Lighting.DirectionalLights[i];

					shader.SetUniformMat4(MxFormat("DirLightProjMatrix[{0}]", i), directionalLight.BiasedProjectionMatrix);
					shader.SetUniformVec3(MxFormat("dirLight[{0}].direction", i), directionalLight.Direction);
					shader.SetUniformVec3(MxFormat("dirLight[{0}].ambient", i), directionalLight.AmbientColor);
					shader.SetUniformVec3(MxFormat("dirLight[{0}].diffuse", i), directionalLight.DiffuseColor);
					shader.SetUniformVec3(MxFormat("dirLight[{0}].specular", i), directionalLight.SpecularColor);

					directionalLight.ShadowMap->Bind(textureBindIndex);
					shader.SetUniformInt(MxFormat("map_dirLight_shadow[{0}]", i), textureBindIndex);
					textureBindIndex++;
				}
			}

			// spot lights
			{
				MAKE_SCOPE_PROFILER("RenderController::SubmitShaderSpotLightUniforms()");
				shader.SetUniformInt("spotLightCount", (int)this->Pipeline.Lighting.SpotLights.size());
				for (size_t i = 0; i < this->Pipeline.Lighting.SpotLights.size(); i++)
				{
					const auto& spotLight = this->Pipeline.Lighting.SpotLights[i];

					shader.SetUniformMat4(MxFormat("SpotLightProjMatrix[{0}]", i), spotLight.BiasedProjectionMatrix);
					shader.SetUniformVec3(MxFormat("spotLight[{0}].position", i), spotLight.Position);
					shader.SetUniformVec3(MxFormat("spotLight[{0}].ambient", i), spotLight.AmbientColor);
					shader.SetUniformVec3(MxFormat("spotLight[{0}].diffuse", i), spotLight.DiffuseColor);
					shader.SetUniformVec3(MxFormat("spotLight[{0}].specular", i), spotLight.SpecularColor);
					shader.SetUniformVec3(MxFormat("spotLight[{0}].direction", i), spotLight.Direction);
					shader.SetUniformFloat(MxFormat("spotLight[{0}].innerAngle", i), spotLight.InnerAngleCos);
					shader.SetUniformFloat(MxFormat("spotLight[{0}].outerAngle", i), spotLight.OuterAngleCos);

					spotLight.ShadowMap->Bind(textureBindIndex);
					shader.SetUniformInt(MxFormat("map_spotLight_shadow[{0}]", i), textureBindIndex);
					textureBindIndex++;
				}
			}

			// point lights
			{
				MAKE_SCOPE_PROFILER("RenderController::SubmitShaderPointLightUniforms()");
				shader.SetUniformInt("pointLightCount", (int)this->Pipeline.Lighting.PointLights.size());
				for (size_t i = 0; i < this->Pipeline.Lighting.PointLights.size(); i++)
				{
					const auto& pointLight = this->Pipeline.Lighting.PointLights[i];

					shader.SetUniformVec3(MxFormat("pointLight[{0}].position", i), pointLight.Position);
					shader.SetUniformFloat(MxFormat("pointLight[{0}].zfar", i), pointLight.FarDistance);
					shader.SetUniformVec3(MxFormat("pointLight[{0}].K", i), pointLight.Factors);
					shader.SetUniformVec3(MxFormat("pointLight[{0}].ambient", i), pointLight.AmbientColor);
					shader.SetUniformVec3(MxFormat("pointLight[{0}].diffuse", i), pointLight.DiffuseColor);
					shader.SetUniformVec3(MxFormat("pointLight[{0}].specular", i), pointLight.SpecularColor);

					pointLight.ShadowMap->Bind(textureBindIndex);
					shader.SetUniformInt(MxFormat("map_pointLight_shadow[{0}]", i), textureBindIndex);
					textureBindIndex++;
				}
			}

			// OpenGL requires all texture units to be bound, even if they are never accessed
			constexpr size_t MAX_DIR_SOURCES = 2;
			constexpr size_t MAX_SPOT_SOURCES = 8;
			constexpr size_t MAX_POINT_SOURCES = 2;
			for (int i = (int)this->Pipeline.Lighting.DirectionalLights.size(); i < MAX_DIR_SOURCES; i++)
				shader.SetUniformInt(MxFormat("map_dirLight_shadow[{0}]", i), textureBindIndex - 1);

			for (int i = (int)this->Pipeline.Lighting.SpotLights.size(); i < MAX_SPOT_SOURCES; i++)
				shader.SetUniformInt(MxFormat("map_spotLight_shadow[{0}]", i), textureBindIndex - 1);

			for (int i = (int)this->Pipeline.Lighting.PointLights.size(); i < MAX_POINT_SOURCES; i++)
				shader.SetUniformInt(MxFormat("map_pointLight_shadow[{0}]", i), textureBindIndex - 1);
		}

		MAKE_SCOPE_PROFILER("RenderController::DrawMeshPrimitives()");
		for (const auto& unit : this->Pipeline.RenderUnits)
		{
			this->DrawObject(unit, textureBindIndex, shader);
		}
	}

	void RenderController::DrawObject(const RenderUnit& unit, Texture::TextureBindId textureBindIndex, const Shader& shader)
	{
		{
			MAKE_SCOPE_PROFILER("RenderController::SubmitShaderRenderPrimitiveUniforms()");
			unit.RenderMaterial.AmbientMap->Bind(textureBindIndex);
			shader.SetUniformInt("map_Ka", textureBindIndex);
			textureBindIndex++;

			unit.RenderMaterial.DiffuseMap->Bind(textureBindIndex);
			shader.SetUniformInt("map_Kd", textureBindIndex);
			textureBindIndex++;

			unit.RenderMaterial.SpecularMap->Bind(textureBindIndex);
			shader.SetUniformInt("map_Ks", textureBindIndex);
			textureBindIndex++;

			unit.RenderMaterial.EmmisiveMap->Bind(textureBindIndex);
			shader.SetUniformInt("map_Ke", textureBindIndex);
			textureBindIndex++;

			unit.RenderMaterial.NormalMap->Bind(textureBindIndex);
			shader.SetUniformInt("map_normal", textureBindIndex);
			textureBindIndex++;

			unit.RenderMaterial.HeightMap->Bind(textureBindIndex);
			shader.SetUniformInt("map_height", textureBindIndex);
			textureBindIndex++;

			shader.SetUniformVec3("material.Ka", unit.RenderMaterial.AmbientColor);
			shader.SetUniformVec3("material.Kd", unit.RenderMaterial.DiffuseColor);
			shader.SetUniformVec3("material.Ks", unit.RenderMaterial.SpecularColor);
			shader.SetUniformVec3("material.Ke", unit.RenderMaterial.EmmisiveColor);
			shader.SetUniformFloat("material.Ns", unit.RenderMaterial.SpecularExponent);
			shader.SetUniformFloat("material.d", unit.RenderMaterial.Transparency);
			shader.SetUniformFloat("material.refl", unit.RenderMaterial.Reflection);
			shader.SetUniformFloat("displacement", unit.RenderMaterial.Displacement);

			this->GetRenderEngine().SetDefaultVertexAttribute(5, unit.ModelMatrix);
			this->GetRenderEngine().SetDefaultVertexAttribute(9, unit.NormalMatrix);
			this->GetRenderEngine().SetDefaultVertexAttribute(12, unit.RenderMaterial.BaseColor);
		}
		
		this->GetRenderEngine().DrawTrianglesInstanced(*unit.VAO, *unit.IBO, shader, unit.InstanceCount);
	}

	void RenderController::DrawShadowMap(const RenderUnit& unit, const DirectionalLigthUnit& dirLight, const Shader& shader)
	{
		shader.SetUniformMat4("LightProjMatrix", dirLight.ProjectionMatrix);

		this->GetRenderEngine().SetDefaultVertexAttribute(5, unit.ModelMatrix);
		this->GetRenderEngine().DrawTrianglesInstanced(*unit.VAO, *unit.IBO, shader, unit.InstanceCount);
	}

	void RenderController::DrawShadowMap(const RenderUnit& unit, const SpotLightUnit& spotLight, const Shader& shader)
	{
		shader.SetUniformMat4("LightProjMatrix", spotLight.ProjectionMatrix);

		this->GetRenderEngine().SetDefaultVertexAttribute(5, unit.ModelMatrix);
		this->GetRenderEngine().DrawTrianglesInstanced(*unit.VAO, *unit.IBO, shader, unit.InstanceCount);
	}

	void RenderController::DrawShadowMap(const RenderUnit& unit, const PointLightUnit& pointLight, const Shader& shader)
	{
		shader.SetUniformMat4("LightProjMatrix[0]", pointLight.ProjectionMatrices[0]);
		shader.SetUniformMat4("LightProjMatrix[1]", pointLight.ProjectionMatrices[1]);
		shader.SetUniformMat4("LightProjMatrix[2]", pointLight.ProjectionMatrices[2]);
		shader.SetUniformMat4("LightProjMatrix[3]", pointLight.ProjectionMatrices[3]);
		shader.SetUniformMat4("LightProjMatrix[4]", pointLight.ProjectionMatrices[4]);
		shader.SetUniformMat4("LightProjMatrix[5]", pointLight.ProjectionMatrices[5]);
		shader.SetUniformFloat("zFar", pointLight.FarDistance);
		shader.SetUniformVec3("lightPos", pointLight.Position);

		this->GetRenderEngine().SetDefaultVertexAttribute(5, unit.ModelMatrix);
		this->GetRenderEngine().DrawTrianglesInstanced(*unit.VAO, *unit.IBO, shader, unit.InstanceCount);
	}

	GResource<Texture> RenderController::PerformBloomIterations(const GResource<Texture>& inputBloom, uint8_t iterations)
	{
		if (iterations == 0) return this->Pipeline.Environment.DefaultBlackMap;
		MAKE_SCOPE_PROFILER("RenderController::PerformBloomIterarations()");

		auto& bloomBuffers = this->Pipeline.Environment.BloomBuffers;

		MX_ASSERT(bloomBuffers[0].IsValid());
		MX_ASSERT(bloomBuffers[1].IsValid());
		MX_ASSERT(bloomBuffers[0]->HasTextureAttached());
		MX_ASSERT(bloomBuffers[1]->HasTextureAttached());
		MX_ASSERT(iterations % 2 == 0);

		auto& rectangle = this->Pipeline.Environment.RectangularObject;
		auto& shader = *this->Pipeline.Environment.BloomShader;

		shader.SetUniformInt("BloomTexture", 0);
		for (uint8_t i = 0; i < iterations; i++)
		{
			auto& target = bloomBuffers[ (i & 1)];
			auto& source = bloomBuffers[!(i & 1)];
			this->AttachFrameBuffer(target);
			shader.SetUniformInt("horizontalKernel", int(i & 1));

			if (i == 0)
				inputBloom->Bind(0);
			else
				GetAttachedTexture(source)->Bind(0);

			this->GetRenderEngine().DrawTriangles(rectangle.GetVAO(), rectangle.VertexCount, shader);
		}

		return GetAttachedTexture(bloomBuffers.back());
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

	void RenderController::AttachDepthMap(const GResource<Texture>& texture)
	{
		auto& framebuffer = this->Pipeline.Environment.DepthFrameBuffer;
		framebuffer->AttachTexture(texture, Attachment::DEPTH_ATTACHMENT);
		this->AttachFrameBuffer(framebuffer);
	}

	void RenderController::AttachDepthMap(const GResource<CubeMap>& cubemap)
	{
		auto& framebuffer = this->Pipeline.Environment.DepthFrameBuffer;
		framebuffer->AttachCubeMap(cubemap, Attachment::DEPTH_ATTACHMENT);
		this->AttachFrameBuffer(framebuffer);
	}

	void RenderController::AttachFrameBuffer(const GResource<FrameBuffer>& framebuffer)
	{
		framebuffer->Bind();
		this->SetViewport(0, 0, (int)framebuffer->GetWidth(), (int)framebuffer->GetHeight());
		this->Clear();
	}

	void RenderController::AttachDefaultFrameBuffer()
	{
		// we can unbind any buffer to set target framebuffer to default one (id = 0)
		// doing this allows us to render to application window
		this->Pipeline.Environment.HDRFrameBuffer->Unbind();
		this->SetViewport(0, 0, this->Pipeline.Environment.Viewport.x, this->Pipeline.Environment.Viewport.y);
		this->Clear();
	}

	void RenderController::PostProcessImage(CameraUnit& camera)
	{
		MAKE_SCOPE_PROFILER("RenderController::PostProcessImage()");

		auto& rectangle = this->Pipeline.Environment.RectangularObject;
		{
			MAKE_SCOPE_PROFILER("RenderController::SplitCameraHDRTexture()");
			MX_ASSERT(camera.AttachedFrameBuffer->HasTextureAttached()); // TODO: think what to do with cubemap
			const auto& inputTexture = *GetAttachedTexture(camera.AttachedFrameBuffer);
			this->AttachFrameBuffer(this->Pipeline.Environment.HDRFrameBuffer);

			MX_ASSERT(inputTexture.IsMultisampled()); // TODO: handle case when texture is not multisampled - other shader should be used to split it

			// here we take HDR MSAA image from camera and split it into to HDR images - one with base color, other with bloom
			inputTexture.Bind(0);
			auto& msaaShader = *this->Pipeline.Environment.MSAAHDRSplitShader;
			msaaShader.SetUniformInt("HDRtexture", 0);
			msaaShader.SetUniformInt("msaa_samples", inputTexture.GetSampleCount());

			this->GetRenderEngine().DrawTriangles(rectangle.GetVAO(), rectangle.VertexCount, msaaShader);
		}

		// here we use bloom enhanced texture and hdr texture obtained from HDRFrameBuffer to combine them into final image
		auto& bloomEnhancedTexture = *PerformBloomIterations(this->Pipeline.Environment.BloomHDRMap, camera.BloomIterations);
		auto& combineShader = *this->Pipeline.Environment.HDRBloomCombineHDRShader;
		this->Pipeline.Environment.PostProcessFrameBuffer->AttachTexture(camera.OutputTexture);
		this->AttachFrameBuffer(this->Pipeline.Environment.PostProcessFrameBuffer);
		auto& mainTexture = *GetAttachedTexture(this->Pipeline.Environment.HDRFrameBuffer);

		combineShader.SetUniformInt("HDRtexture", 0);
		combineShader.SetUniformFloat("bloomWeight", camera.BloomWeight);
		combineShader.SetUniformInt("BloomTexture", 1);
		combineShader.SetUniformFloat("exposure", camera.Exposure);
		mainTexture.Bind(0);
		bloomEnhancedTexture.Bind(1);

		this->GetRenderEngine().DrawTriangles(rectangle.GetVAO(), rectangle.VertexCount, combineShader);

		MAKE_SCOPE_PROFILER("CameraTexture::GenerateMipmaps()");
		camera.OutputTexture->GenerateMipmaps();
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

		shader.SetUniformMat4("StaticViewProjection", camera.StaticViewProjMatrix);
		shader.SetUniformMat3("Rotation", Transpose(camera.InversedSkyboxRotation));
		shader.SetUniformVec3("fogColor", this->Pipeline.Environment.FogColor);
		shader.SetUniformFloat("fogDistance", this->Pipeline.Environment.FogDistance);
		shader.SetUniformFloat("fogDensity", this->Pipeline.Environment.FogDensity);

		camera.SkyboxMap->Bind(0);
		shader.SetUniformInt("skybox", 0);

		this->GetRenderEngine().DrawTriangles(skybox.GetVAO(), skybox.VertexCount, shader);
	}

	void RenderController::DrawDebugBuffer(const CameraUnit& camera)
	{
		if (this->Pipeline.Environment.DebugBufferObject.VertexCount == 0) return;
		MAKE_SCOPE_PROFILER("RenderController::DrawDebugBuffer()");

		this->GetRenderEngine().UseDepthBuffer(!this->Pipeline.Environment.OverlayDebugDraws);

		auto& shader = *this->Pipeline.Environment.DebugDrawShader;
		shader.SetUniformMat4("ViewProjMatrix", camera.ViewProjMatrix);

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
		this->Pipeline.RenderUnits.clear();
		this->Pipeline.Cameras.clear();
	}

	void RenderController::SubmitLightSource(const DirectionalLight& light, const Transform& parentTransform)
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

	void RenderController::SubmitLightSource(const PointLight& light, const Transform& parentTransform)
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

	void RenderController::SubmitLightSource(const SpotLight& light, const Transform& parentTransform)
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

	void RenderController::SubmitCamera(const CameraController& controller, const Transform& parentTransform, const Skybox& skybox)
	{
		auto& camera = this->Pipeline.Cameras.emplace_back();

		camera.ViewportPosition = parentTransform.GetPosition();
		camera.ViewProjMatrix = controller.GetMatrix(parentTransform.GetPosition());
		camera.StaticViewProjMatrix = controller.GetStaticMatrix();
		camera.IsPerspective = controller.GetCamera().IsPerspective();
		camera.AttachedFrameBuffer = controller.GetFrameBuffer();
		camera.BloomIterations = (uint8_t)controller.GetBloomIterations();
		camera.BloomWeight = controller.GetBloomWeight();
		camera.Exposure = controller.GetExposure();
		camera.SkyboxMap = skybox.Texture;
		camera.HasSkybox = skybox.Texture.IsValid();
		camera.InversedSkyboxRotation = Transpose(ToMatrix(parentTransform.GetRotation()));
		camera.OutputTexture = controller.GetTexture();
	}

    void RenderController::SubmitPrimitive(const SubMesh& object, const Material& material, const Transform& parentTransform, size_t instanceCount)
    {
		auto& primitive = this->Pipeline.RenderUnits.emplace_back();

		primitive.VAO = object.MeshData.GetVAO();
		primitive.IBO = object.MeshData.GetIBO();
		primitive.RenderMaterial = material;
		primitive.ModelMatrix  = parentTransform.GetMatrix() * object.GetTransform()->GetMatrix();
		primitive.NormalMatrix = parentTransform.GetNormalMatrix() * object.GetTransform()->GetNormalMatrix();
		primitive.InstanceCount = instanceCount;

		// we need to change displacement to account object scale, so we take average of object scale components as multiplier
		primitive.RenderMaterial.Displacement *= Dot(parentTransform.GetScale() * object.GetTransform()->GetScale(), MakeVector3(1.0f / 3.0f));
		// set default textures if they are not exist
		if (!primitive.RenderMaterial.AmbientMap.IsValid())  primitive.RenderMaterial.AmbientMap  = this->Pipeline.Environment.DefaultMaterialMap;
		if (!primitive.RenderMaterial.DiffuseMap.IsValid())  primitive.RenderMaterial.DiffuseMap  = this->Pipeline.Environment.DefaultMaterialMap;
		if (!primitive.RenderMaterial.SpecularMap.IsValid()) primitive.RenderMaterial.SpecularMap = this->Pipeline.Environment.DefaultMaterialMap;
		if (!primitive.RenderMaterial.EmmisiveMap.IsValid()) primitive.RenderMaterial.EmmisiveMap = this->Pipeline.Environment.DefaultMaterialMap;
		if (!primitive.RenderMaterial.NormalMap.IsValid())   primitive.RenderMaterial.NormalMap   = this->Pipeline.Environment.DefaultNormalMap;
		if (!primitive.RenderMaterial.HeightMap.IsValid())   primitive.RenderMaterial.HeightMap   = this->Pipeline.Environment.DefaultHeightMap;
    }

	void RenderController::SubmitFinalImage(const GResource<Texture>& texture)
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
			this->AttachDefaultFrameBuffer();
			return;
		}

		MX_ASSERT(this->Pipeline.Environment.MainCameraIndex < this->Pipeline.Cameras.size());

		this->PrepareShadowMaps();

		for (auto& camera : this->Pipeline.Cameras)
		{
			this->ToggleReversedDepth(camera.IsPerspective);
			this->AttachFrameBuffer(camera.AttachedFrameBuffer);

			this->DrawObjects(camera);
			if(camera.HasSkybox) this->DrawSkybox(camera);
			this->DrawDebugBuffer(camera);

			this->PostProcessImage(camera);
		}
		
		MAKE_SCOPE_PROFILER("RenderController::SubmitFinalImage");
		this->AttachDefaultFrameBuffer();
		if (this->Pipeline.Environment.RenderToDefaultFrameBuffer)
		{
			const auto& mainCamera = this->Pipeline.Cameras[this->Pipeline.Environment.MainCameraIndex];
			MX_ASSERT(mainCamera.OutputTexture.IsValid());
			this->SubmitFinalImage(mainCamera.OutputTexture);
		}
	}
}