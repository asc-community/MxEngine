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

#include "RenderController.h"

namespace MxEngine
{
    RenderController::RenderController(Renderer& renderer)
        : renderer(renderer)
    {
    }

    Renderer& RenderController::GetRenderEngine() const
    {
        return this->renderer;
    }

    void RenderController::Render() const
    {
        this->renderer.Finish();
    }

    void RenderController::Clear() const
    {
        this->renderer.Clear();
    }

    void RenderController::DrawObject(const IDrawable& object) const
    {
        if (!this->ViewPort.HasCamera()) return;
        if (!object.IsDrawable()) return;
        size_t iterator = object.GetIterator();
        auto MVP = this->ViewPort.GetCameraMatrix() * object.GetModel();
        Matrix3x3 NormalMatrix = Transpose(Inverse(object.GetModel()));
        auto cameraPos = this->ViewPort.GetPosition();

        while (!object.IsLast(iterator))
        {
            const auto& renderObject = object.GetCurrent(iterator);
            if (renderObject.HasMaterial())
            {
                const Material& material = renderObject.GetMaterial();

                #define BIND_TEX(name, slot) \
				if (material.name != nullptr)\
					material.name->Bind(slot);\
				else if (object.HasTexture())\
					object.GetTexture().Bind(slot);\
				else\
					this->DefaultTexture->Bind(slot)

                BIND_TEX(map_Ka, 0);
                BIND_TEX(map_Kd, 1);
                BIND_TEX(map_Ks, 2);
                BIND_TEX(map_Ke, 3);
                //BIND_TEX(map_Kd, 4); kd not used now

                const Shader& shader = object.HasShader() ? object.GetShader() : *this->ObjectShader;
                shader.SetUniformMat4("MVP", MVP);
                shader.SetUniformMat4("Model", object.GetModel());
                shader.SetUniformMat3("NormalMatrix", NormalMatrix);
                shader.SetUniformVec3("material.Ka", material.Ka.r, material.Ka.g, material.Ka.b);
                shader.SetUniformVec3("material.Kd", material.Kd.r, material.Kd.g, material.Kd.b);
                shader.SetUniformVec3("material.Ks", material.Ks.r, material.Ks.g, material.Ks.b);
                shader.SetUniformVec3("material.Ke", material.Ke.r, material.Ke.g, material.Ke.b);
                shader.SetUniformFloat("material.Ns", material.Ns);
                shader.SetUniformFloat("material.d", material.d);
                shader.SetUniformVec3("globalLight", cameraPos.x, cameraPos.y, cameraPos.z);
                shader.SetUniformVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

                if (object.GetInstanceCount() == 0)
                    this->GetRenderEngine().DrawTriangles(renderObject.GetVAO(), renderObject.GetVertexCount(), shader);
                else
                    this->GetRenderEngine().DrawTrianglesInstanced(renderObject.GetVAO(), renderObject.GetVertexCount(), shader, object.GetInstanceCount());
            }
            iterator = object.GetNext(iterator);
        }
    }

    void RenderController::DrawObjectMesh(const IDrawable& object) const
    {
        if (!object.IsDrawable()) return;
        size_t iterator = object.GetIterator();
        auto MVP = this->ViewPort.GetCameraMatrix() * object.GetModel();
        while (!object.IsLast(iterator))
        {
            const auto& renderObject = object.GetCurrent(iterator);
            this->MeshShader->SetUniformMat4("MVP", MVP);
            if (object.GetInstanceCount() == 0)
                this->GetRenderEngine().DrawLines(renderObject.GetVAO(), renderObject.GetMeshIBO(), *this->MeshShader);
            else
                this->GetRenderEngine().DrawLinesInstanced(renderObject.GetVAO(), renderObject.GetMeshIBO(), *this->MeshShader, object.GetInstanceCount());
            iterator = object.GetNext(iterator);
        }
    }
}