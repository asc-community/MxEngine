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

namespace MxEngine
{
    #define MXENGINE_REFLECT_TYPE RTTR_REGISTRATION

    struct MetaInfo
    {
        constexpr static const char* FLAGS = "flags";

        enum Flags : uint32_t
        {
            SERIALIZABLE = 1 << 0,
            EDITABLE = 1 << 1,
        };
    };

    struct EditorInfo
    {
        constexpr static const char* EDIT_PRECISION = "edit precision";
        constexpr static const char* SUBTREE_NAME = "subtree name";
        constexpr static const char* EDIT_RANGE = "edit name";
        constexpr static const char* INTERPRET_AS = "interpret as";
        constexpr static const char* EXTERNAL_VIEW = "extra view";
        constexpr static const char* VIEW_CONDITION = "view condition";
    };

    using ViewConditionFunction = bool(*)(const rttr::instance&);
    using ExtraViewFunction = void(*)(rttr::instance&);

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

            rttr::variant precision = obj.get_metadata(EditorInfo::EDIT_PRECISION);
            this->Editor.EditPrecision = precision.is_valid() ? precision.get_value<float>() : 1.0f;

            rttr::variant subtreeName = obj.get_metadata(EditorInfo::SUBTREE_NAME);
            this->Editor.SubtreeName = subtreeName.is_valid() ? subtreeName.get_value<std::string>() : std::string{ };

            rttr::variant editRange = obj.get_metadata(EditorInfo::EDIT_RANGE);
            this->Editor.EditRange = editRange.is_valid() ? editRange.get_value<Range>() : Range{ 0.0f, 0.0f };

            rttr::variant interpretAs = obj.get_metadata(EditorInfo::INTERPRET_AS);
            this->Editor.InterpretAs = interpretAs.is_valid() ? interpretAs.get_value<InterpretAsInfo>() : InterpretAsInfo::DEFAULT;

            rttr::variant externalView = obj.get_metadata(EditorInfo::EXTERNAL_VIEW);
            this->Editor.ExtraView = externalView.is_valid() ? externalView.get_value<ExtraViewFunction>() : nullptr;

            rttr::variant viewCondition = obj.get_metadata(EditorInfo::VIEW_CONDITION);
            this->Editor.ViewCondition = viewCondition.is_valid() ? viewCondition.get_value<ViewConditionFunction>() : nullptr;
        }
        
        uint32_t Flags;

        struct
        {
            std::string SubtreeName;
            ExtraViewFunction ExtraView;
            ViewConditionFunction ViewCondition;
            Range EditRange;
            float EditPrecision;
            InterpretAsInfo InterpretAs;
        } Editor;
    };
}