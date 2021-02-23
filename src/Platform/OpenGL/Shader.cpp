// Copyright(c) 2019 - 2020, #Momo
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

#include "Shader.h"
#include "Utilities/Logging/Logger.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/FileSystem/FileManager.h"

namespace MxEngine
{
	std::array PipelineStageToNative = {
		GL_VERTEX_SHADER,
		GL_GEOMETRY_SHADER,
		GL_FRAGMENT_SHADER
	};

	std::array PipelineStageToString = {
		"vertex",
		"geometry",
		"fragment"
	};

	struct PipelineStageInfo
	{
		Shader::PipelineStage Stage;
		MxString SourceCode;
		FilePath Path;
	};

	Shader::BindableId Shader::CreateShaderProgram(ShaderId* ids, const PipelineStageInfo* stageInfos, size_t count)
	{
		// create each shader stage
		for (size_t i = 0; i < count; i++)
		{
			auto& stageInfo = stageInfos[i];
			MXLOG_DEBUG("OpenGL::Shader", "compiling " + MxString(PipelineStageToString[stageInfo.Stage]) + " shader");
			ids[i] = ShaderBase::CreateShader(PipelineStageToNative[stageInfo.Stage], stageInfo.SourceCode, stageInfo.Path);
		}

		// link stages into one shader program
		BindableId program = ShaderBase::CreateProgram(ids, count);
		MXLOG_DEBUG("OpenGL::Shader", "created shader program with id = " + ToMxString(program));

		// delete separate shader stages
		for (size_t i = 0; i < count; i++)
		{
			ShaderBase::DeleteShader(ids[i]);
		}

		return program;
	}

	void Shader::LoadDebugVariables(const PipelineStageInfo* stageInfos, size_t count)
	{
		#if defined(MXENGINE_DEBUG)
		for (size_t i = 0; i < count; i++)
		{
			auto& stageInfo = stageInfos[i];
			this->debugFilePaths[(size_t)stageInfo.Stage] = ToMxString(FileManager::GetProximatePath(stageInfo.Path, FileManager::GetWorkingDirectory()));
			auto filepaths = ShaderBase::GetShaderIncludeFiles(stageInfo.SourceCode, stageInfo.Path);
			this->includedFilePaths.insert(this->includedFilePaths.end(), eastl::make_move_iterator(filepaths.begin()), eastl::make_move_iterator(filepaths.end()));
		}
		#endif
	}

	template<>
	void Shader::Load(const FilePath& vertex, const FilePath& fragment)
	{
		std::array stageInfos = {
			PipelineStageInfo {
				PipelineStage::VERTEX,
				File::ReadAllText(vertex),
				vertex
			},
			PipelineStageInfo {
				PipelineStage::FRAGMENT,
				File::ReadAllText(fragment),
				fragment
			},
		};

		constexpr size_t StageCount = stageInfos.size();
		std::array<ShaderId, StageCount> ids;

		auto program = Shader::CreateShaderProgram(ids.data(), stageInfos.data(), StageCount);
		this->LoadDebugVariables(stageInfos.data(), StageCount);
		this->SetNewNativeHandle(program);
	}

	template<>
	void Shader::Load(const FilePath& vertex, const FilePath& geometry, const FilePath& fragment)
	{
		std::array stageInfos = {
			PipelineStageInfo {
				PipelineStage::VERTEX,
				File::ReadAllText(vertex),
				vertex
			},
			PipelineStageInfo {
				PipelineStage::GEOMETRY,
				File::ReadAllText(geometry),
				geometry
			},
			PipelineStageInfo {
				PipelineStage::FRAGMENT,
				File::ReadAllText(fragment),
				fragment
			},
		};

		constexpr size_t StageCount = stageInfos.size();
		std::array<ShaderId, StageCount> ids;

		auto program = Shader::CreateShaderProgram(ids.data(), stageInfos.data(), StageCount);
		this->LoadDebugVariables(stageInfos.data(), StageCount);
		this->SetNewNativeHandle(program);
	}

	void Shader::Load(const MxString& vertexPath, const MxString& fragmentPath)
	{
		this->Load(ToFilePath(vertexPath), ToFilePath(fragmentPath));
	}

	void Shader::Load(const MxString& vertexPath, const MxString& geometryPath, const MxString& fragmentPath)
	{
		this->Load(ToFilePath(vertexPath), ToFilePath(geometryPath), ToFilePath(fragmentPath));
	}

    void Shader::LoadFromString(const MxString& vertex, const MxString& fragment)
    {
		std::array stageInfos = {
			PipelineStageInfo {
				PipelineStage::VERTEX,
				vertex,
				FilePath("_vertex.glsl")
			},
			PipelineStageInfo {
				PipelineStage::FRAGMENT,
				fragment,
				FilePath("_fragment.glsl")
			},
		};

		constexpr size_t StageCount = stageInfos.size();
		std::array<ShaderId, StageCount> ids;

		auto program = Shader::CreateShaderProgram(ids.data(), stageInfos.data(), StageCount);
		this->LoadDebugVariables(stageInfos.data(), StageCount);
		this->SetNewNativeHandle(program);
    }

	void Shader::LoadFromString(const MxString& vertex, const MxString& geometry, const MxString& fragment)
	{
		std::array stageInfos = {
			PipelineStageInfo {
				PipelineStage::VERTEX,
				vertex,
				FilePath("_vertex.glsl")
			},
			PipelineStageInfo {
				PipelineStage::GEOMETRY,
				geometry,
				FilePath("_geometry.glsl")
			},
			PipelineStageInfo {
				PipelineStage::FRAGMENT,
				fragment,
				FilePath("_fragment.glsl")
			},
		};

		constexpr size_t StageCount = stageInfos.size();
		std::array<ShaderId, StageCount> ids;

		auto program = Shader::CreateShaderProgram(ids.data(), stageInfos.data(), StageCount);
		this->LoadDebugVariables(stageInfos.data(), StageCount);
		this->SetNewNativeHandle(program);
	}

	const MxString& Shader::GetDebugFilePath(Shader::PipelineStage stage) const
	{
		#if defined(MXENGINE_DEBUG)
		return this->debugFilePaths[(size_t)stage];
		#else
		const static MxString EmptyFilePath;
		return EmptyFilePath;
		#endif
	}

	const MxVector<MxString>& Shader::GetIncludedFilePaths() const
	{
		#if defined(MXENGINE_DEBUG)
		return this->includedFilePaths;
		#else
		const static MxVector<MxString> EmptyVector;
		return EmptyVector;
		#endif
	}
}