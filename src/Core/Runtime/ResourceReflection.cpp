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

#include "ResourceReflection.h"
#include "Core/Resources/AssetManager.h"
#include "Core/MxObject/MxObject.h"
#include "Utilities/Logging/Logger.h"
#include "HandleMappings.h"

namespace MxEngine
{
    #define HANDLE_TYPELIST MaterialHandle, MeshHandle, MxObject::Handle, AudioBufferHandle, TextureHandle, ShaderHandle, CubeMapHandle


    template<typename T>
    rttr::variant GetHandleByIdGeneric(size_t handleId)
    {
        using Type = typename T::Type;
        using Factory = typename T::Factory;
        auto& pool = Factory::template Get<Type>();
        auto handle = pool.IsAllocated(handleId) ? Factory::GetHandle(pool[handleId]) : T{ };
        return rttr::variant{ handle };
    }
    
    template<typename... Args>
    rttr::variant GetHandleByIdImpl(const rttr::type& type, size_t handleId)
    {
        const static MxMap<rttr::type, rttr::variant(*)(size_t)> mapping = {
            { rttr::type::get<Args>(), GetHandleByIdGeneric<Args> }...
        };

        auto visitor = mapping.find(type);
        if (visitor != mapping.end())
        {
            return visitor->second(handleId);
        }
        else
        {
            return rttr::variant{ };
        }
    }

    rttr::variant GetHandleById(const rttr::type& type, size_t handleId)
    {
        return GetHandleByIdImpl<HANDLE_TYPELIST>(type, handleId);
    }



    struct HandleReflection
    {
        rttr::instance DereferencedValue;
        rttr::type DereferencedType;
        size_t HandleId;
    };

    template<typename T>
    HandleReflection ReflectHandleGeneric(rttr::instance object)
    {
        T& handle = *object.try_convert<T>();

        return HandleReflection{
            handle.IsValid() ? rttr::instance{ *handle } : rttr::instance{ },
            rttr::type::get<typename T::Type>(),
            handle.IsValid() ? handle.GetHandle() : size_t(-1),
        };
    }

    template<typename... Args>
    HandleReflection ReflectHandleImpl(rttr::instance handle)
    {
        const static MxMap<rttr::type, HandleReflection(*)(rttr::instance)> mapping = {
            { rttr::type::get<Args>(), ReflectHandleGeneric<Args> }...
        };

        auto type = handle.get_type();
        auto visitor = mapping.find(type);
        if (visitor != mapping.end())
        {
            return visitor->second(handle);
        }
        else
        {
            return HandleReflection{
                rttr::instance{ },
                type,
                size_t(-1),
            };
        }
    }

    rttr::instance DereferenceHandle(rttr::instance handle)
    {
        return ReflectHandleImpl<HANDLE_TYPELIST>(handle).DereferencedValue;
    }

    size_t GetHandleId(rttr::instance handle)
    {
        return ReflectHandleImpl<HANDLE_TYPELIST>(handle).HandleId;
    }

    rttr::type GetTypeByHandle(rttr::instance handle)
    {
        return ReflectHandleImpl<HANDLE_TYPELIST>(handle).DereferencedType;
    }

    bool IsHandle(rttr::instance handle)
    {
        return ReflectHandleImpl<HANDLE_TYPELIST>(handle).DereferencedType != handle.get_type();
    }

    rttr::variant CreateEmptyHandle(const rttr::type& type)
    {
        return GetHandleById(type, size_t(-1));
    }

    rttr::variant GetHandleById(const rttr::type& type, size_t handleId, const HandleMappings& mappings)
    {
        auto projections = mappings.TypeToProjection.find(type);
        if (projections == mappings.TypeToProjection.end())
        {
            MXLOG_ERROR("MxEngine::Reflection", "cannot find handle mapping for type: " + MxString(type.get_name().cbegin()));
            return rttr::variant{ };
        }

        auto projection = projections->second.find(handleId);
        if (projection == projections->second.end())
        {
            return CreateEmptyHandle(type);
        }

        return projection->second;
    }
}