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

#include "MeshLOD.h"
#include "Utilities/LODGenerator/LODGenerator.h"
#include "Core/MxObject/MxObject.h"

namespace MxEngine
{
    void MeshLOD::Generate(LODConfig config)
    {
        auto& object = MxObject::GetByComponent(*this);
        auto meshSource = object.GetComponent<MeshSource>();
        if (!meshSource.IsValid() || !meshSource->GetMesh().IsValid())
        {
            Logger::Instance().Warning("MxEngine::MeshLOD", "LODs are not generated as object has no mesh: " + object.Name);
            return;
        }

        auto mesh = meshSource.GetUnchecked()->GetMesh();
        this->LODs.clear();
        this->LODs.reserve(config.Factors.size());

        for (auto factor : config.Factors)
        {
            auto meshLODhandle = this->LODs.emplace_back(ResourceFactory::Create<Mesh>());
            auto& meshLODsubmeshes = meshLODhandle->GetSubmeshes();
            meshLODsubmeshes.reserve(mesh->GetSubmeshes().size());

            size_t totalIndicies = 0;
            for (const auto& submesh : mesh->GetSubmeshes())
            {
                LODGenerator lod(submesh.MeshData);
                auto& submeshLOD = meshLODsubmeshes.emplace_back(submesh.GetMaterialId(), submesh.GetTransform());
                submeshLOD.Name = submesh.Name;
                submeshLOD.MeshData = lod.CreateObject(factor);
                totalIndicies += submeshLOD.MeshData.GetIndicies().size();
            }
            Logger::Instance().Debug("MxEngine::MeshLOD", MxFormat("generated LOD with {0} indicies for object: {1}", totalIndicies, object.Name.c_str()));
        }
    }
}