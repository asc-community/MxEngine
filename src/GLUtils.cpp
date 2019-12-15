#include "GLutils.h"

#include <fstream>

void GlClearErrors()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GlLogCall(const char* function, const char* file, int line)
{
	bool success = true;
	while (GLenum error = glGetError())
	{
		success = false;
		const GLubyte* errMsg = gluErrorString(error);
		setlocale(LC_ALL, "ru");
		std::cerr << "[OpenGL error]: " << errMsg << std::endl;
		std::cerr << function << " in file: " << file << ", line: " << line << std::endl;
	}
	return success;
}

unsigned int GetGLTypeSize(unsigned int type)
{
	switch (type)
	{
	case GL_FLOAT:
		return sizeof(GLfloat);
	case GL_UNSIGNED_INT:
		return sizeof(GLuint);
	case GL_UNSIGNED_BYTE:
		return sizeof(GLubyte);
	default:
		return 0;
	}
}

std::string ReadFile(const std::string& filename)
{
	std::ifstream file(filename);
	std::string content;
	content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	return content;
}

template<>
unsigned int GetGLType<float>()
{
	return GL_FLOAT;
}

template<>
unsigned int GetGLType<unsigned int>()
{
	return GL_UNSIGNED_INT;
}

template<>
unsigned int GetGLType<unsigned char>()
{
	return GL_UNSIGNED_BYTE;
}