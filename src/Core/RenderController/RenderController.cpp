#include "RenderController.h"

namespace MomoEngine
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
        this->renderer.Flush();
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
                this->GetRenderEngine().DrawLines(renderObject.GetVAO(), renderObject.GetIBO(), *this->MeshShader);
            else
                this->GetRenderEngine().DrawLinesInstanced(renderObject.GetVAO(), renderObject.GetIBO(), *this->MeshShader, object.GetInstanceCount());
            iterator = object.GetNext(iterator);
        }
    }
}