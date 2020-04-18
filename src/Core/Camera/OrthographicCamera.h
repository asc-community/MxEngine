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

#include "Core/Interfaces/ICamera.h"

namespace MxEngine
{
	class OrthographicCamera : public ICamera
	{
		mutable Matrix4x4 matrix;
		mutable Matrix4x4 view;
		mutable Matrix4x4 projection;
		Vector4 projectLimits = { -1.0f, 1.0f, -1.0f, 1.0f };
		float aspectRatio = 1.777f;
		float zNear = 0.1f, zFar = 100.0f;
		mutable bool updateMatrix = true;
		mutable bool updateProjection = true;
	public:
		void SetProjection(float left, float right, float bottom, float top);
		
		// Inherited via ICamera
		virtual const Matrix4x4& GetMatrix() const override;
		virtual const Matrix4x4& GetViewMatrix() const override;
		virtual const Matrix4x4& GetProjectionMatrix() const override;
		virtual void SetViewMatrix(const Matrix4x4& view) override;
		virtual void SetAspectRatio(float w, float h = 1.0f) override;
		virtual float GetAspectRatio() const override;
		virtual void SetZNear(float zNear) override;
		virtual void SetZFar(float zFar) override;
		virtual float GetZNear() const override;
		virtual float GetZFar() const override;
		virtual void SetZoom(float zoom) override;
		virtual float GetZoom() const override;
	};
}