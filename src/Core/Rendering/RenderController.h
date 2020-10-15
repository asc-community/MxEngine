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

#pragma once

#include "Core/Resources/Material.h"
#include "Platform/OpenGL/Renderer.h"
#include "RenderPipeline.h"
#include "RenderObjects/DebugBuffer.h"

namespace MxEngine
{
	class DirectionalLight;
	class PointLight;
	class SpotLight;
	class CameraController;
	class CameraEffects;
	class CameraToneMapping;
	class CameraSSR;
	class Skybox;
	class SubMesh;
	class TransformComponent;

	class RenderController
	{
		Renderer renderer;
		RenderPipeline Pipeline;

		void PrepareShadowMaps();
		void DrawSkybox(const CameraUnit& camera);
		void DrawObjects(const CameraUnit& camera, const Shader& shader, const MxVector<RenderUnit>& objects);
		void DrawDebugBuffer(const CameraUnit& camera);
		void DrawObject(const RenderUnit& unit, const Shader& shader);
		void ComputeBloomEffect(CameraUnit& camera);
		void ComputeAmbientOcclusion(CameraUnit& camera);
		TextureHandle ComputeAverageWhite(CameraUnit& camera);
		void PerformPostProcessing(CameraUnit& camera);
		void DrawDirectionalLights(CameraUnit& camera);
		void PerformLightPass(CameraUnit& camera);
		void DrawTransparentObjects(CameraUnit& camera);
		void ApplyFogEffect(CameraUnit& camera, TextureHandle& input, TextureHandle& output);
		void ApplyChromaticAbberation(CameraUnit& camera, TextureHandle& input, TextureHandle& output);
		void ApplySSR(CameraUnit& camera, TextureHandle& input, TextureHandle& output);
		void ApplyHDRToLDRConversion(CameraUnit& camera, TextureHandle& input, TextureHandle& output);
		void ApplyFXAA(CameraUnit& camera, TextureHandle& input, TextureHandle& output);
		void ApplyVignette(CameraUnit& camera, TextureHandle& input, TextureHandle& output);
		void DrawShadowedPointLights(CameraUnit& camera);
		void DrawShadowedSpotLights(CameraUnit& camera);
		void DrawNonShadowedPointLights(CameraUnit& camera);
		void DrawNonShadowedSpotLights(CameraUnit& camera);
		void BindGBuffer(const CameraUnit& camera, const Shader& shader);
		void BindFogInformation(const Shader& shader);
		void BindCameraInformation(const CameraUnit& camera, const Shader& shader);
	public:
		const Renderer& GetRenderEngine() const;
		Renderer& GetRenderEngine();
		void Render() const;
		void Clear() const;
		void ToggleDepthOnlyMode(bool value);
		void ToggleReversedDepth(bool value);
		void ToggleFaceCulling(bool value, bool counterClockWise = true, bool cullBack = true);
		void SetAnisotropicFiltering(float value);
		void SetViewport(int x, int y, int width, int height);
		void AttachFrameBuffer(const FrameBufferHandle& framebuffer);
		void AttachFrameBufferNoClear(const FrameBufferHandle& framebuffer);
		void AttachDefaultFrameBuffer();
		void AttachDepthMap(const TextureHandle& texture);
		void AttachDepthMap(const CubeMapHandle& cubemap);
		void RenderToFrameBuffer(const FrameBufferHandle& framebuffer, const ShaderHandle& shader);
		void RenderToTexture(const TextureHandle& texture, const ShaderHandle& shader, Attachment attachment = Attachment::COLOR_ATTACHMENT0);
		void CopyTexture(const TextureHandle& input, const TextureHandle& output);

		EnvironmentUnit& GetEnvironment();
		const EnvironmentUnit& GetEnvironment() const;
		LightingSystem& GetLightInformation();
		const LightingSystem& GetLightInformation() const;
		void ResetPipeline();
		void SubmitLightSource(const DirectionalLight& light, const TransformComponent& parentTransform);
		void SubmitLightSource(const PointLight& light, const TransformComponent& parentTransform);
		void SubmitLightSource(const SpotLight& light, const TransformComponent& parentTransform);
		void SubmitCamera(const CameraController& controller, const TransformComponent& parentTransform, 
			const Skybox& skybox, const CameraEffects* effects = nullptr, const CameraToneMapping* toneMapping = nullptr, const CameraSSR* ssr = nullptr);
		void SubmitPrimitive(const SubMesh& object, const Material& material, const TransformComponent& parentTransform, size_t instanceCount);
		void SubmitImage(const TextureHandle& texture);
		void StartPipeline();
		void EndPipeline();
	};
}