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

#include "Core/Serialization/Serializer.h"
#include "Core/Components/Physics/CompoundCollider.h"
#include "Core/Components/Instancing/InstanceFactory.h"
#include "Core/Serialization/SceneSerializer.h"

namespace MxEngine
{
    template<>
    void SerializeExtra<CompoundCollider::CompoundColliderChild>(rttr::instance jsonWrapped, rttr::instance& object)
    {
        auto& json = *jsonWrapped.try_convert<JsonFile>();
        auto& child = *object.try_convert<CompoundCollider::CompoundColliderChild>();

        std::visit([&json](auto&& shape) mutable
        {
            auto bounding = shape->GetNativeBounding();
            const char* name = rttr::type::get(bounding).get_name().cbegin();
            Serialize(json[name], bounding);
        }, child.Shape);
    }

    template<>
    void SerializeExtra<InstanceFactory>(rttr::instance jsonWrapped, rttr::instance& object)
    {
        auto& json = *jsonWrapped.try_convert<JsonFile>();
        auto& instanceFactory = *object.try_convert<InstanceFactory>();

        for (auto& instance : instanceFactory.GetInstances())
        {
            json.push_back(SceneSerializer::SerializeMxObject(*instance));
        }
    }
}