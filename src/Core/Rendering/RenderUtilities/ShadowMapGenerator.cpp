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
#include "Core/BoundingObjects/FrustrumCuller.h"

namespace MxEngine
{
    ShadowMapGenerator::ShadowMapGenerator(const RenderList& shadowCasters, ArrayView<RenderUnit> renderUnits, ArrayView<Material> materials)
        : shadowCasters(shadowCasters), renderUnits(renderUnits), materials(materials)
    {
        Rendering::GetController().ToggleReversedDepth(false);
        Rendering::GetController().ToggleDepthOnlyMode(true);
    }

    ShadowMapGenerator::~ShadowMapGenerator()
    {
        Rendering::GetController().ToggleDepthOnlyMode(false);
    }

    void RenderUnitToDepthMap(const Shader& shader, size_t instanceCount, const RenderUnit& unit, ArrayView<Material> materials)
    {
        const auto& material = materials[unit.materialIndex];
        material.HeightMap->Bind(0);
        material.AlbedoMap->Bind(1);
        shader.SetUniform("displacement", material.Displacement);
        shader.SetUniform("uvMultipliers", material.UVMultipliers);
        shader.SetUniform("map_height", material.HeightMap->GetBoundId());
        shader.SetUniform("map_albedo", material.AlbedoMap->GetBoundId());

        Rendering::GetController().GetRenderEngine().SetDefaultVertexAttribute(5, unit.ModelMatrix); //-V807
        Rendering::GetController().GetRenderEngine().SetDefaultVertexAttribute(9, unit.NormalMatrix);
        Rendering::GetController().DrawIndicies(RenderPrimitive::TRIANGLES, unit.IndexCount, unit.IndexOffset, instanceCount);
        Rendering::GetController().GetRenderStatistics().AddEntry("shadow casts", 1);
    }

    bool InOrthoFrustrum(const FrustrumCuller& culler, const Vector3& minAABB, const Vector3& maxAABB)
    {
        return culler.IsAABBVisible(minAABB, maxAABB);
    };

    bool InSphereBounds(const PointLightUnit& pointLight, const Vector3& minAABB, const Vector3& maxAABB)
    {
        auto halfSize = 0.5f * (maxAABB - minAABB);
        auto pos = minAABB + halfSize;
        auto dist = RootThree<float>() * Max(halfSize.x, halfSize.y, halfSize.z);

        auto relative = pointLight.Position - pos;
        auto radius = dist + pointLight.Radius;
        return Dot(relative, relative) < radius * radius;
    }

    bool InConeBounds(const SpotLightUnit& spotLight, const Vector3& minAABB, const Vector3& maxAABB)
    {
        auto halfSize = 0.5f * (maxAABB - minAABB);
        auto pos = minAABB + halfSize;
        auto dist = RootThree<float>() * Max(halfSize.x, halfSize.y, halfSize.z);

        auto relative = pos - spotLight.Position;

        auto sinAngle = std::sqrt(1.0f - spotLight.OuterAngle * spotLight.OuterAngle);
        auto relativeWithSize = relative + spotLight.Direction * (dist / sinAngle);
        bool inside = Dot(Normalize(relativeWithSize), spotLight.Direction) > spotLight.OuterAngle;
        return inside || (Dot(relative, relative) < dist * dist);
    }

    template<typename CullFunc>
    void CastShadowsPerUnit(const CullFunc& culler, const Shader& shader, const RenderUnit& unit, size_t instanceCount, ArrayView<Material> materials)
    {
        // do not cull instanced objects, as their position may differ
        bool culled = instanceCount == 0 && !culler(unit.MinAABB, unit.MaxAABB);
        if (!culled)
        {
            RenderUnitToDepthMap(shader, instanceCount, unit, materials);
        }
        else
        {
            Rendering::GetController().GetRenderStatistics().AddEntry("culled from shadow cast", 1);
        }
    }

