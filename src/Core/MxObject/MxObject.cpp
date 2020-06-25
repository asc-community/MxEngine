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

#include "MxObject.h"
#include "Utilities/ObjectLoader/ObjectLoader.h"
#include "Platform/GraphicAPI.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/Format/Format.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Rendering/MeshRenderer.h"

namespace MxEngine
{
    MxObject::Handle MxObject::Create()
    {
		auto object = Factory::Create<MxObject>();
		object->handle = object.GetHandle();
		object.MakeStatic();
		return object;
    }

	void MxObject::Destroy(MxObject::Handle& object)
	{
		Factory::Destroy(object);
	}

    void MxObject::Destroy(MxObject& object)
    {
		MX_ASSERT(object.handle != InvalidHandle);
		Factory::Get<MxObject>().Deallocate(object.handle);
    }

    ComponentView<MxObject> MxObject::GetObjects()
    {
		return ComponentView<MxObject>{ Factory::Get<MxObject>() };
    }

	MxObject::Handle MxObject::GetByName(const MxString& name)
	{
		auto& factory = Factory::Get<MxObject>();
		for (auto& resource : factory)
		{
			if (resource.value.Name == name)
				return MxObject::Handle{ resource.uuid, factory.IndexOf(resource) };
		}
		return MxObject::Handle{ };
	}

    MxObject::MxObject()
    {
		this->Transform = this->AddComponent<MxEngine::Transform>();
    }

    MxObject::~MxObject()
    {
		this->components.RemoveAllComponents();
    }
}