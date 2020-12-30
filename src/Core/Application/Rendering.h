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

#include "Core/Rendering/RenderController.h"

namespace MxEngine
{
    struct RenderAdaptor;
    
    template<typename, typename> class Resource;
    class CameraController;
    class ComponentFactory;
    using CameraControllerHandle = Resource<CameraController, ComponentFactory>;

	class Rendering
	{
    public:
		static CameraControllerHandle& GetViewport();
		static void SetViewport(const CameraControllerHandle& viewport);
        static void ResizeViewport(size_t width, size_t height);
        static VectorInt2 GetViewportSize();
        static TextureHandle GetRenderTexture();
		static RenderController& GetController();
        static RenderAdaptor& GetAdaptor();
        static bool IsDebugOverlayed();
        static void SetDebugOverlay(bool value = true);
        static void SetRenderToDefaultFrameBuffer(bool value = true);
        static bool IsRenderedToDefaultFrameBuffer();
        static void Draw(const Line& line, const Vector4& color);
        static void Draw(const AABB& box, const Vector4& color);
        static void Draw(const BoundingBox& box, const Vector4& color);
        static void Draw(const BoundingSphere& sphere, const Vector4& color);
        static void Draw(const Frustrum& frustrum, const Vector4& color);
        static void Draw(const Cone& cone, const Vector4& color);
        static void Draw(const Capsule& capsule, const Vector4& color);
        static void Draw(const Cylinder& cylinder, const Vector4& color);
        static void Draw(const Rectangle& rectangle, const Vector4& color);
        static void Draw(const Circle& circle, const Vector4& color);
	};
}