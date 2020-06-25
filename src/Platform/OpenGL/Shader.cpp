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
#include "Utilities/Logger/Logger.h"
#include "Core/Macro/Macro.h"
#include "Platform/OpenGL/GLUtilities.h"
#include "Utilities/FileSystem/File.h"

namespace MxEngine
{
	enum class ShaderType
	{
		VERTEX_SHADER   = GL_VERTEX_SHADER,
		GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
		FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
	};

	Shader::Shader()
	{
		this->id = 0;
	}

	Shader::Shader(const MxString& vertexShaderPath, const MxString& fragmentShaderPath)
	{
		this->Load(vertexShaderPath, fragmentShaderPath);
	}

	Shader::Shader(const MxString& vertexShaderPath, const MxString& geometryShaderPath, const MxString& fragmentShaderPath)
	{
		this->Load(vertexShaderPath, geometryShaderPath, fragmentShaderPath);
	}

	void Shader::Bind() const
	{
		GLCALL(glUseProgram(id));
	}

	void Shader::Unbind() const
	{
		GLCALL(glUseProgram(0));
	}

    void Shader::InvalidateUniformCache()
    {
		this->uniformCache.clear();
    }

	Shader::BindableId Shader::GetNativeHandle() const
    {
		return id;
    }

	Shader::Shader(Shader&& shader) noexcept
	{
		#if defined(MXENGINE_DEBUG)
		this->vertexShaderPath = shader.vertexShaderPath;
		this->geometryShaderPath = shader.geometryShaderPath;
		this->fragmentShaderPath = shader.fragmentShaderPath;
		#endif
		this->id = shader.id;
		this->uniformCache = std::move(shader.uniformCache);
		shader.id = 0;
	}

	Shader& Shader::operator=(Shader&& shader) noexcept
	{
		#if defined(MXENGINE_DEBUG)
		this->vertexShaderPath = shader.vertexShaderPath;
		this->geometryShaderPath = shader.geometryShaderPath;
		this->fragmentShaderPath = shader.fragmentShaderPath;
		#endif
		this->id = shader.id;
		this->uniformCache = std::move(shader.uniformCache);
		shader.id = 0;
		return *this;
	}

	Shader::~Shader()
	{
		if (id != 0)
		{
			GLCALL(glDeleteProgram(id));
		}
	}

	void Shader::Load(const MxString& vertex, const MxString& fragment)
	{
		this->InvalidateUniformCache();
		#if defined(MXENGINE_DEBUG)
		this->vertexShaderPath = vertex;
		this->fragmentShaderPath = fragment;
		#endif
		MxString vs = File::ReadAllText(vertex);
		MxString fs = File::ReadAllText(fragment);

		if (fs.empty())
			Logger::Instance().Warning("OpenGL::Shader", "fragment shader is empty: " + fragment);
		if (vs.empty())
			Logger::Instance().Warning("OpenGL::Shader", "vertex shader is empty: " + vertex);

		Logger::Instance().Debug("OpenGL::Shader", "compiling vertex shader: " + vertex);
		unsigned int vertexShader = CompileShader((GLenum)ShaderType::VERTEX_SHADER, vs, vertex);
		Logger::Instance().Debug("OpenGL::Shader", "compiling fragment shader: " + fragment);
		unsigned int fragmentShader = CompileShader((GLenum)ShaderType::FRAGMENT_SHADER, fs, fragment);

		id = CreateProgram(vertexShader, fragmentShader);
		Logger::Instance().Debug("OpenGL::Shader", "shader program created with id = " + ToMxString(id));
	}

	void Shader::Load(const MxString& vertex, const MxString& geometry, const MxString& fragment)
	{
		this->InvalidateUniformCache();
		#if defined(MXENGINE_DEBUG)
		this->vertexShaderPath = vertex;
		this->geometryShaderPath = geometry;
		this->fragmentShaderPath = fragment;
		#endif
		MxString vs = File::ReadAllText(vertex);
		MxString gs = File::ReadAllText(geometry);
		MxString fs = File::ReadAllText(fragment);

		if (fs.empty())
			Logger::Instance().Warning("OpenGL::Shader", "fragment shader is empty: " + fragment);
		if (gs.empty())
			Logger::Instance().Warning("OpenGL::Shader", "fragment shader is empty: " + geometry);
		if (vs.empty())
			Logger::Instance().Warning("OpenGL::Shader", "vertex shader is empty: " + vertex);

		Logger::Instance().Debug("OpenGL::Shader", "compiling vertex shader: " + vertex);
		unsigned int vertexShader = CompileShader((GLenum)ShaderType::VERTEX_SHADER, vs, vertex);
		Logger::Instance().Debug("OpenGL::Shader", "compiling geometry shader: " + geometry);
		unsigned int geometryShader = CompileShader((GLenum)ShaderType::GEOMETRY_SHADER, gs, geometry);
		Logger::Instance().Debug("OpenGL::Shader", "compiling fragment shader: " + fragment);
		unsigned int fragmentShader = CompileShader((GLenum)ShaderType::FRAGMENT_SHADER, fs, fragment);

		id = CreateProgram(vertexShader, geometryShader, fragmentShader);
		Logger::Instance().Debug("OpenGL::Shader", "shader program created with id = " + ToMxString(id));
	}

