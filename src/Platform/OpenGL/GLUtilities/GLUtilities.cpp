// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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
				std::string(message) + "\n	" + function +
				" in file: " + file + ", line: " + std::to_string(line)
			);
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