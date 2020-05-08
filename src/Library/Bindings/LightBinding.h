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

#include "Core/Application/Application.h"
#include "Core/Lighting/PointLight/PointLight.h"
#include "Core/Lighting/SpotLight/SpotLight.h"
#include "Library/Primitives/Cube.h"

namespace MxEngine
{
	template<typename LightType>
	class LightObject : public Cube
	{
		using Container = Scene::LightContainer<LightType>;
		const Container& container;
		size_t id;
	public:
		inline LightObject(const Container& container, size_t id)
			: container(container), id(id)
		{
			this->ObjectTransform.Scale(0.25f);
		}

		inline virtual void OnUpdate() override
		{
			if (this->id < this->container.GetCount())
			{
				this->Show();
				const auto& light = this->container[this->id];
				this->ObjectTransform.SetTranslation(light.Position);

				auto& material = this->GetMesh()->GetRenderObjects().front().GetMaterial();
				material.Ke = light.AmbientColor + light.DiffuseColor;
			}
			else
			{
				this->Hide();
			}
		}
	};

	template<typename LightType>
	class LightBinding
	{
		static constexpr size_t bindAll = std::numeric_limits<size_t>::max();
		template<typename T>
		using Container = Scene::LightContainer<T>;

		std::string name;
		const Container<LightType>& container;
		size_t id;
	public:
		inline LightBinding(const Scene::LightContainer<LightType>& container)
			: container(container), id(bindAll)
		{
			if constexpr (std::is_same<LightType, PointLight>::value)
			{
				this->name = "PointLight #";
			}
			else if constexpr (std::is_same<LightType, SpotLight>::value)
			{
				this->name = "SpotLight #";
			}
			else
			{
				static_assert(false, "LightBinding accepts either PointLight either SpotLight");
			}
		}

		inline LightBinding(const Scene::LightContainer<LightType>& container, size_t id)
			: LightBinding(container)
		{
			this->id = id;
		}

		inline void Bind()
		{
			auto& scene = Application::Get()->GetCurrentScene();
			std::string objectName = this->name + std::to_string(this->id);
			if (scene.HasObject(objectName)) scene.DestroyObject(objectName);
			scene.AddObject(objectName, MakeUnique<LightObject<LightType>>(this->container, this->id));
		}

		inline void Unbind()
		{
			std::string objectName = this->name + std::to_string(this->id);
			Application::Get()->GetCurrentScene().DestroyObject(objectName);
		}

		inline void BindAll()
		{
			size_t last = this->id;
			for (size_t i = 0; i < this->container.GetCount(); i++)
			{
				this->id = i;
				this->Bind();
			}
			this->id = last;
		}

		inline void UnbindAll()
		{
			size_t last = this->id;
			for (size_t i = 0; i < this->container.GetCount(); i++)
			{
				this->id = i;
				this->Unbind();
			}
			this->id = last;
		}
	};
}