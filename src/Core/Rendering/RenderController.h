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

#include "Utilities/Memory/Memory.h"
#include "Utilities/Array/ArrayView.h"
#include "Core/Components/Camera/CameraController.h"
#include "Core/Components/Lighting/DirectionalLight.h"
#include "Core/Components/Lighting/PointLight.h"
#include "Core/Components/Lighting/SpotLight.h"
#include "Core/Components/Rendering/Skybox.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Resources/Material.h"
#include "Platform/OpenGL/Renderer.h"
#include "DebugBuffer.h"
#include "RenderPipeline.h"

#include <array>

namespace MxEngine
{
	class RenderController
	{
		Renderer renderer;
		RenderPipeline Pipeline;

		void PrepareShadowMaps();
		void DrawSkybox(const CameraUnit& camera);
		void DrawObjects(const CameraUnit& camera, const MxVector<RenderUnit>& objects);
		void DrawDebugBuffer(const CameraUnit& camera);
		void DrawObject(const RenderUnit& unit, Texture::TextureBindId textureBindIndex, const Shader& shader);
		void DrawShadowMap(const RenderUnit& unit, const DirectionalLigthUnit& dirLight, const Shader& shader);
		void DrawShadowMap(const RenderUnit& unit, const SpotLightUnit& spotLight, const Shader& shader);
		void DrawShadowMap(const RenderUnit& unit, const PointLightUnit& pointLight, const Shader& shader);
		void AttachDepthMap(const GResource<Texture>& texture);
		void AttachDepthMap(const GResource<CubeMap>& cubemap);
		void PostProcessImage(CameraUnit& camera);
		GResource<Texture> PerformBloomIterations(const GResource<Texture>& inputBloom, uint8_t iterations);
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
		void AttachFrameBuffer(const GResource<FrameBuffer>& framebuffer);
		void AttachDefaultFrameBuffer();
		void RenderToFrameBuffer(const GResource<FrameBuffer>& framebuffer, const Shader& shader);
		void RenderToTexture(const GResource<Texture>& texture, const Shader& shader, Attachment attachment = Attachment::COLOR_ATTACHMENT0);
		
		EnvironmentUnit& GetEnvironment();
		const EnvironmentUnit& GetEnvironment() const;
		void ResetPipeline();
		void SubmitLightSource(const DirectionalLight& light, const Transform& parentTransform);
		void SubmitLightSource(const PointLight& light, const Transform& parentTransform);
		void SubmitLightSource(const SpotLight& light, const Transform& parentTransform);
		void SubmitCamera(const CameraController& controller, const Transform& parentTransform, const Skybox& skybox);
		void SubmitPrimitive(const SubMesh& object, const Material& material, const Transform& parentTransform, size_t instanceCount);
		void SubmitFinalImage(const GResource<Texture>& texture);
		void StartPipeline();
	};
}