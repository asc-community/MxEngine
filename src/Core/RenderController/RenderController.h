// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#pragma once

#include "Utilities/Memory/Memory.h"
#include "Utilities/Array/ArrayView.h"
#include "Core/Interfaces/IDrawable.h"
#include "Core/Camera/CameraController.h"
#include "Core/Interfaces/GraphicAPI/Renderer.h"
#include "Core/Lighting/DirectionalLight/DirectionalLight.h"
#include "Core/Lighting/PointLight/PointLight.h"
#include "Core/Lighting/SpotLight/SpotLight.h"
#include "Core/Skybox/Skybox.h"
#include "Core/DebugDraw/Rectangle.h"
#include "Core/DebugDraw/DebugBuffer.h"

#include <array>

namespace MxEngine
{
	struct LightSystem
	{
		DirectionalLight* Global = nullptr;
		ArrayView<PointLight> Point;
		ArrayView<SpotLight> Spot;
	};

	struct FrameBuffer;

	class RenderController
	{
		Renderer& renderer;

		size_t directionalDepthSize = 4096;
		size_t spotDepthSize = 512;
		size_t pointDepthSize = 512;
		int bloomIterations = 0;
		int samples = 1;
		float exposure = 1.0f;
		float bloomWeight = 100.0f;

		VectorInt2 viewportSize{ 0, 0 };
		UniqueRef<FrameBuffer> MSAABuffer;		
		UniqueRef<FrameBuffer> HDRBuffer;
		UniqueRef<FrameBuffer> BloomBuffers[2];
		UniqueRef<RenderBuffer> MSAARenderBuffer;
		std::array<UniqueRef<FrameBuffer>, 4> upscaleBuffers;
		UniqueRef<Texture> hdrTexture;
		UniqueRef<Texture> bloomTexture;
		UniqueRef<DebugBuffer> debugBuffer;
		UniqueRef<Rectangle> rectangle;

		static constexpr TextureFormat HDRTextureFormat = TextureFormat::RGBA16F;
	public:
		RenderController(Renderer& renderer);

		int PCFdistance = 1;
		Shader* ObjectShader = nullptr;
		Shader* DepthTextureShader = nullptr;
		Shader* DepthCubeMapShader = nullptr;
		Shader* MSAAShader = nullptr;
		Shader* HDRShader = nullptr;
		Shader* BloomShader = nullptr;
		Shader* UpscaleShader = nullptr;
		Texture* DefaultTexture = nullptr;
		Texture* DefaultHeight = nullptr;
		Texture* DefaultNormal = nullptr;
		FrameBuffer* DepthBuffer = nullptr;

		Renderer& GetRenderEngine() const;
		void Render() const;
		void Clear() const;
		void AttachDepthTexture(const Texture& texture);
		void AttachDepthCubeMap(const CubeMap& cubemap);
		void DetachDepthBuffer();
		void ToggleDepthOnlyMode(bool value) const;
		void ToggleReversedDepth(bool value) const;
		void ToggleFaceCulling(bool value, bool counterClockWise = true, bool cullBack = true) const;
		void SetAnisotropicFiltering(float value) const;
		void SetViewport(int x, int y, int width, int height);
		void DrawObject(const IDrawable& object, const CameraController& viewport) const;
		void DrawObject(const IDrawable& object, const CameraController& viewport, const LightSystem& lights, const Skybox* skybox) const;
		void DrawSkybox(const Skybox& skybox, const CameraController& viewport);
		void DrawHDRTexture(const Texture& texture, int MSAAsamples);
		void DrawPostProcessImage(const Texture& hdrTexture, const Texture& bloomTexture, float hdrExposure, int bloomIters, float bloomWeight);
		const Texture& UpscaleTexture(const Texture& texture, const VectorInt2& dist);
		void SetPCFDistance(int value);
		int GetPCFDIstance() const;
		void SetHDRExposure(float value);
		float GetHDRExposure() const;
		void SetBloomIterations(int iterations);
		int GetBloomIterations() const;
		void SetBloomWeight(float weight);
		float GetBloomWeight();
		template<typename LightSource>
		void DrawDepthTexture(const IDrawable& object, const LightSource& light) const;
		template<typename PositionedLightSource>
		void DrawDepthCubeMap(const IDrawable& object, const PositionedLightSource& light, float distance) const;
		template<typename LightSource>
		size_t GetDepthBufferSize() const;
		template<typename LightSource>
		void SetDepthBufferSize(size_t size);
		void SetMSAASampling(size_t samples, int viewportWidth, int viewportHeight);
		int getMSAASamples() const;
		void AttachDrawBuffer();
		void DetachDrawBuffer();
		DebugBuffer& GetDebugBuffer();
		Rectangle& GetRectangle();
		void DrawDebugBuffer(const CameraController& viewport, bool overlay = false) const;
	};

