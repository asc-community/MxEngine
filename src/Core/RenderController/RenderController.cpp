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

namespace MxEngine
{
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

	void RenderController::AttachDepthTexture(const Texture& texture)
	{
		this->DepthBuffer->AttachTexture(texture, Attachment::DEPTH_ATTACHMENT);
		this->SetViewport(0, 0, this->DepthBuffer->GetWidth(), this->DepthBuffer->GetHeight());
		this->Clear();
	}

	void RenderController::AttachDepthCubeMap(const CubeMap& cubemap)
	{
		this->DepthBuffer->AttachCubeMap(cubemap, Attachment::DEPTH_ATTACHMENT);
		this->SetViewport(0, 0, this->DepthBuffer->GetWidth(), this->DepthBuffer->GetHeight());
		this->Clear();
	}

	void RenderController::DetachDepthBuffer()
	{
		this->SetViewport(0, 0, viewportSize.x, viewportSize.y);
		this->DepthBuffer->Unbind();
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
		this->viewportSize = VectorInt2(width, height);
	}

	void RenderController::DrawObject(const IDrawable& object, const CameraController& viewport, MeshRenderer* meshRenderer) const
	{
		// probably nothing to do at all
		if (!viewport.HasCamera()) return;
		if (!object.IsDrawable()) return;

		// getting all data for easy use
		size_t iterator = object.GetIterator();
		const auto& ViewProjection = viewport.GetMatrix();

		// choosing shader and setting up data per object
		const Shader& shader = object.HasShader() ? object.GetShader() : *this->ObjectShader;

		auto& ModelMatrix  = object.GetTransform().GetMatrix();
		auto& renderColor  = object.GetRenderColor();

		shader.SetUniformMat4("ViewProjMatrix", ViewProjection);

		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			size_t materialId = renderObject.GetMaterialId();
			if (meshRenderer != nullptr && materialId < meshRenderer->Materials.size())
			{
				const Material& material = meshRenderer->Materials[materialId];

				Vector4 submeshRenderColor = MakeVector4(1.0f);
				if (materialId < meshRenderer->RenderColors.size())
					submeshRenderColor = meshRenderer->RenderColors[materialId];

				#define BIND_TEX(NAME, SLOT)         \
				if (material.NAME.IsValid())        \
					material.NAME->Bind(SLOT);       \
				else if (object.HasTexture())        \
					object.GetTexture().Bind(SLOT);  \
				else                                 \
					this->DefaultTexture->Bind(SLOT);\
				shader.SetUniformInt(#NAME, SLOT)

				BIND_TEX(map_Ka, 0);
				BIND_TEX(map_Kd, 1);

				// setting materials
				shader.SetUniformFloat("material.d", material.d);
				
				this->GetRenderEngine().SetDefaultVertexAttribute(5, ModelMatrix * renderObject.GetTransform().GetMatrix());
				this->GetRenderEngine().SetDefaultVertexAttribute(12, submeshRenderColor);

				if (object.GetInstanceCount() == 0)
				{
					this->GetRenderEngine().DrawTriangles(renderObject.GetVAO(), renderObject.GetIBO(), shader);
				}
				else
				{
					this->GetRenderEngine().DrawTrianglesInstanced(renderObject.GetVAO(), renderObject.GetIBO(), shader, object.GetInstanceCount());
				}
			}
			iterator = object.GetNext(iterator);
		}
	}