    template<typename CullFunc>
    void CastsShadowsPerGroup(const CullFunc& culler, const Shader& shader, const RenderList& shadowCasters, ArrayView<RenderUnit> units, ArrayView<Material> materials)
    {
        size_t currentUnit = 0;
        for (const auto& group : shadowCasters.Groups)
        {
            if (group.unitCount == 0) continue;

            group.VAO->Bind();
            group.IBO->Bind();
            for (size_t i = 0; i < group.unitCount; i++, currentUnit++)
            {
                const RenderUnit& unit = units[shadowCasters.UnitsIndex[currentUnit]];
                CastShadowsPerUnit(culler, shader, unit, group.InstanceCount, materials);
            }
        }
    }

    void ShadowMapGenerator::GenerateFor(const Shader& shader, ArrayView<DirectionalLightUnit> directionalLights)
    {
        auto& controller = Rendering::GetController();

        shader.Bind();
        for (auto& directionalLight : directionalLights)
        {
            controller.AttachDepthMap(directionalLight.ShadowMap);

            for (size_t i = 0; i < directionalLight.ProjectionMatrices.size(); i++)
            {
                const auto& projection = directionalLight.ProjectionMatrices[i];
                shader.SetUniform("LightProjMatrix", projection);
                shader.SetUniform("index", (int)i);

                auto CullingFunction = [culler = FrustrumCuller(projection)](const Vector3& min, const Vector3& max)
                {
                    return InOrthoFrustrum(culler, min, max);
                };

                CastsShadowsPerGroup(CullingFunction, shader, this->shadowCasters, this->renderUnits, this->materials);
            }

        }

        for (auto& directionalLight : directionalLights)
        {
            directionalLight.ShadowMap->GenerateMipmaps();
        }
    }

    void ShadowMapGenerator::GenerateFor(const Shader& shader, ArrayView<SpotLightUnit> spotLights)
    {
        auto& controller = Rendering::GetController();

        shader.Bind();
        for (auto& spotLight : spotLights)
        {
            controller.AttachDepthMap(spotLight.ShadowMap);
            shader.SetUniform("LightProjMatrix", spotLight.ProjectionMatrix);

            auto CullingFunction = [&spotLight](const Vector3& min, const Vector3& max)
            {
                return InConeBounds(spotLight, min, max);
            };

            CastsShadowsPerGroup(CullingFunction, shader, this->shadowCasters, this->renderUnits, this->materials);
        }

        for (auto& spotLight : spotLights)
        {
            spotLight.ShadowMap->GenerateMipmaps();
        }
    }

    void ShadowMapGenerator::GenerateFor(const Shader& shader, ArrayView<PointLightUnit> pointLights)
    {
        auto& controller = Rendering::GetController();

        shader.Bind();
        for (auto& pointLight : pointLights)
        {
            controller.AttachDepthMap(pointLight.ShadowMap);
            shader.SetUniform("LightProjMatrix[0]", pointLight.ProjectionMatrices[0]);
            shader.SetUniform("LightProjMatrix[1]", pointLight.ProjectionMatrices[1]);
            shader.SetUniform("LightProjMatrix[2]", pointLight.ProjectionMatrices[2]);
            shader.SetUniform("LightProjMatrix[3]", pointLight.ProjectionMatrices[3]);
            shader.SetUniform("LightProjMatrix[4]", pointLight.ProjectionMatrices[4]);
            shader.SetUniform("LightProjMatrix[5]", pointLight.ProjectionMatrices[5]);
            shader.SetUniform("zFar", pointLight.Radius);
            shader.SetUniform("lightPos", pointLight.Position);

            auto CullingFunction = [&pointLight](const Vector3& min, const Vector3& max)
            {
                return InSphereBounds(pointLight, min, max);
            };

            CastsShadowsPerGroup(CullingFunction, shader, this->shadowCasters, this->renderUnits, this->materials);
        }

        for (auto& pointLight : pointLights)
        {
            pointLight.ShadowMap->GenerateMipmaps();
        }
    }
}