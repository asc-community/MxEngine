#include "GLUtilities.h"
#include "Utilities/Logger/Logger.h"

#include <fstream>
#include <string>
#include <set>

namespace MxEngine
{
	static std::set<int> ExistingErrors;

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
			if (ExistingErrors.find(error) != ExistingErrors.end())
				continue;
			ExistingErrors.insert(error);
			const char* message = (const char*)gluErrorString(error);
			setlocale(LC_ALL, "ru");
			Logger::Instance().Error("OpenGL", 
				std::string(message) + "\n    " + function +
				" in file: " + file + ", line: " + std::to_string(line)
			);
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

	template<>
	std::string TypeToString<unsigned char>()
	{
		return "ubyte";
	}

	template<>
	std::string TypeToString<unsigned int>()
	{
		return "uint";
	}

	template<>
	std::string TypeToString<float>()
	{
		return "float";
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
}