#include "GLShader.h"
#include "Utilities/Logger/Logger.h"

#include "Platform/OpenGL/GLUtilities/GLUtilities.h"
#include <fstream>

namespace MxEngine
{
	GLShader::GLShader()
	{
		this->id = 0;
	}

	GLShader::GLShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		Load(vertexShaderPath, fragmentShaderPath);
	}

	void GLShader::Bind() const
	{
		GLCALL(glUseProgram(id));
	}

	void GLShader::Unbind() const
	{
		GLCALL(glUseProgram(0));
	}

	GLShader::GLShader(GLShader&& shader) noexcept
	{
		#ifdef _DEBUG
		this->vertexShaderPath = shader.vertexShaderPath;
		this->fragmentShaderPath = shader.fragmentShaderPath;
		#endif
		this->id = shader.id;
		shader.id = 0;
	}

	GLShader::~GLShader()
	{
		if (id != 0)
		{
			GLCALL(glDeleteProgram(id));
		}
	}

	void GLShader::Load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		#ifdef _DEBUG
		this->vertexShaderPath = vertexShaderPath;
		this->fragmentShaderPath = fragmentShaderPath;
		#endif
		std::string vs = ReadFile(vertexShaderPath);
		std::string fs = ReadFile(fragmentShaderPath);

		if (vs.empty())
		{
			Logger::Instance().Error("MxEngine::Shader", "shader file is empty: " + vertexShaderPath);
			return;
		}
		if (fs.empty())
		{
			Logger::Instance().Error("MxEngine::Shader", "shader file is empty: " + fragmentShaderPath);
			return;
		}

		Logger::Instance().Debug("MxEngine::Shader", "compiling vertex shader: " + vertexShaderPath);
		unsigned int vertexShader = CompileShader(ShaderType::VERTEX_SHADER, vs, vertexShaderPath);
		Logger::Instance().Debug("MxEngine::Shader", "compiling fragment shader: " + fragmentShaderPath);
		unsigned int fragmentShader = CompileShader(ShaderType::FRAGMENT_SHADER, fs, fragmentShaderPath);

		id = CreateProgram(vertexShader, fragmentShader);
		Logger::Instance().Debug("MxEngine::Shader", "shader program created with id = " + std::to_string(id));
	}

	void GLShader::SetUniformFloat(const std::string& name, float f) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform1f(location, f));
	}

	void GLShader::SetUniformVec3(const std::string& name, float f1, float f2, float f3) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform3f(location, f1, f2, f3));
	}

	void GLShader::SetUniformVec4(const std::string& name, float f1, float f2, float f3, float f4) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform4f(location, f1, f2, f3, f4));
	}

	void GLShader::SetUniformVec3(const std::string& name, const Vector3& vec) const
	{
		SetUniformVec3(name, vec[0], vec[1], vec[2]);
	}

	void GLShader::SetUniformVec4(const std::string& name, const Vector3& vec) const
	{
		SetUniformVec4(name, vec[0], vec[1], vec[2], vec[3]);
	}

	void GLShader::SetUniformMat4(const std::string& name, const Matrix4x4& matrix) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniformMatrix4fv(location, 1, false, &matrix[0][0]));
	}

	void GLShader::SetUniformMat3(const std::string& name, const Matrix3x3& matrix) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniformMatrix3fv(location, 1, false, &matrix[0][0]));
	}

	void GLShader::SetUniformInt(const std::string& name, int i) const
	{
		int location = GetUniformLocation(name);
		if (location == -1) return;
		Bind();
		GLCALL(glUniform1i(location, i));
	}

    std::string GLShader::ReadFile(const std::string& filename) const
    {
        std::ifstream file(filename);
        if (file.bad())
        {
            Logger::Instance().Error("MxEngine::FileReader", "file with name '" + filename + "' was not found");
            return "";
        }
        std::string content;
        content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        return content;
    }

    GLShader::ShaderId GLShader::CompileShader(ShaderType type, std::string& source, const std::string& name) const
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
			Logger::Instance().Error("MxEngine::Shader", (std::string)"failed to compile " +
				(type == ShaderType::VERTEX_SHADER ? "vertex" : "fragment") + " shader: " + name);
			Logger::Instance().Error("OpenGL", msg);
		}

		return shaderId;
	}

	IBindable::IdType GLShader::CreateProgram(GLShader::ShaderId vertexShader, GLShader::ShaderId fragmentShader) const
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

	int GLShader::GetUniformLocation(const std::string& uniformName) const
	{
		if (uniformCache.find(uniformName) != uniformCache.end())
			return uniformCache[uniformName];

		GLCALL(int location = glGetUniformLocation(id, uniformName.c_str()));
		if (location == -1)
			Logger::Instance().Warning("MxEngine::Shader", "uniform was not found: " + uniformName);
		uniformCache[uniformName] = location;
		return location;
	}
}