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

#include "Serialization.h"
#include "Utilities/STL/MxMap.h"
#include "Utilities/Logging/Logger.h"
#include "Core/Runtime/ResourceReflection.h"

namespace MxEngine
{
    void VisitSerialize(JsonFile& json, const rttr::variant& v, const ReflectionMeta& meta);

    template<typename T>
    void SerializeGeneric(JsonFile& json, const rttr::variant& v, const ReflectionMeta& meta)
    {
        json = v.get_value<T>();
    }

    void SerializeSequentialContainer(JsonFile& json, const rttr::variant& v, const ReflectionMeta& meta)
    {
        auto view = v.create_sequential_view();

        for (const auto& object : view)
        {
            auto elementValue = object.extract_wrapped_value();
            VisitSerialize(json.emplace_back(), elementValue, ReflectionMeta(elementValue.get_type()));
        }
    }

    void SerializeEnumeration(JsonFile& json, const rttr::variant& v, const ReflectionMeta& meta)
    {
        auto t = v.get_type().get_enumeration();
        const char* name = t.value_to_name(v).cbegin();
        json = name;
    }

    void VisitSerialize(JsonFile& json, const rttr::variant& v, const ReflectionMeta& meta)
    {
        #define VISITOR_SERIALIZE_ENTRY(TYPE) { rttr::type::get<TYPE>(), SerializeGeneric<TYPE> }
        using SerializeCallback = void(*)(JsonFile&, const rttr::variant&, const ReflectionMeta&);
        static MxMap<rttr::type, SerializeCallback> visitor = {
            VISITOR_SERIALIZE_ENTRY(bool),
            VISITOR_SERIALIZE_ENTRY(MxString),
            VISITOR_SERIALIZE_ENTRY(float),
            VISITOR_SERIALIZE_ENTRY(int),
            VISITOR_SERIALIZE_ENTRY(unsigned int),
            VISITOR_SERIALIZE_ENTRY(size_t),
            VISITOR_SERIALIZE_ENTRY(Quaternion),
            VISITOR_SERIALIZE_ENTRY(Vector2),
            VISITOR_SERIALIZE_ENTRY(Vector3),
            VISITOR_SERIALIZE_ENTRY(Vector4),
        };

        auto t = v.get_type();
        if (visitor.find(t) != visitor.end())
        {
            visitor[t](json, v, meta);
        }
        else if (v.is_sequential_container())
        {
            SerializeSequentialContainer(json, v, meta);
        }
        else if (t.is_enumeration())
        {
            SerializeEnumeration(json, v, meta);
        }
        else
        {
            Serialize(json, v);
        }
    }

    void Serialize(JsonFile& json, rttr::instance object)
    {
        if (IsHandle(object))
        {
            json = GetHandleId(object);
            return;
        }

        auto type = object.get_type();

        for (auto property : type.get_properties())
        {
            ReflectionMeta propertyMeta(property);
            if ((propertyMeta.Flags & MetaInfo::SERIALIZABLE) == 0) continue;
            if ((propertyMeta.Condition != nullptr && !propertyMeta.Condition(object))) continue;

            const char* propertyName = property.get_name().cbegin();

            if (propertyMeta.Serialization.CustomSerialize != nullptr)
            {
                propertyMeta.Serialization.CustomSerialize(json[propertyName], object);
            }
            else
            {
                VisitSerialize(json[propertyName], property.get_value(object), propertyMeta);
            }
        }
        if (type.get_properties().empty())
        {
            MXLOG_WARNING("MxEngine::Serializer", "no properties for type: " + MxString(object.get_type().get_name().cbegin()));
        }
    }
}