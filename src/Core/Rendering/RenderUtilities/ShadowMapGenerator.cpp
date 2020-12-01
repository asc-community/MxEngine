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

#include "ShadowMapGenerator.h"
#include "Core/Application/Rendering.h"
#include "Core/Rendering/RenderPipeline.h"

namespace MxEngine
{
    ShadowMapGenerator::ShadowMapGenerator(ArrayView<RenderUnit> shadowCasters, ArrayView<Material> materials)
        : shadowCasters(shadowCasters), materials(materials)
    {
        Rendering::GetController().ToggleReversedDepth(false);
        Rendering::GetController().ToggleDepthOnlyMode(true);
    }

    ShadowMapGenerator::~ShadowMapGenerator()
    {
        Rendering::GetController().ToggleDepthOnlyMode(false);
    }

    void CastShadowsUnit(const Shader& shader, const RenderUnit& unit, ArrayView<Material> materials)
    {
        const auto& material = materials[unit.materialIndex];
        material.HeightMap->Bind(0);
        shader.SetUniformFloat("displacement", material.Displacement);
        shader.SetUniformVec2("uvMultipliers", material.UVMultipliers);
        shader.SetUniformInt("map_height", material.HeightMap->GetBoundId());

        Rendering::GetController().GetRenderEngine().SetDefaultVertexAttribute(5, unit.ModelMatrix); //-V807
        Rendering::GetController().GetRenderEngine().SetDefaultVertexAttribute(9, unit.NormalMatrix);
        Rendering::GetController().GetRenderEngine().DrawTrianglesInstanced(*unit.VAO, *unit.IBO, shader, unit.InstanceCount);
    }

    void CastShadowsNoCulling(const Shader& shader, ArrayView<RenderUnit> shadowCasters, ArrayView<Material> materials)
    {
        for (const auto& unit : shadowCasters)
        {
            CastShadowsUnit(shader, unit, materials);
        }
    }

    bool IsInBounds(const Matrix4x4& projection, const Vector3& minAABB, const Vector3& maxAABB)
    {
        auto pmin = projection * Vector4(minAABB, 1.0f);
        auto pmax = projection * Vector4(maxAABB, 1.0f);

        std::array conditions = {
            pmin.x < -1.0f && pmax.x < -1.0f,
            pmin.x >  1.0f && pmax.x >  1.0f,
            pmin.y < -1.0f && pmax.y < -1.0f,
            pmin.y >  1.0f && pmax.y >  1.0f,
            pmin.z < -1.0f && pmax.z < -1.0f,
            pmin.z >  1.0f && pmax.z >  1.0f,
        };

        bool outOfBounds = false;
        for (bool& c : conditions)
            outOfBounds = outOfBounds || c;

        return !outOfBounds;
    };


    void CastShadowsWithCulling(const Matrix4x4& orthoProjection, const Shader& shader, ArrayView<RenderUnit> shadowCasters, ArrayView<Material> materials)
    {
        for (const auto& unit : shadowCasters)
        {
            // do not cull instanced objects, as their position may differ
            if (unit.InstanceCount != 0 || IsInBounds(orthoProjection, unit.MinAABB, unit.MaxAABB))
                CastShadowsUnit(shader, unit, materials);
        }
    }

    void ShadowMapGenerator::GenerateFor(const Shader& shader, ArrayView<DirectionalLightUnit> directionalLights)
    {
        auto& controller = Rendering::GetController();

        for (auto& directionalLight : directionalLights)
        {
            for (size_t i = 0; i < directionalLight.ShadowMaps.size(); i++)
            {
                const auto& projection = directionalLight.ProjectionMatrices[i];

                controller.AttachDepthMap(directionalLight.ShadowMaps[i]);
                shader.SetUniformMat4("LightProjMatrix", projection);

                CastShadowsWithCulling(projection, shader, this->shadowCasters, this->materials);
            }
        }

        for (auto& directionalLight : directionalLights)
        {
            for (size_t i = 0; i < directionalLight.ShadowMaps.size(); i++)
            {
                directionalLight.ShadowMaps[i]->GenerateMipmaps();
            }
        }
    }

    void ShadowMapGenerator::GenerateFor(const Shader& shader, ArrayView<SpotLightUnit> spotLights)
    {
        auto& controller = Rendering::GetController();

        for (auto& spotLight : spotLights)
        {
            controller.AttachDepthMap(spotLight.ShadowMap);
            shader.SetUniformMat4("LightProjMatrix", spotLight.ProjectionMatrix);

            CastShadowsNoCulling(shader, this->shadowCasters, this->materials);
            spotLight.ShadowMap->GenerateMipmaps();
        }
    }

    void ShadowMapGenerator::GenerateFor(const Shader& shader, ArrayView<PointLightUnit> pointLights)
    {
        auto& controller = Rendering::GetController();

        for (auto& pointLight : pointLights)
        {
            controller.AttachDepthMap(pointLight.ShadowMap);
            shader.SetUniformMat4("LightProjMatrix[0]", pointLight.ProjectionMatrices[0]);
            shader.SetUniformMat4("LightProjMatrix[1]", pointLight.ProjectionMatrices[1]);
            shader.SetUniformMat4("LightProjMatrix[2]", pointLight.ProjectionMatrices[2]);
            shader.SetUniformMat4("LightProjMatrix[3]", pointLight.ProjectionMatrices[3]);
            shader.SetUniformMat4("LightProjMatrix[4]", pointLight.ProjectionMatrices[4]);
            shader.SetUniformMat4("LightProjMatrix[5]", pointLight.ProjectionMatrices[5]);
            shader.SetUniformFloat("zFar", pointLight.Radius);
            shader.SetUniformVec3("lightPos", pointLight.Position);

            CastShadowsNoCulling(shader, this->shadowCasters, this->materials);
            pointLight.ShadowMap->GenerateMipmaps();
        }
    }
}