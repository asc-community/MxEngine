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

#include "Core/Components/Rendering/DebugDraw.h"
#include "Core/Components/Rendering/MeshLOD.h"
#include "Core/Components/Rendering/MeshRenderer.h"
#include "Core/Components/Rendering/MeshSource.h"
#include "Core/Components/Rendering/Skybox.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Utilities/ImGui/Editors/ComponentEditors/GenericComponentEditor.h"
#include "Utilities/ImGui/ImGuiUtils.h"

namespace MxEngine::GUI
{
	void SkyboxEditor(Skybox& skybox)
	{
		ComponentEditor(skybox);
	}

	void DebugDrawEditor(DebugDraw& debugDraw)
	{
		ComponentEditor(debugDraw);
	}

	void LoadMaterialsFileDialog(const rttr::instance& parent)
	{
		if (ImGui::Button("load from file"))
		{
			auto& meshRenderer = *parent.try_convert<MeshRenderer>();
			MxString path = FileManager::OpenFileDialog();
			if (!path.empty() && File::Exists(path))
				meshRenderer = AssetManager::LoadMaterials(path);
		}
	}

	void MeshRendererEditor(MeshRenderer& meshRenderer)
	{
		ComponentEditor(meshRenderer);
	}

	void MeshSourceEditor(MeshSource& meshSource)
	{
		ComponentEditor(meshSource);
	}

	void MeshLODEditor(MeshLOD& meshLOD)
	{
		ComponentEditor(meshLOD);
	}
}