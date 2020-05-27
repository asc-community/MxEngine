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

#include "GLUtilities.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/Format/Format.h"

#include <fstream>
#include <string>
#include <set>
#include <sstream>

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
			if (message == nullptr) message = "unknown";
			setlocale(LC_ALL, "");
			Logger::Instance().Error("OpenGL::ErrorHandler", MxFormat(FMT_STRING("{0}\n  {1} in file: {2}, line: {3}"), message, function, file, line));
		}
		return success;
	}

	size_t GetGLTypeSize(unsigned int type)
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

    void PrintDebugInformation(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131184 || id == 131185 || id == 131218 || id == 131204) return;

		std::stringstream out;
		out << "device message [errcode " << id << "]: " << message << "\n        ";

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             out << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   out << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: out << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     out << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     out << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           out << "Source: Other"; break;
		} out << "\n        ";

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               out << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: out << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  out << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         out << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         out << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              out << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          out << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           out << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               out << "Type: Other"; break;
		} out << "\n        ";

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         out << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       out << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          out << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: out << "Severity: notification"; break;
		}

		Logger::Instance().Error("OpenGL", ToMxString(out.str()));
	}

	template<>
	const char* TypeToString<unsigned char>()
	{
		return "ubyte";
	}

	template<>
	const char* TypeToString<unsigned int>()
	{
		return "uint";
	}

	template<>
	const char* TypeToString<float>()
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