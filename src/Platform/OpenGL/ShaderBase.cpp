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

#include "ShaderBase.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/Logging/Logger.h"
#include "Core/Config/GlobalConfig.h"
#include "Utilities/Parsing/ShaderPreprocessor.h"

namespace MxEngine
{
    ShaderBase::BindableId ShaderBase::CurrentlyAttachedShader = 0;

    ShaderBase::UniformCache::UniformCache(BindableId shaderId)
        : shaderId(shaderId) { }

    ShaderBase::UniformIdType ShaderBase::UniformCache::GetUniformLocation(const char* uniformName)
    {
        auto it = cache.find_as(uniformName);
        if (it != cache.end())
            return it->second;

        GLCALL(ShaderBase::UniformIdType location = glGetUniformLocation(this->shaderId, uniformName));

        if (location == UniformCache::InvalidLocation)
        {
            MXLOG_WARNING("OpenGL::Shader", "uniform was not found: " + MxString(uniformName));
        }

        cache[uniformName] = location;
        return location;
    }

    ShaderBase::UniformIdType ShaderBase::UniformCache::GetUniformLocationSilent(const char* uniformName)
    {
        auto it = cache.find_as(uniformName);
        if (it != cache.end())
            return it->second;

        GLCALL(ShaderBase::UniformIdType location = glGetUniformLocation(this->shaderId, uniformName));
        cache[uniformName] = location;
        return location;
    }

    MxString ShaderBase::GetShaderVersionString()
    {
        return "#version " + ToMxString(GlobalConfig::GetGraphicAPIMajorVersion() * 100 + GlobalConfig::GetGraphicAPIMinorVersion() * 10);
    }

    void ShaderBase::FreeProgram()
    {
        if (this->id != 0)
        {
            MXLOG_DEBUG("OpenGL::Shader", "deleted shader program with id = " + ToMxString(id));
            GLCALL(glDeleteProgram(this->id));
        }
        this->id = 0;
    }

    ShaderBase::BindableId ShaderBase::CreateProgram(const ShaderId* ids, size_t shaderCount)
    {
        GLCALL(BindableId program = glCreateProgram());

        for (size_t i = 0; i < shaderCount; i++)
        {
            GLCALL(glAttachShader(program, ids[i]));
        }
        GLCALL(glLinkProgram(program));
        GLCALL(glValidateProgram(program));

        return program;
    }

    template<>
    MxVector<MxString> ShaderBase::GetShaderIncludeFiles<FilePath>(const MxString& sourceCode, const FilePath& path)
    {
        ShaderPreprocessor preprocessor(sourceCode);
        return preprocessor
            .LoadIncludes(path.parent_path())
            .GetIncludeFiles();
    }