	void RenderController::DrawObject(const IDrawable& object, const CameraController& viewport, MeshRenderer* meshRenderer, const LightSystem& lights, const FogInformation& fog, const Skybox* skybox) const
	{
		// probably nothing to do at all
		if (!viewport.HasCamera()) return;
		if (!object.IsDrawable()) return;

		// getting all data for easy use
		size_t iterator = object.GetIterator();
		const auto& ViewProjection = viewport.GetMatrix();
		const auto& cameraPos = viewport.GetPosition();

		// choosing shader and setting up data per object
		const Shader& shader = object.HasShader() ? object.GetShader() : *this->ObjectShader;

		auto& ModelMatrix = object.GetTransform().GetMatrix();
		auto& NormalMatrix = object.GetTransform().GetNormalMatrix();
		auto& renderColor = object.GetRenderColor();

		shader.SetUniformMat4("ViewProjMatrix", ViewProjection);
		shader.SetUniformMat3("skyboxModelMatrix", Transpose(skybox->GetRotationMatrix()));
		shader.SetUniformVec3("viewPos", cameraPos);
		
		// fog parameters
		shader.SetUniformVec3 ("fogColor", fog.Color);
		shader.SetUniformFloat("fogDistance", fog.Distance);
		shader.SetUniformFloat("fogDensity", fog.Density);

		// set shadow mapping
		shader.SetUniformInt("PCFdistance", this->PCFdistance);
		shader.SetUniformMat4("DirLightProjMatrix", MakeBiasMatrix() * lights.Global->GetMatrix());
		for (size_t i = 0; i < lights.Spot.size(); i++)
		{
			shader.SetUniformMat4(MxFormat(FMT_STRING("SpotLightProjMatrix[{0}]"), i), MakeBiasMatrix() * lights.Spot[i].GetMatrix());
		}

		// set direction light
		shader.SetUniformVec3("dirLight.direction", lights.Global->Direction);
		shader.SetUniformVec3("dirLight.ambient", lights.Global->AmbientColor);
		shader.SetUniformVec3("dirLight.diffuse", lights.Global->DiffuseColor);
		shader.SetUniformVec3("dirLight.specular", lights.Global->SpecularColor);

		// set point lights
		shader.SetUniformInt("pointLightCount", (int)lights.Point.size());
		for (size_t i = 0; i < lights.Point.size(); i++)
		{
			shader.SetUniformVec3 (MxFormat(FMT_STRING("pointLight[{0}].position"), i), lights.Point[i].Position);
			shader.SetUniformFloat(MxFormat(FMT_STRING("pointLight[{0}].zfar"), i), lights.Point[i].FarDistance);
			shader.SetUniformVec3 (MxFormat(FMT_STRING("pointLight[{0}].K"), i), lights.Point[i].GetFactors());
			shader.SetUniformVec3 (MxFormat(FMT_STRING("pointLight[{0}].ambient"), i), lights.Point[i].AmbientColor);
			shader.SetUniformVec3 (MxFormat(FMT_STRING("pointLight[{0}].diffuse"), i), lights.Point[i].DiffuseColor);
			shader.SetUniformVec3 (MxFormat(FMT_STRING("pointLight[{0}].specular"), i), lights.Point[i].SpecularColor);
		}

		// set spot lights
		shader.SetUniformInt("spotLightCount", (int)lights.Spot.size());
		for (size_t i = 0; i < lights.Spot.size(); i++)
		{
			shader.SetUniformVec3 (MxFormat(FMT_STRING("spotLight[{0}].position"), i), lights.Spot[i].Position);
			shader.SetUniformVec3 (MxFormat(FMT_STRING("spotLight[{0}].ambient"), i), lights.Spot[i].AmbientColor);
			shader.SetUniformVec3 (MxFormat(FMT_STRING("spotLight[{0}].diffuse"), i), lights.Spot[i].DiffuseColor);
			shader.SetUniformVec3 (MxFormat(FMT_STRING("spotLight[{0}].specular"), i), lights.Spot[i].SpecularColor);
			shader.SetUniformVec3 (MxFormat(FMT_STRING("spotLight[{0}].direction"), i), lights.Spot[i].GetDirection());
			shader.SetUniformFloat(MxFormat(FMT_STRING("spotLight[{0}].innerAngle"), i), lights.Spot[i].GetInnerCos());
			shader.SetUniformFloat(MxFormat(FMT_STRING("spotLight[{0}].outerAngle"), i), lights.Spot[i].GetOuterCos());
		}

		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);
			size_t materialId = renderObject.GetMaterialId();
			if (meshRenderer != nullptr && materialId < meshRenderer->Materials.size())
			{
				const Material& material = meshRenderer->Materials[materialId];

				Vector4 submeshRenderColor = renderColor;
				if(materialId < meshRenderer->RenderColors.size())
					submeshRenderColor *= meshRenderer->RenderColors[materialId];

				#define BIND_TEX(NAME, SLOT)\
				if (material.NAME.IsValid())            \
					material.NAME->Bind(SLOT);           \
				else if (object.HasTexture())            \
					object.GetTexture().Bind(SLOT);      \
				else                                     \
					this->DefaultTexture->Bind(SLOT);\
				shader.SetUniformInt(#NAME, SLOT)

				BIND_TEX(map_Ka, 0);
				BIND_TEX(map_Kd, 1);
				BIND_TEX(map_Ks, 2);
				BIND_TEX(map_Ke, 3);

				if (material.map_normal.IsValid())
					material.map_normal->Bind(4);
				else
					this->DefaultNormal->Bind(4);
				shader.SetUniformInt("map_normal", 4);

				if (material.map_height.IsValid())
					material.map_height->Bind(5);
				else
					this->DefaultHeight->Bind(5);
				shader.SetUniformInt("map_height", 5);

				int shadowMapsStartIdx = 6;
				lights.Global->GetDepthTexture()->Bind(shadowMapsStartIdx);
				shader.SetUniformInt("map_dirLight_shadow", shadowMapsStartIdx);

				for (int i = 0; i < lights.Spot.size(); i++)
				{
					int bindIndex = shadowMapsStartIdx + 1 + i;
					lights.Spot[i].GetDepthTexture()->Bind(bindIndex);
					shader.SetUniformInt(MxFormat(FMT_STRING("map_spotLight_shadow[{0}]"), i), bindIndex);
				}

				for (int i = 0; i < lights.Point.size(); i++)
				{
					int bindIndex = (shadowMapsStartIdx + 1 + (int)lights.Spot.size()) + i;
					lights.Point[i].GetDepthCubeMap()->Bind(bindIndex);
					shader.SetUniformInt(MxFormat(FMT_STRING("map_pointLight_shadow[{0}]"), i), bindIndex);
				}

				// dont ask - OpenGL requires all samplerCubes to be bound
				constexpr size_t MAX_POINT_SOURCES = 2;
				for (int i = (int)lights.Point.size(); i < MAX_POINT_SOURCES; i++)
				{
					int bindIndex = int(shadowMapsStartIdx + 1 + lights.Spot.size() + lights.Point.size()) + i;
					lights.Global->GetDepthTexture()->Bind(bindIndex);
					shader.SetUniformInt(MxFormat(FMT_STRING("map_pointLight_shadow[{0}]"), i), bindIndex);
				}

				int bindIndex = int(6 + lights.Spot.size() + MAX_POINT_SOURCES);
				if (skybox->SkyboxTexture.IsValid()) // TODO: what should we do if no skybox exists for scene?
					skybox->SkyboxTexture->Bind(bindIndex);
				shader.SetUniformInt("map_skybox", bindIndex);

				// setting materials (ka, kd not used for now
				// shader.SetUniformVec3("material.Ka", material.Ka);
				// shader.SetUniformVec3("material.Kd", material.Kd);
				shader.SetUniformVec3("material.Ks", material.Ks);
				shader.SetUniformVec3("material.Ke", material.Ke);
				shader.SetUniformFloat("material.Ns", material.Ns);
				shader.SetUniformFloat("material.d", material.d);
				shader.SetUniformFloat("material.refl", material.reflection);

				shader.SetUniformFloat("Ka", material.f_Ka);
				shader.SetUniformFloat("Kd", material.f_Kd);
				shader.SetUniformVec3("displacement", object.GetTransform().GetScale() * renderObject.GetTransform().GetScale() * material.displacement);

				this->GetRenderEngine().SetDefaultVertexAttribute(5, ModelMatrix * renderObject.GetTransform().GetMatrix());
				this->GetRenderEngine().SetDefaultVertexAttribute(9, NormalMatrix * renderObject.GetTransform().GetNormalMatrix());
				this->GetRenderEngine().SetDefaultVertexAttribute(12, submeshRenderColor);

				if (object.GetInstanceCount() == 0)
				{
					this->GetRenderEngine().DrawTriangles(renderObject.GetVAO(), renderObject.GetIBO(), shader);
				}
				else
				{
					this->GetRenderEngine().DrawTrianglesInstanced(renderObject.GetVAO(), renderObject.GetIBO(), shader, object.GetInstanceCount());
				}
			}
			iterator = object.GetNext(iterator);
		}
	}

	void RenderController::DrawSkybox(const Skybox& skybox, const CameraController& viewport, const FogInformation& fog)
	{
		if (!viewport.HasCamera()) return;
		if (!skybox.SkyboxTexture.IsValid()) return;

		auto& shader = skybox.GetShader();
		auto View = (Matrix3x3)viewport.GetCamera().GetViewMatrix();
		auto Projection = viewport.GetCamera().GetProjectionMatrix();
		shader.SetUniformMat4("ViewProjection", Projection * (Matrix4x4)View);
		shader.SetUniformMat3("Rotation", skybox.GetRotationMatrix());
		shader.SetUniformVec3("fogColor", fog.Color);
		shader.SetUniformFloat("fogDistance", fog.Distance);
		shader.SetUniformFloat("fogDensity", fog.Density);

		skybox.SkyboxTexture->Bind(0);
		shader.SetUniformInt("skybox", 0);

		this->GetRenderEngine().DrawTriangles(skybox.GetVAO(), skybox.GetVBO().GetSize(), shader);
	}

	void RenderController::DrawHDRTexture(const Texture& texture, int MSAAsamples)
	{
		texture.Bind(0); // bind hdr texture
		this->MSAAShader->SetUniformInt("HDRtexture", 0);
		this->MSAAShader->SetUniformInt("msaa_samples", MSAAsamples);
		auto& rectangle = this->GetRectangle();

		this->GetRenderEngine().DrawTriangles(rectangle.GetVAO(), rectangle.VertexCount, *this->MSAAShader);
	}

	void RenderController::DrawPostProcessImage(const Texture& hdrTexture, const Texture& bloomTexture, float hdrExposure, int bloomIters, float bloomWeight)
	{
		bool horizontalKernel = false;
		bool noIterations = true;
		auto& rectangle = this->GetRectangle();

		bloomIters = bloomIters + bloomIters % 2;

		this->GetRenderEngine().SetViewport(0, 0, (int)this->BloomBuffers[0]->GetAttachedTexture().GetWidth(), (int)this->BloomBuffers[0]->GetAttachedTexture().GetHeight());
		this->BloomShader->SetUniformInt("BloomTexture", 0);
		for (int i = 0; i < bloomIters; i++)
		{
			this->BloomBuffers[(size_t)horizontalKernel]->Bind();
			if (noIterations)
			{
				bloomTexture.Bind(0); 
				noIterations = false;
			}
			else
			{
				this->BloomBuffers[(size_t)!horizontalKernel]->GetAttachedTexture().Bind(0);
			}
			this->BloomShader->SetUniformInt("horizontalKernel", (int)horizontalKernel);

			this->GetRenderEngine().DrawTriangles(rectangle.GetVAO(), rectangle.VertexCount, *this->BloomShader);

			horizontalKernel = !horizontalKernel;
		}
		this->BloomBuffers[1]->Unbind();


		if (bloomIters > 0)
		{
			//auto& bloomTexture = this->UpscaleTexture(*this->BloomBuffers[1]->GetAttachedTexture(), viewportSize);
			this->BloomBuffers[1]->GetAttachedTexture().Bind(1);
		}
		else
		{
			hdrTexture.Bind(1); // if no bloom was performed, use hdrTexture
			bloomWeight = 0.0f;
		}
		this->GetRenderEngine().SetViewport(0, 0, viewportSize.x, viewportSize.y);
		hdrTexture.Bind(0);
		this->HDRShader->SetUniformInt("HDRtexture", 0);
		this->HDRShader->SetUniformFloat("bloomWeight", bloomWeight);
		this->HDRShader->SetUniformInt("BloomTexture", 1);
		this->HDRShader->SetUniformFloat("exposure", hdrExposure);

		this->GetRenderEngine().DrawTriangles(rectangle.GetVAO(), rectangle.VertexCount, *this->HDRShader);
	}

	void RenderController::SetPCFDistance(int value)
	{
		this->PCFdistance = value;
	}

	int RenderController::GetPCFDIstance() const
	{
		return this->PCFdistance;
	}

	void RenderController::SetHDRExposure(float value)
	{
		this->exposure = Max(value, 0.0f);
	}

	float RenderController::GetHDRExposure() const
	{
		return this->exposure;
	}

	void RenderController::SetBloomIterations(int iterations)
	{
		this->bloomIterations = Max(0, iterations);
	}

	int RenderController::GetBloomIterations() const
	{
		return this->bloomIterations;
	}

	void RenderController::SetBloomWeight(float weight)
	{
		this->bloomWeight = Max(0.0f, weight);
	}

	float RenderController::GetBloomWeight()
	{
		return this->bloomWeight;
	}

	void RenderController::SetMSAASampling(size_t samples, int viewportWidth, int viewportHeight)
	{
		if(this->samples == 1 && samples != 1) this->GetRenderEngine().UseSampling(true);
		this->samples = (int)samples;

		Texture MSAATexture;
		MSAATexture.LoadMultisample(viewportWidth, viewportHeight, HDRTextureFormat, (int)samples, TextureWrap::CLAMP_TO_EDGE);

		if (!this->MSAABuffer.IsValid())
			this->MSAABuffer = GraphicFactory::Create<FrameBuffer>();
		this->MSAABuffer->AttachTexture(std::move(MSAATexture), Attachment::COLOR_ATTACHMENT0);

		if(!this->MSAARenderBuffer.IsValid())
			this->MSAARenderBuffer = GraphicFactory::Create<RenderBuffer>();
		this->MSAARenderBuffer->InitStorage(viewportWidth, viewportHeight, (int)samples);
		this->MSAARenderBuffer->LinkToFrameBuffer(*this->MSAABuffer);

		if(!this->hdrTexture.IsValid()) 
			this->hdrTexture = GraphicFactory::Create<Texture>();
		this->hdrTexture->Load(nullptr, viewportWidth, viewportHeight, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);

		if (!this->bloomTexture.IsValid())
			this->bloomTexture = GraphicFactory::Create<Texture>();
		this->bloomTexture->Load(nullptr, viewportWidth, viewportHeight, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);
			
		if(!this->HDRBuffer.IsValid())
			this->HDRBuffer = GraphicFactory::Create<FrameBuffer>();
		this->HDRBuffer->AttachTexture(*this->hdrTexture, Attachment::COLOR_ATTACHMENT0);
		this->HDRBuffer->AttachTexture(*this->bloomTexture, Attachment::COLOR_ATTACHMENT1);

		this->HDRBuffer->UseDrawBuffers(2);
		this->HDRBuffer->Validate();

		for (size_t i = 0; i < 2; i++)
		{
			if (!BloomBuffers[i].IsValid())
				BloomBuffers[i] = GraphicFactory::Create<FrameBuffer>();
			Texture texture;
			size_t width = (viewportWidth + 3) / 2 + ((viewportWidth + 1) / 2) % 2;
			size_t height = (viewportHeight + 3) / 2 + ((viewportHeight + 1) / 2) % 2;
			texture.Load(nullptr, (int)width, (int)height, HDRTextureFormat, TextureWrap::CLAMP_TO_EDGE);
			BloomBuffers[i]->AttachTexture(std::move(texture), Attachment::COLOR_ATTACHMENT0);
		}
	}

	int RenderController::getMSAASamples() const
	{
		return this->samples;
	}

	void RenderController::AttachDrawBuffer()
	{
		if (this->MSAABuffer.IsValid() &&
			this->MSAABuffer->GetWidth() == this->viewportSize.x && this->MSAABuffer->GetHeight() == this->viewportSize.y)
		{
			this->MSAABuffer->Bind();
		}
		this->Clear();
	}

	void RenderController::DetachDrawBuffer()
	{
		if (this->MSAABuffer.IsValid() &&
			this->MSAABuffer->GetWidth() == this->viewportSize.x && this->MSAABuffer->GetHeight() == this->viewportSize.y)
		{
			this->HDRBuffer->Bind();
			this->Clear();
			this->DrawHDRTexture(this->MSAABuffer->GetAttachedTexture(), this->samples);
			this->HDRBuffer->Unbind();
			this->DrawPostProcessImage(*this->hdrTexture, *this->bloomTexture, this->exposure, this->bloomIterations, this->bloomWeight);
		}
	}

	DebugBuffer& RenderController::GetDebugBuffer()
	{
		if (this->debugBuffer == nullptr)
			this->debugBuffer = MakeUnique<DebugBuffer>();
		return *this->debugBuffer;
	}

	Rectangle& RenderController::GetRectangle()
	{
		if (this->rectangle == nullptr)
			this->rectangle = MakeUnique<Rectangle>(1.0f);
		return *this->rectangle;
	}

	void RenderController::DrawDebugBuffer(const CameraController& viewport, bool overlay)
	{
		this->debugBuffer->SubmitBuffer();

		this->GetRenderEngine().UseDepthBuffer(!overlay);

		auto& shader = this->debugBuffer->GetShader();
		shader.SetUniformMat4("ViewProjMatrix", viewport.GetMatrix());

		this->GetRenderEngine().DrawLines(this->debugBuffer->GetVAO(), this->debugBuffer->GetSize(), shader);
		this->debugBuffer->ClearBuffer();

		this->GetRenderEngine().UseDepthBuffer(true);
	}
}