#include "Shader.h"
#include "Utilities/Logger/Logger.h"
#include "Core/OpenGL/GLUtils/GLUtils.h"
#include <glm/glm.hpp>

namespace MomoEngine
{
	Shader::Shader()
	{
		this->id = 0;
	}

	Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		Load(vertexShaderPath, fragmentShaderPath);
	}

	void Shader::Bind() const
	{
		GLCALL(glUseProgram(id));
	}

	void Shader::Unbind() const
	{
		GLCALL(glUseProgram(0));
	}

	Shader::Shader(Shader&& shader) noexcept
	{
		#ifdef _DEBUG
		this->vertexShaderPath = shader.vertexShaderPath;
		this->fragmentShaderPath = shader.fragmentShaderPath;
		#endif
		this->id = shader.id;
		shader.id = 0;
	}

	Shader::~Shader()
	{
		if (id != 0)
		{
			GLCALL(glDeleteProgram(id));
		}
	}

	void Shader::Load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		#ifdef _DEBUG
		this->vertexShaderPath = vertexShaderPath;
		this->fragmentShaderPath = fragmentShaderPath;
		#endif
		std::string vs = ReadFile(vertexShaderPath);
		std::string fs = ReadFile(fragmentShaderPath);

		if (vs.empty())
		{
			Logger::Instance().Error("MomoEngine::Shader", "shader file is empty: " + vertexShaderPath);
			return;
		}
		if (fs.empty())
		{
			Logger::Instance().Error("MomoEngine::Shader", "shader file is empty: " + fragmentShaderPath);
			return;
		}

		Logger::Instance().Debug("MomoEngine::Shader", "compiling vertex shader: " + vertexShaderPath);
		unsigned int vertexShader = CompileShader(ShaderType::VERTEX_SHADER, vs, vertexShaderPath);
		Logger::Instance().Debug("MomoEngine::Shader", "compiling fragment shader: " + fragmentShaderPath);
		unsigned int fragmentShader = CompileShader(ShaderType::FRAGMENT_SHADER, fs, fragmentShaderPath);

		id = CreateProgram(vertexShader, fragmentShader);
		Logger::Instance().Debug("MomoEngine::Shader", "shader program created with id = " + std::to_string(id));
	}

	void Shader::SetUniformFloat(const std::string& name, float f) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform1f(location, f));
	}

	void Shader::SetUniformVec3(const std::string& name, float f1, float f2, float f3) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform3f(location, f1, f2, f3));
	}

	void Shader::SetUniformVec4(const std::string& name, float f1, float f2, float f3, float f4) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform4f(location, f1, f2, f3, f4));
	}

	void Shader::SetUniformVec3(const std::string& name, const Vector3& vec) const
	{
		SetUniformVec3(name, vec[0], vec[1], vec[2]);
	}

	void Shader::SetUniformVec4(const std::string& name, const Vector3& vec) const
	{
		SetUniformVec4(name, vec[0], vec[1], vec[2], vec[3]);
	}

	void Shader::SetUniformMat4(const std::string& name, const Matrix4x4& matrix) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniformMatrix4fv(location, 1, false, &matrix[0][0]));
	}

	void Shader::SetUniformMat3(const std::string& name, const Matrix3x3& matrix) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniformMatrix3fv(location, 1, false, &matrix[0][0]));
	}

	void Shader::SetUniformInt(const std::string& name, int i) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform1i(location, i));
	}

	unsigned int Shader::CompileShader(ShaderType type, std::string& source, const std::string& name) const
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
			std::string msg;
			msg.resize(length);
			GLCALL(glGetShaderInfoLog(shaderId, length, &length, &msg[0]));
			msg.pop_back(); // extra \n character
			Logger::Instance().Error("MomoEngine::Shader", (std::string)"failed to compile " +
				(type == ShaderType::VERTEX_SHADER ? "vertex" : "fragment") + " shader: " + name);
			Logger::Instance().Error("OpenGL", msg);
		}

		return shaderId;
	}

	unsigned int Shader::CreateProgram(unsigned int vertexShader, unsigned int fragmentShader) const
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

	int Shader::GetUniformLocation(const std::string& uniformName) const
	{
		if (uniformCache.find(uniformName) != uniformCache.end())
			return uniformCache[uniformName];

		GLCALL(int location = glGetUniformLocation(id, uniformName.c_str()));
		if (location == -1)
			Logger::Instance().Warning("MomoEngine::Shader", "uniform was not found: " + uniformName);
		uniformCache[uniformName] = location;
		return location;
	}
}