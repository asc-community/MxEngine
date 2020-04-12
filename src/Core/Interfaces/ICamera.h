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

#include "Utilities/Math/Math.h"

namespace MxEngine
{
	struct ICamera
	{
		virtual const Matrix4x4& GetMatrix() const = 0;
		virtual const Matrix4x4& GetViewMatrix() const = 0;
		virtual const Matrix4x4& GetProjectionMatrix() const = 0;
		virtual void SetViewMatrix(const Matrix4x4& view) = 0;
		virtual void SetAspectRatio(float w, float h = 1.0f) = 0;
		virtual float GetAspectRatio() const = 0;
		virtual void SetZNear(float zNear) = 0;
		virtual void SetZFar(float zFar) = 0;
		virtual float GetZNear() const = 0;
		virtual float GetZFar() const = 0;
		virtual void SetZoom(float zoom) = 0;
		virtual float GetZoom() const = 0;

		virtual ~ICamera() = default;
	};
}