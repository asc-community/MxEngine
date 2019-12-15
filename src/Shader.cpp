#include "Shader.h"

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
#ifdef _DEBUG
	this->vertexShaderPath = vertexShaderPath;
	this->fragmentShaderPath = fragmentShaderPath;
#endif
	std::string vs = ReadFile(vertexShaderPath);
	std::string fs = ReadFile(fragmentShaderPath);

	unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vs);
	unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fs);

	id = CreateProgram(vertexShader, fragmentShader);
}

void Shader::Bind() const
{
	GLCALL(glUseProgram(id));
}

void Shader::Unbind() const
{
	GLCALL(glUseProgram(0));
}

void Shader::SetUniformVec4(const std::string& name, float f1, float f2, float f3, float f4) const
{
	int location = GetUniformLocation(name);
	GLCALL(glUniform4f(location, f1, f2, f3, f4));
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4x4& matrix) const
{
	int location = GetUniformLocation(name);
	GLCALL(glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]));
}

void Shader::SetUniformInt(const std::string& name, int i) const
{
	int location = GetUniformLocation(name);
	GLCALL(glUniform1i(location, i));
}

unsigned int Shader::CompileShader(GLenum type, std::string& source) const
{
	GLCALL(GLuint shaderId = glCreateShader(type));
	const char* src = source.c_str();
	GLCALL(glShaderSource(shaderId, 1, &src, nullptr));
	GLCALL(glCompileShader(shaderId));

	GLint result;
	GLCALL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		GLint length;
		GLCALL(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length));
		char* msg = (char*)alloca(length * sizeof(char));
		GLCALL(glGetShaderInfoLog(shaderId, length, &length, msg));
		std::cout << "[OpenGL error]: failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
		std::cout << msg << std::endl;
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
		std::cerr << "[OpenGL warning]: uniform " << uniformName << " was not found" << std::endl;
	else
		uniformCache[uniformName] = location;
	return location;
}
