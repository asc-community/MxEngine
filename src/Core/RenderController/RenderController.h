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
#include "Core/Interfaces/IDrawable.h"
#include "Core/Camera/CameraController.h"
#include "Core/Interfaces/GraphicAPI/Renderer.h"
#include "Core/Lighting/DirectionalLight/DirectionalLight.h"
#include "Core/Lighting/PointLight/PointLight.h"
#include "Core/Lighting/SpotLight/SpotLight.h"

#include <array>

namespace MxEngine
{
	class RenderController
	{
		Renderer& renderer;
	public:
		template<typename LightType>
		class LightContainer
		{
			using Storage = std::vector<LightType>;
			Storage storage;
		public:
			size_t GetCount() const;
			void SetCount(size_t count);
			LightType& operator[](size_t index);
			const LightType& operator[](size_t index) const;
		};

		LightContainer<PointLight> PointLights;
		LightContainer<SpotLight> SpotLights;

		RenderController(Renderer& renderer);

		CameraController ViewPort;
		DirectionalLight GlobalLight;
		Shader* ObjectShader = nullptr;
		Shader* MeshShader = nullptr;
		Texture* DefaultTexture = nullptr;

		Renderer& GetRenderEngine() const;
		void Render() const;
		void Clear() const;
		void DrawObject(const IDrawable& object) const;
		void DrawObjectMesh(const IDrawable& object) const;
	};

	template<typename LightType>
	inline size_t RenderController::LightContainer<LightType>::GetCount() const
	{
		return storage.size();
	}

	template<typename LightType>
	inline void RenderController::LightContainer<LightType>::SetCount(size_t count)
	{
		storage.resize(count);
	}

	template<typename LightType>
	inline LightType& RenderController::LightContainer<LightType>::operator[](size_t index)
	{
		assert(index < storage.size());
		return storage[index];
	}
	
	template<typename LightType>
	inline const LightType& RenderController::LightContainer<LightType>::operator[](size_t index) const
	{
		assert(index < storage.size());
		return storage[index];
	}
}