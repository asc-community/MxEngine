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

#include "DereferenceHandle.h"
#include "Utilities/STL/MxMap.h"
#include "Core/Resources/AssetManager.h"
#include "Core/MxObject/MxObject.h"

namespace MxEngine
{
    template<typename THandle>
    DereferenceHandleInfo DereferenceGeneric(rttr::instance object)
    {
        using T = typename std::decay<decltype(*std::declval<THandle>())>::type;
        THandle& handle = *object.try_convert<THandle>();
        auto handleId = handle.IsValid() ? handle.GetHandle() : size_t(-1);
        return DereferenceHandleInfo{ handle.IsValid() ? rttr::instance{ *handle } : rttr::instance{ }, rttr::type::get<T>(), handleId };
    }

    DereferenceHandleInfo DereferenceHandle(rttr::instance object)
    {
        #define VISITOR_DEREFERENCE_ENTRY(TYPE) { rttr::type::get<TYPE>(), DereferenceGeneric<TYPE> }
        using DereferenceCallback = DereferenceHandleInfo(*)(rttr::instance);
        MxMap<rttr::type, DereferenceCallback> visitor = {
            VISITOR_DEREFERENCE_ENTRY(MaterialHandle),
            VISITOR_DEREFERENCE_ENTRY(MeshHandle),
            VISITOR_DEREFERENCE_ENTRY(MxObject::Handle),
            VISITOR_DEREFERENCE_ENTRY(AudioBufferHandle),
            VISITOR_DEREFERENCE_ENTRY(AudioPlayerHandle),
            VISITOR_DEREFERENCE_ENTRY(TextureHandle),
            VISITOR_DEREFERENCE_ENTRY(ShaderHandle),
            VISITOR_DEREFERENCE_ENTRY(FrameBufferHandle),
            VISITOR_DEREFERENCE_ENTRY(CubeMapHandle),
            VISITOR_DEREFERENCE_ENTRY(IndexBufferHandle),
            VISITOR_DEREFERENCE_ENTRY(RenderBufferHandle),
            VISITOR_DEREFERENCE_ENTRY(VertexArrayHandle),
            VISITOR_DEREFERENCE_ENTRY(VertexBufferHandle),
            VISITOR_DEREFERENCE_ENTRY(VertexBufferLayoutHandle),
        };

        auto t = object.get_type();
        if (visitor.find(t) != visitor.end())
        {
            return visitor[t](object);
        }
        else
        {
            return DereferenceHandleInfo{ object, t, size_t(-1) };
        }
    }
}