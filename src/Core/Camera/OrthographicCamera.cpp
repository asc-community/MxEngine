// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in sourceand binary forms, with or without
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

#include "OrthographicCamera.h"
#include <algorithm>

namespace MxEngine
{
	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		this->updateProjection = true;
		this->updateMatrix = true;
		this->projectLimits = { left, right, bottom, top };
	}

	const Matrix4x4& OrthographicCamera::GetMatrix() const
	{
		if (this->updateMatrix)
		{
			if (this->updateProjection)
			{
				float left = this->projectLimits.x;
				float right = this->projectLimits.y;
				float bottom = this->projectLimits.z;
				float top = this->projectLimits.w;
				this->projection = MakeOrthographicMatrix(
					left * this->aspectRatio,
					right * this->aspectRatio,
					bottom,
					top,
					this->zNear,
					this->zFar
				);
				this->updateProjection = false;
			}
			this->matrix = this->projection * this->view;
			this->updateMatrix = false;
		}
		return this->matrix;
	}

	void OrthographicCamera::SetViewMatrix(const Matrix4x4& view)
	{
		this->view = view;
		this->updateMatrix = true;
	}

	void OrthographicCamera::SetAspectRatio(float w, float h)
	{
		updateMatrix = true;
		updateProjection = true;
		this->aspectRatio = w / h;
	}

	float OrthographicCamera::GetAspectRatio() const
	{
		return this->aspectRatio;
	}

	void OrthographicCamera::SetZNear(float zNear)
	{
		this->zNear = zNear;
		this->updateProjection = true;
		this->updateMatrix = true;
	}

	void OrthographicCamera::SetZFar(float zFar)
	{
		this->zFar = zFar;
		this->updateProjection = true;
		this->updateMatrix = true;
	}

	float OrthographicCamera::GetZNear() const
	{
		return this->zNear;
	}

	float OrthographicCamera::GetZFar() const
	{
		return this->zFar;
	}
	void OrthographicCamera::SetZoom(float zoom)
	{
		this->SetProjection(-zoom, zoom, -zoom, zoom);
	}

	float OrthographicCamera::GetZoom() const
	{
		return this->projectLimits.y; // y, w possible as they are 1 * zoom
	}
}