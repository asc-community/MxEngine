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

#include "Core/Serialization/Serialization.h"
#include "Core/Components/Physics/CompoundCollider.h"
#include "Core/Components/Instancing/InstanceFactory.h"
#include "Core/Components/Camera/VRCameraController.h"
#include "Platform/OpenGL/Texture.h"
#include "Core/Runtime/HandleMappings.h"
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
            json["type"] = name;
            Serialize(json["bounding"], bounding);
        }, child.Shape);
    }

    template<size_t Index = 0, typename... Args>
    void FindDeserializerForChildShapeImpl(const JsonFile& json, std::variant<Args...>& shape, HandleMappings& mappings)
    {
        if constexpr (Index < std::variant_size_v<std::variant<Args...>>)
        {
            using ShapeType = std::decay_t<decltype(*std::get<Index>(shape))>;
            using BoundingType = decltype(std::declval<ShapeType>().GetNativeBounding());
            const char* name = rttr::type::get<BoundingType>().get_name().cbegin();

            if (json["type"].get<MxString>() == name)
            {
                BoundingType bounding;
                Deserialize(json["bounding"], rttr::instance{ bounding }, mappings);
                shape = PhysicsFactory::Create<ShapeType>(bounding);
            }
            else
            {
                FindDeserializerForChildShapeImpl<Index + 1>(json, shape, mappings);
            }
        }
        else
        {
            MXLOG_WARNING("MxEngine::Serializer", "cannot find deserializer for type: " + json["type"].get<MxString>());
        }
    }

    void FindDeserializerForChildShape(const JsonFile& json, CompoundCollider::VariantType& shape, HandleMappings& mappings)
    {
        FindDeserializerForChildShapeImpl(json, shape, mappings);
    }

    template<>
    void DeserializeExtra<CompoundCollider::CompoundColliderChild>(rttr::instance jsonWrapped, rttr::instance& object, HandleMappings& mappings)
    {
        const auto& json = *jsonWrapped.try_convert<JsonFile>();
        auto& child = *object.try_convert<CompoundCollider::CompoundColliderChild>();

        FindDeserializerForChildShape(json, child.Shape, mappings);
    }

    template<>
    void SerializeExtra<InstanceFactory>(rttr::instance jsonWrapped, rttr::instance& object)
    {
        auto& json = *jsonWrapped.try_convert<JsonFile>();
        auto& instanceFactory = *object.try_convert<InstanceFactory>();

        for (auto& instance : instanceFactory.GetInstances())
        {
            if (instance->IsSerialized)
            {
                json.push_back(SceneSerializer::SerializeMxObject(*instance));
            }
        }
    }

    template<>
    void DeserializeExtra<InstanceFactory>(rttr::instance jsonWrapped, rttr::instance& object, HandleMappings& mappings)
    {
        const auto& json = *jsonWrapped.try_convert<JsonFile>();
        auto& instanceFactory = *object.try_convert<InstanceFactory>();

        for (const auto& entry : json)
        {
            auto instance = instanceFactory.Instanciate();
            SceneSerializer::DeserializeMxObject(entry, instance, mappings);
            DeserializeComponent(entry["Instance"], instance->GetComponent<Instance>(), mappings);
        }
    }

    template<>
    void SerializeExtra<VRCameraController>(rttr::instance jsonWrapped, rttr::instance& object)
    {
        auto& json = *jsonWrapped.try_convert<JsonFile>();
        auto& vr = *object.try_convert<VRCameraController>();

        if (vr.LeftEye.IsValid()) json["left"] = vr.LeftEye.GetHandle();
        if (vr.RightEye.IsValid()) json["right"] = vr.RightEye.GetHandle();
    }

    template<>
    void DeserializeExtra<VRCameraController>(rttr::instance jsonWrapped, rttr::instance& object, HandleMappings& mappings)
    {
        const auto& json = *jsonWrapped.try_convert<JsonFile>();
        auto& vr = *object.try_convert<VRCameraController>();

        if (json.contains("left")) vr.LeftEye = GetHandleById<CameraController::Handle>(json["left"], mappings);
        if (json.contains("right")) vr.RightEye = GetHandleById<CameraController::Handle>(json["right"], mappings);
    }

    template<>
    void SerializeExtra<Texture>(rttr::instance jsonWrapped, rttr::instance& object)
    {
        auto& json = *jsonWrapped.try_convert<JsonFile>();
        auto& texture = *object.try_convert<Texture>();

        auto formatEnumType = rttr::type::get<TextureFormat>().get_enumeration();

        json["path"] = texture.GetFilePath();
        json["format"] = formatEnumType.value_to_name(texture.GetFormat()).cbegin();
    }

    template<>
    void DeserializeExtra<Texture>(rttr::instance jsonWrapped, rttr::instance& object, HandleMappings& mappings)
    {
        const auto& json = *jsonWrapped.try_convert<JsonFile>();
        auto& texture = *object.try_convert<Texture>();

        auto formatEnumType = rttr::type::get<TextureFormat>().get_enumeration();

        auto filepath = ToFilePath(json["path"].get<MxString>());
        auto format = formatEnumType.name_to_value(json["format"].get<MxString>().c_str()).convert<TextureFormat>();

        texture.Load(filepath, format);
    }
}