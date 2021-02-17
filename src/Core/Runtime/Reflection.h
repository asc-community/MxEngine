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

#include <rttr/registration.h>
#include "Utilities/STL/MxVector.h"
#include "Utilities/VectorPool/VectorPool.h"

namespace MxEngine
{
    #define MXENGINE_REFLECT_TYPE RTTR_REGISTRATION

    struct HandleMappings;

    struct MetaInfo
    {
        constexpr static const char* FLAGS = "flags";
        constexpr static const char* COPY_FUNCTION = "copy";
        constexpr static const char* CONDITION = "condition";

        enum Flags : uint32_t
        {
            SERIALIZABLE = 1 << 0,
            EDITABLE = 1 << 1,
            HANDLE = 1 << 2,
        };
    };

    struct EditorInfo
    {
        constexpr static const char* EDIT_PRECISION = "edit precision";
        constexpr static const char* EDIT_RANGE = "edit name";
        constexpr static const char* INTERPRET_AS = "interpret as";
        constexpr static const char* CUSTOM_VIEW = "custom view";
        constexpr static const char* HANDLE_EDITOR = "handle editor";
    };

    struct SerializeInfo
    {
        constexpr static const char* CUSTOM_SERIALIZE = "serialize";
        constexpr static const char* CUSTOM_DESERIALIZE = "deserialize";
    };

    using ConditionFunction = bool(*)(const rttr::instance&);
    using CustomViewFunction = rttr::variant(*)(rttr::instance&);
    using HandleEditorFunction = rttr::variant(*)(rttr::instance&);
    using InstanceToVariantFunction = rttr::variant(*)(rttr::instance&);
    using CustomSerializeFunction = void(*)(rttr::instance, rttr::instance&);
    using CustomDeserializeFunction = void(*)(rttr::instance, rttr::instance&, HandleMappings&);

    enum class InterpretAsInfo 
    {
        DEFAULT,
        COLOR,
    };

    struct Range
    {
        float Min = 0.0f;
        float Max = 0.0f;
    };

    class ReflectionMeta
    {
    public:
        template<typename T>
        ReflectionMeta(const T& obj)
        {
            rttr::variant flags = obj.get_metadata(MetaInfo::FLAGS);
            this->Flags = flags.is_valid() ? flags.to_uint32() : uint32_t{ 0 };

            rttr::variant copyFunction = obj.get_metadata(MetaInfo::COPY_FUNCTION);
            this->CopyFunction = copyFunction.is_valid() ? copyFunction.get_value<InstanceToVariantFunction>() : nullptr;

            rttr::variant condition = obj.get_metadata(MetaInfo::CONDITION);
            this->Condition = condition.is_valid() ? condition.get_value<ConditionFunction>() : nullptr;

            rttr::variant precision = obj.get_metadata(EditorInfo::EDIT_PRECISION);
            this->Editor.EditPrecision = precision.is_valid() ? precision.get_value<float>() : 1.0f;

            rttr::variant editRange = obj.get_metadata(EditorInfo::EDIT_RANGE);
            this->Editor.EditRange = editRange.is_valid() ? editRange.get_value<Range>() : Range{ 0.0f, 0.0f };

            rttr::variant interpretAs = obj.get_metadata(EditorInfo::INTERPRET_AS);
            this->Editor.InterpretAs = interpretAs.is_valid() ? interpretAs.get_value<InterpretAsInfo>() : InterpretAsInfo::DEFAULT;

            rttr::variant customView = obj.get_metadata(EditorInfo::CUSTOM_VIEW);
            this->Editor.CustomView = customView.is_valid() ? customView.get_value<CustomViewFunction>() : nullptr;

            rttr::variant handleEditor = obj.get_metadata(EditorInfo::HANDLE_EDITOR);
            this->Editor.HandleEditor = handleEditor.is_valid() ? handleEditor.get_value<HandleEditorFunction>() : nullptr;

            rttr::variant customSerialize = obj.get_metadata(SerializeInfo::CUSTOM_SERIALIZE);
            this->Serialization.CustomSerialize = customSerialize.is_valid() ? customSerialize.get_value<CustomSerializeFunction>() : nullptr;

            rttr::variant customDeserialize = obj.get_metadata(SerializeInfo::CUSTOM_DESERIALIZE);
            this->Serialization.CustomDeserialize = customDeserialize.is_valid() ? customDeserialize.get_value<CustomDeserializeFunction>() : nullptr;
        }

        uint32_t Flags = 0;
        InstanceToVariantFunction CopyFunction = nullptr;
        ConditionFunction Condition = nullptr;

        struct
        {
            CustomViewFunction CustomView = nullptr;
            HandleEditorFunction HandleEditor = nullptr;
            Range EditRange{ 0.0f, 0.0f };
            float EditPrecision = 1.0f;
            InterpretAsInfo InterpretAs = InterpretAsInfo::DEFAULT;
        } Editor;

        struct 
        {
            CustomSerializeFunction CustomSerialize = nullptr;
            CustomDeserializeFunction CustomDeserialize = nullptr;
        } Serialization;
    };

    template<typename T>
    rttr::variant Copy(rttr::instance& i)
    {
        return rttr::variant{ *i.try_convert<T>() };
    }

    template<typename>
    void SerializeExtra(rttr::instance, rttr::instance&);

    template<typename>
    void DeserializeExtra(rttr::instance, rttr::instance&, HandleMappings&);

    namespace GUI
    {
        template<typename>
        rttr::variant EditorExtra(rttr::instance&);

        template<typename>
        rttr::variant HandleEditorExtra(rttr::instance&);
    }
}

template<typename T, typename Allocator>
struct rttr::sequential_container_mapper<MxEngine::MxVector<T, Allocator>>
    : public std::conditional_t<std::is_default_constructible_v<T>,
        rttr::detail::sequential_container_base_dynamic_direct_access<MxEngine::MxVector<T, Allocator>>,
        rttr::detail::sequential_container_base_static<MxEngine::MxVector<T, Allocator>>
    >
{

};