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

#include "MeshRenderer.h"
#include "Utilities/ObjectLoader/ObjectLoader.h"
#include "Core/Resources/ResourceFactory.h"

namespace MxEngine
{
	Resource<Material, ResourceFactory> ConvertMaterial(const MaterialInfo& mat, MxHashMap<StringId, GResource<Texture>>& textures)
	{
		auto materialResource = ResourceFactory::Create<Material>();
		auto& material = *materialResource;

		#define MAKE_TEX(tex) if(!mat.tex.empty()) {\
		auto id = MakeStringId(mat.tex);\
		if(textures.find(id) == textures.end())\
			textures[id] = GraphicFactory::Create<Texture>(mat.tex);\
		material.tex = textures[id];}

		MAKE_TEX(AmbientMap);
		MAKE_TEX(DiffuseMap);
		MAKE_TEX(SpecularMap);
		MAKE_TEX(EmmisiveMap);
		MAKE_TEX(HeightMap);
		MAKE_TEX(NormalMap);

		material.AmbientColor = mat.AmbientColor;
		material.DiffuseColor = mat.DiffuseColor;
		material.EmmisiveColor = mat.EmmisiveColor;
		material.SpecularColor = mat.SpecularColor;
		material.SpecularExponent = mat.SpecularExponent;
		material.Transparency = mat.Transparency;
		material.Name = mat.Name;

		if (material.SpecularExponent == 0.0f) material.SpecularExponent = 128.0f; // bad as pow(0.0, 0.0) -> NaN //-V550

		return materialResource;
	}

	MeshRenderer::MaterialArray MeshRenderer::LoadMaterials(const MxString& path)
	{
		MaterialArray materials;
		MxHashMap<StringId, GResource<Texture>> textures;
		auto materialLibPath = path + MeshRenderer::GetMaterialFileSuffix();
		auto materialLibrary = ObjectLoader::LoadMaterials(materialLibPath);

		materials.resize(materialLibrary.size());
		for (size_t i = 0; i < materialLibrary.size(); i++)
		{
			materials[i] = ConvertMaterial(materialLibrary[i], textures);
		}

		return materials;
	}

	MxString MeshRenderer::GetMaterialFileSuffix()
	{
		return ".mx_matlib";
	}
}