    void Shader::LoadFromString(const MxString& vertex, const MxString& fragment)
    {
		this->InvalidateUniformCache();

		Logger::Instance().Debug("OpenGL::Shader", "compiling vertex shader: [[raw source]]");
		unsigned int vertexShader = CompileShader((GLenum)ShaderType::VERTEX_SHADER, vertex, "[[raw source]]");
		Logger::Instance().Debug("OpenGL::Shader", "compiling fragment shader: [[raw source]]");
		unsigned int fragmentShader = CompileShader((GLenum)ShaderType::FRAGMENT_SHADER, fragment, "[[raw source]]");

		id = CreateProgram(vertexShader, fragmentShader);
		Logger::Instance().Debug("OpenGL::Shader", "shader program created with id = " + ToMxString(id));
    }

	void Shader::LoadFromString(const MxString& vertex, const MxString& geometry, const MxString& fragment)
	{
		this->InvalidateUniformCache();

		Logger::Instance().Debug("OpenGL::Shader", "compiling vertex shader: [[raw source]]");
		unsigned int vertexShader = CompileShader((GLenum)ShaderType::VERTEX_SHADER, vertex, "[[raw source]]");
		Logger::Instance().Debug("OpenGL::Shader", "compiling geometry shader: [[raw source]]");
		unsigned int geometryShader = CompileShader((GLenum)ShaderType::GEOMETRY_SHADER, geometry, "[[raw source]]");
		Logger::Instance().Debug("OpenGL::Shader", "compiling fragment shader: [[raw source]]");
		unsigned int fragmentShader = CompileShader((GLenum)ShaderType::FRAGMENT_SHADER, fragment, "[[raw source]]");

		id = CreateProgram(vertexShader, geometryShader, fragmentShader);
		Logger::Instance().Debug("OpenGL::Shader", "shader program created with id = " + ToMxString(id));
	}

	void Shader::SetUniformFloat(const MxString& name, float f) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform1f(location, f));
	}

    void Shader::SetUniformVec2(const MxString& name, const Vector2& vec) const
    {
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform2f(location, vec.x, vec.y));
    }

	void Shader::SetUniformVec3(const MxString& name, const Vector3& vec) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform3f(location, vec.x, vec.y, vec.z));
	}

	void Shader::SetUniformVec4(const MxString& name, const Vector4& vec) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform4f(location, vec.x, vec.y, vec.z, vec.w));
	}

	void Shader::SetUniformMat4(const MxString& name, const Matrix4x4& matrix) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniformMatrix4fv(location, 1, false, &matrix[0][0]));
	}

	void Shader::SetUniformMat3(const MxString& name, const Matrix3x3& matrix) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniformMatrix3fv(location, 1, false, &matrix[0][0]));
	}

	void Shader::SetUniformInt(const MxString& name, int i) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform1i(location, i));
	}

	Shader::ShaderId Shader::CompileShader(unsigned int type, const MxString& source, const MxString& name) const
	{
		GLCALL(GLuint shaderId = glCreateShader((GLenum)type));
		const char* src = source.c_str();
		GLCALL(glShaderSource(shaderId, 1, &src, nullptr));
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
			switch ((ShaderType)type)
			{
			case ShaderType::VERTEX_SHADER:
				typeName = "vertex";
				break;
			case ShaderType::GEOMETRY_SHADER:
				typeName = "geometry";
				break;
			case ShaderType::FRAGMENT_SHADER:
				typeName = "fragment";
				break;
			}
			Logger::Instance().Error("OpenGL::Shader", "failed to compile " + typeName + " shader: " + name);
			Logger::Instance().Error("OpenGL", msg);
		}

		return shaderId;
	}

	Shader::BindableId Shader::CreateProgram(Shader::ShaderId vertexShader, Shader::ShaderId fragmentShader) const
	{
		GLCALL(unsigned int program = glCreateProgram());

		GLCALL(glAttachShader(program, vertexShader));
		GLCALL(glAttachShader(program, fragmentShader));
		GLCALL(glLinkProgram(program));
		GLCALL(glValidateProgram(program));

		GLCALL(glDeleteShader(vertexShader));
		GLCALL(glDeleteShader(fragmentShader));

		return program;
	}

	Shader::BindableId Shader::CreateProgram(ShaderId vertexShader, ShaderId geometryShader, ShaderId fragmentShader) const
	{
		GLCALL(unsigned int program = glCreateProgram());

		GLCALL(glAttachShader(program, vertexShader));
		GLCALL(glAttachShader(program, geometryShader));
		GLCALL(glAttachShader(program, fragmentShader));
		GLCALL(glLinkProgram(program));
		GLCALL(glValidateProgram(program));

		GLCALL(glDeleteShader(vertexShader));
		GLCALL(glDeleteShader(geometryShader));
		GLCALL(glDeleteShader(fragmentShader));

		return program;
	}

	int Shader::GetUniformLocation(const MxString& uniformName) const
	{
		if (uniformCache.find(uniformName) != uniformCache.end())
			return uniformCache[uniformName];

		GLCALL(int location = glGetUniformLocation(id, uniformName.c_str()));
		if (location == -1)
			Logger::Instance().Warning("OpenGL::Shader", "uniform was not found: " + uniformName);
		uniformCache[uniformName] = location;
		return location;
	}
}