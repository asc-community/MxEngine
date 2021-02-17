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

#include "ObjectSaver.h"
#include "Utilities/Logging/Logger.h"
#include "Utilities/Profiler/Profiler.h"

namespace MxEngine
{
    void SaveVerteciesIndiciesAsObj(const FilePath& filepath, const MeshData::VertexData& vertecies, const MeshData::IndexData& indicies)
    {
        MAKE_SCOPE_PROFILER("ObjectSaver::SaveMeshAsObj()");
        MAKE_SCOPE_TIMER("MxEngine::ObjectSaver", "ObjectSaver::SaveMeshAsObj()");

        File f(filepath, File::WRITE);
        if (!f.IsOpen())
        {
            MXLOG_ERROR("MxEngine::ObjectSaver", "cannot open file: " + ToMxString(filepath));
            return;
        }
        MXLOG_INFO("MxEngine::ObjectSaver", "saving .obj model to file: " + ToMxString(filepath));

        // vertex position
        for (const auto& vertex : vertecies)
        {
            f << "v  " << vertex.Position.x << ' ' << vertex.Position.y << ' ' << vertex.Position.z << '\n';
        }
        f << "# total vertex count: " << vertecies.size() << "\n\n";

        // vertex texcoord
        for (const auto& vertex : vertecies)
        {
            f << "vt  " << vertex.TexCoord.x << ' ' << vertex.TexCoord.y << '\n';
        }
        f << "# total texcoord count: " << vertecies.size() << "\n\n";

        // vertex normal
        for (const auto& vertex : vertecies)
        {
            f << "vn  " << vertex.Normal.x << ' ' << vertex.Normal.y << ' ' << vertex.Normal.z << '\n';
        }
        f << "# total normal count: " << vertecies.size() << "\n\n";

        // faces have +1 offset due to .obj format
        for (size_t i = 0; i < indicies.size(); i += 3)
        {
            f << "f  ";
            f << indicies[i + 0] + 1 << '/' << indicies[i + 0] + 1 << '/' << indicies[i + 0] + 1;
            f << ' ';
            f << indicies[i + 1] + 1 << '/' << indicies[i + 1] + 1 << '/' << indicies[i + 1] + 1;
            f << ' ';
            f << indicies[i + 2] + 1 << '/' << indicies[i + 2] + 1 << '/' << indicies[i + 2] + 1;
            f << '\n';
        }
        f << "# total index count: " << indicies.size() << '\n';
    }

    void ObjectSaver::SaveVerteciesIndicies(const FilePath& filepath, SupportedSaveFormats format, const MeshData::VertexData& vertecies, const MeshData::IndexData& indicies)
    {
        switch (format)
        {
        case MxEngine::SupportedSaveFormats::OBJ:
            SaveVerteciesIndiciesAsObj(filepath, vertecies, indicies);
            break;
        default:
            MXLOG_WARNING("MxEngine::ObjectSaver", "cannot determine save format for file: " + ToMxString(filepath));
            break;
        }
    }

    void ObjectSaver::SaveMeshData(const FilePath& filepath, SupportedSaveFormats format, const MeshData& meshData)
    {
        auto vertecies = meshData.GetVerteciesFromGPU();
        auto indicies = meshData.GetIndiciesFromGPU();
        return ObjectSaver::SaveVerteciesIndicies(filepath, format, vertecies, indicies);
    }
}