    template<>
    ShaderBase::ShaderId ShaderBase::CreateShader<FilePath>(ShaderTypeEnum type, const MxString& sourceCode, const FilePath& path)
    {
        GLCALL(ShaderId shaderId = glCreateShader((GLenum)type));

        ShaderPreprocessor preprocessor(sourceCode);

        auto modifiedSourceCode = preprocessor
            .LoadIncludes(path.parent_path())
            .EmitPrefixLine(ShaderBase::GetShaderVersionString())
            .GetResult();

        auto sourceptr = modifiedSourceCode.c_str();
        GLCALL(glShaderSource(shaderId, 1, &sourceptr, nullptr));
        GLCALL(glCompileShader(shaderId));

        GLint result;
        GLCALL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result));
        if (result == GL_FALSE)
        {
            GLint length;
            GLCALL(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length));
            MxString msg;
            msg.resize(length);
            GLCALL(glGetShaderInfoLog(shaderId, length, &length, &msg[0]));
            msg.pop_back(); // extra \n character
            MxString typeName;
            MXLOG_ERROR("OpenGL::Shader", "failed to compile shader loaded from path: " + ToMxString(path));
            MXLOG_ERROR("OpenGL::ErrorHandler", msg);
        }

        return shaderId;
    }

    void ShaderBase::DeleteShader(ShaderId id)
    {
        GLCALL(glDeleteShader(id));
    }

    void ShaderBase::SetNewNativeHandle(BindableId id)
    {
        this->FreeProgram();
        this->id = id;
        this->uniformCache = UniformCache{ this->id };
    }

    void ShaderBase::Bind() const
    {
        GLCALL(glUseProgram(this->id));
        ShaderBase::CurrentlyAttachedShader = this->id;
    }

    void ShaderBase::Unbind() const
    {
        GLCALL(glUseProgram(0));
        ShaderBase::CurrentlyAttachedShader = 0;
    }

    ShaderBase::ShaderBase()
        : id(0), uniformCache(id) { }

    ShaderBase::~ShaderBase()
    {
        this->FreeProgram();
    }

    ShaderBase::ShaderBase(ShaderBase&& other) noexcept
        : id(other.id), uniformCache(std::move(other.uniformCache))
    {
        other.id = 0;
    }

    ShaderBase& ShaderBase::operator=(ShaderBase&& other) noexcept
    {
        this->FreeProgram();

        this->id = other.id;
        this->uniformCache = std::move(other.uniformCache);

        other.id = 0;

        return *this;
    }

    ShaderBase::BindableId ShaderBase::GetNativeHandle() const
    {
        return this->id;
    }

    void ShaderBase::SetUniform(const MxString& name, int i) const
    {
        MX_ASSERT(this->id == ShaderBase::CurrentlyAttachedShader);
        auto location = this->GetUniformLocation(name);
        if (location == ShaderBase::UniformCache::InvalidLocation)
            return;

        GLCALL(glUniform1i(location, i));
    }

    void ShaderBase::SetUniform(const MxString& name, bool b) const
    {
        this->SetUniform(name, (int)b);
    }

    void ShaderBase::SetUniform(const MxString& name, float f) const
    {
        MX_ASSERT(this->id == ShaderBase::CurrentlyAttachedShader);
        auto location = this->GetUniformLocation(name);
        if (location == ShaderBase::UniformCache::InvalidLocation)
            return;

        GLCALL(glUniform1f(location, f));
    }

    void ShaderBase::SetUniform(const MxString& name, const Vector2& v) const
    {
        MX_ASSERT(this->id == ShaderBase::CurrentlyAttachedShader);
        auto location = this->GetUniformLocation(name);
        if (location == ShaderBase::UniformCache::InvalidLocation)
            return;

        GLCALL(glUniform2f(location, v[0], v[1]));
    }

    void ShaderBase::SetUniform(const MxString& name, const Vector3& v) const
    {
        MX_ASSERT(this->id == ShaderBase::CurrentlyAttachedShader);
        auto location = this->GetUniformLocation(name);
        if (location == ShaderBase::UniformCache::InvalidLocation)
            return;

        GLCALL(glUniform3f(location, v[0], v[1], v[2]));
    }

    void ShaderBase::SetUniform(const MxString& name, const Vector4& v) const
    {
        MX_ASSERT(this->id == ShaderBase::CurrentlyAttachedShader);
        auto location = this->GetUniformLocation(name);
        if (location == ShaderBase::UniformCache::InvalidLocation)
            return;

        GLCALL(glUniform4f(location, v[0], v[1], v[2], v[3]));
    }

    void ShaderBase::SetUniform(const MxString& name, const VectorInt2& v) const
    {
        MX_ASSERT(this->id == ShaderBase::CurrentlyAttachedShader);
        auto location = this->GetUniformLocation(name);
        if (location == ShaderBase::UniformCache::InvalidLocation)
            return;

        GLCALL(glUniform2i(location, v[0], v[1]));
    }

    void ShaderBase::SetUniform(const MxString& name, const VectorInt3& v) const
    {
        MX_ASSERT(this->id == ShaderBase::CurrentlyAttachedShader);
        auto location = this->GetUniformLocation(name);
        if (location == ShaderBase::UniformCache::InvalidLocation)
            return;

        GLCALL(glUniform3i(location, v[0], v[1], v[2]));
    }

    void ShaderBase::SetUniform(const MxString& name, const VectorInt4& v) const
    {
        MX_ASSERT(this->id == ShaderBase::CurrentlyAttachedShader);
        auto location = this->GetUniformLocation(name);
        if (location == ShaderBase::UniformCache::InvalidLocation)
            return;

        GLCALL(glUniform4i(location, v[0], v[1], v[2], v[3]));
    }

    void ShaderBase::SetUniform(const MxString& name, const Matrix2x2& m) const
    {
        MX_ASSERT(this->id == ShaderBase::CurrentlyAttachedShader);
        auto location = this->GetUniformLocation(name);
        if (location == ShaderBase::UniformCache::InvalidLocation)
            return;

        GLCALL(glUniformMatrix2fv(location, 1, false, &m[0][0]));
    }

    void ShaderBase::SetUniform(const MxString& name, const Matrix3x3& m) const
    {
        MX_ASSERT(this->id == ShaderBase::CurrentlyAttachedShader);
        auto location = this->GetUniformLocation(name);
        if (location == ShaderBase::UniformCache::InvalidLocation)
            return;

        GLCALL(glUniformMatrix3fv(location, 1, false, &m[0][0]));
    }

    void ShaderBase::SetUniform(const MxString& name, const Matrix4x4& m) const
    {
        MX_ASSERT(this->id == ShaderBase::CurrentlyAttachedShader);
        auto location = this->GetUniformLocation(name);
        if (location == ShaderBase::UniformCache::InvalidLocation)
            return;

        GLCALL(glUniformMatrix4fv(location, 1, false, &m[0][0]));
    }

    void ShaderBase::IgnoreNonExistingUniform(const MxString& name) const
    {
        this->GetUniformLocation(name.c_str());
    }

    void ShaderBase::IgnoreNonExistingUniform(const char* name) const
    {
        (void)this->uniformCache.GetUniformLocationSilent(name);
    }

    ShaderBase::UniformIdType ShaderBase::GetUniformLocation(const MxString& name) const
    {
        return this->GetUniformLocation(name.c_str());
    }

    ShaderBase::UniformIdType ShaderBase::GetUniformLocation(const char* name) const
    {
        return this->uniformCache.GetUniformLocation(name);
    }

    void ShaderBase::InvalidateUniformCache()
    {
        this->uniformCache = UniformCache{ this->id };
    }
}