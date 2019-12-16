#pragma once

#include "GLUtils.h"
#include "IBindable.h"
#include <unordered_map>
#include <glm/glm.hpp>

namespace MomoEngine
{
	class Shader : IBindable
	{
		#ifdef _DEBUG
		std::string vertexShaderPath, fragmentShaderPath;
		#endif
		mutable std::unordered_map<std::string, int> uniformCache;

		unsigned int CompileShader(GLenum type, std::string& source) const;
		unsigned int CreateProgram(unsigned int vertexShader, unsigned int fragmentShader) const;
		int GetUniformLocation(const std::string& uniformName) const;
	public:
		Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		Shader(const Shader&) = delete;
		Shader(Shader&&) = delete;
		~Shader();

		void SetUniformVec4(const std::string& name, float f1, float f2, float f3, float f4) const;
		void SetUniformMat4(const std::string& name, const glm::mat4x4& matrix) const;
		void SetUniformInt(const std::string& name, int i) const;

		// Унаследовано через IBindable
		virtual void Bind() const override;
		virtual void Unbind() const override;
	};
}
