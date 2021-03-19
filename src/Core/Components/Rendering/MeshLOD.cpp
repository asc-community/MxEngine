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
#include "Core/MxObject/MxObject.h"
#include "Core/Runtime/Reflection.h"

namespace MxEngine
{
    void MeshLOD::FixBestLOD(const Vector3& viewportPosition, float viewportZoom)
    {
        if (!this->AutoLODSelection) return;
        auto& object = MxObject::GetByComponent(*this);
        auto meshSource = object.GetComponent<MeshSource>();
        if (!meshSource.IsValid()) 
        {
            this->SetCurrentLOD(0); 
            return;
        }

        auto box = meshSource->Mesh->MeshAABB * object.Transform.GetMatrix();

        float distance = Length(box.GetCenter() - viewportPosition);
        Vector3 length = box.Length();
        float maxLength = ComponentMax(length);
        float scaledDistance = maxLength / (distance * viewportZoom);

        // magic numbers which were measured in game to find best distance for each LOD peek
        constexpr static std::array lodDistance = {
            0.21f, 0.15f, 0.10f, 0.06f, 0.03f, 0.01f
        };
        this->SetCurrentLOD(0);
        while (this->currentLOD < lodDistance.size() && scaledDistance < lodDistance[this->currentLOD])
            this->currentLOD++;

        this->SetCurrentLOD(Min(this->currentLOD, this->LODs.size()));
    }

    void MeshLOD::SetCurrentLOD(size_t lod)
    {
        this->currentLOD = (uint8_t)Min(lod, (size_t)Max((int)this->LODs.size() - 1, 0));
    }

    size_t MeshLOD::GetCurrentLOD() const
    {
        return (size_t)this->currentLOD;
    }

    MeshHandle MeshLOD::GetMeshLOD() const
    {
        if (this->currentLOD == 0 || this->currentLOD >= this->LODs.size())
            return MxObject::GetByComponent(*this).GetComponent<MeshSource>()->Mesh;
        else
            return this->LODs[this->currentLOD - 1];
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<MeshLOD>("MeshLOD")
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::CLONE_COPY)
            )
            .constructor<>()
            .property("auto lod selection", &MeshLOD::AutoLODSelection)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            )
            .property("current lod", &MeshLOD::GetCurrentLOD, &MeshLOD::SetCurrentLOD)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(MetaInfo::CONDITION, +([](const rttr::instance& v) { return !v.try_convert<MeshLOD>()->AutoLODSelection; }))
            )
            .property("lods", &MeshLOD::LODs)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
            );
    }
}