	template<typename LightSource>
	inline void RenderController::DrawDepthTexture(const IDrawable& object, const LightSource& light) const
	{
		if (!object.IsDrawable()) return;

		this->DepthTextureShader->SetUniformMat4("LightProjMatrix", light.GetMatrix());
		size_t iterator = object.GetIterator();

		auto& ModelMatrix = object.GetTransform().GetMatrix();

		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);

			this->GetRenderEngine().SetDefaultVertexAttribute(5, ModelMatrix * renderObject.GetTransform().GetMatrix());

			if (object.GetInstanceCount() == 0)
			{
				this->GetRenderEngine().DrawTriangles(renderObject.GetVAO(), renderObject.GetIBO(), *this->DepthTextureShader);
			}
			else
			{
				this->GetRenderEngine().DrawTrianglesInstanced(renderObject.GetVAO(), renderObject.GetIBO(), *this->DepthTextureShader, object.GetInstanceCount());
			}
			iterator = object.GetNext(iterator);
		}
	}

	template<typename PositionedLightSource>
	inline void RenderController::DrawDepthCubeMap(const IDrawable& object, const PositionedLightSource& light, float distance) const
	{
		if (!object.IsDrawable()) return;

		this->DepthCubeMapShader->SetUniformMat4("LightProjMatrix[0]", light.GetMatrix(0));
		this->DepthCubeMapShader->SetUniformMat4("LightProjMatrix[1]", light.GetMatrix(1));
		this->DepthCubeMapShader->SetUniformMat4("LightProjMatrix[2]", light.GetMatrix(2));
		this->DepthCubeMapShader->SetUniformMat4("LightProjMatrix[3]", light.GetMatrix(3));
		this->DepthCubeMapShader->SetUniformMat4("LightProjMatrix[4]", light.GetMatrix(4));
		this->DepthCubeMapShader->SetUniformMat4("LightProjMatrix[5]", light.GetMatrix(5));

		this->DepthCubeMapShader->SetUniformFloat("zFar", distance);
		this->DepthCubeMapShader->SetUniformVec3("lightPos", light.Position);

		size_t iterator = object.GetIterator();
		
		auto& ModelMatrix  = object.GetTransform().GetMatrix();

		while (!object.IsLast(iterator))
		{
			const auto& renderObject = object.GetCurrent(iterator);

			this->GetRenderEngine().SetDefaultVertexAttribute(5, ModelMatrix * renderObject.GetTransform().GetMatrix());

			if (object.GetInstanceCount() == 0)
			{
				this->GetRenderEngine().DrawTriangles(renderObject.GetVAO(), renderObject.GetIBO(), *this->DepthCubeMapShader);
			}
			else
			{
				this->GetRenderEngine().DrawTrianglesInstanced(renderObject.GetVAO(), renderObject.GetIBO(), *this->DepthCubeMapShader, object.GetInstanceCount());
			}
			iterator = object.GetNext(iterator);
		}
	}

	template<typename LightSource>
	inline size_t RenderController::GetDepthBufferSize() const
	{
		if constexpr (std::is_same<LightSource, DirectionalLight>::value)
			return this->directionalDepthSize;
		else if constexpr (std::is_same<LightSource, SpotLight>::value)
			return this->spotDepthSize;
		else if constexpr (std::is_same<LightSource, PointLight>::value)
			return this->pointDepthSize;
		else
			static_assert(false, "Invalid LightSource type provided as template argument");
		return 0;
	}

	template<typename LightSource>
	inline void RenderController::SetDepthBufferSize(size_t size)
	{
		size = Max(1, size);
		if constexpr (std::is_same<LightSource, DirectionalLight>::value)
			this->directionalDepthSize = size;
		else if constexpr (std::is_same<LightSource, SpotLight>::value)
			this->spotDepthSize = size;
		else if constexpr (std::is_same<LightSource, PointLight>::value)
			this->pointDepthSize = size;
		else
			static_assert(false, "Invalid LightSource type provided as template argument");
	}
}