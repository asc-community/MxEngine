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

#include "CameraBase.h"

namespace MxEngine
{
    const Matrix4x4& CameraBase::GetMatrix() const
    {
        MX_ASSERT(this->UpdateProjection == false);
        if (this->updateMatrix) this->matrix = this->GetProjectionMatrix() * this->GetViewMatrix();
        return this->matrix;
    }

    const Matrix4x4& CameraBase::GetProjectionMatrix() const
    {
        return this->projection;
    }

    const Matrix4x4& CameraBase::GetViewMatrix() const
    {
        return this->view;
    }

    void CameraBase::SetProjectionMatrix(const Matrix4x4& projection)
    {
        this->projection = projection;
        this->UpdateProjection = false;
        this->updateMatrix = true;
    }

    void CameraBase::SetViewMatrix(const Matrix4x4& view)
    {
        this->view = view;
        this->updateMatrix = true;
    }

    float CameraBase::GetZoom() const
    {
        return this->zoom;
    }

    void CameraBase::SetZoom(float zoom)
    {
        this->zoom = zoom;
        this->UpdateProjection = true;
    }

    void CameraBase::SetAspectRatio(float w, float h)
    {
        this->aspectRatio = w / h;
        this->UpdateProjection = true;
    }

    float CameraBase::GetAspectRatio() const
    {
        return this->aspectRatio;
    }

    float CameraBase::GetZFar() const
    {
        return this->zFar;
    }

    void CameraBase::SetZFar(float zFar)
    {
        this->zFar = Max(zFar, this->zNear);
        this->UpdateProjection = true;
    }

    float CameraBase::GetZNear() const
    {
        return this->zNear;
    }

    void CameraBase::SetZNear(float zNear)
    {
        this->zNear = Min(zNear, this->zFar);
        this->UpdateProjection = true;
    }
}