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

#include "ComputeShader.h"
#include "Utilities/Logging/Logger.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/FileSystem/FileManager.h"

namespace MxEngine
{
    template<>
    ComputeShader::BindableId ComputeShader::CreateShaderProgram<FilePath>(const MxString& source, const FilePath& path)
    {
        MXLOG_DEBUG("OpenGL::Shader", "compiling compute shader");
        ShaderId shaderId = ShaderBase::CreateShader(GL_COMPUTE_SHADER, source, path);

        BindableId program = ShaderBase::CreateProgram(&shaderId, 1);
        MXLOG_DEBUG("OpenGL::Shader", "created shader program with id = " + ToMxString(program));

        ShaderBase::DeleteShader(shaderId);

        return program;
    }

    void ComputeShader::LoadFromString(const MxString& source)
    {
        BindableId program = ComputeShader::CreateShaderProgram(source, FilePath("_compute.glsl"));
        this->SetNewNativeHandle(program);
    }

    template<>
    void ComputeShader::Load<FilePath>(const FilePath& path)
    {
        BindableId program = ComputeShader::CreateShaderProgram(File::ReadAllText(path), path);
        this->SetNewNativeHandle(program);
    }

    void ComputeShader::Load(const MxString& path)
    {
        this->Load(ToFilePath(path));
    }

    const MxString& ComputeShader::GetDebugFilePath()
	{
		#if defined(MXENGINE_DEBUG)
        return this->debugFilePath;
		#else
		const static MxString EmptyFilePath;
		return EmptyFilePath;
		#endif
	}

	const MxVector<MxString>& ComputeShader::GetIncludedFilePaths() const
	{
		#if defined(MXENGINE_DEBUG)
		return this->includedFilePaths;
		#else
		const static MxVector<MxString> EmptyVector;
		return EmptyVector;
		#endif
	}
}