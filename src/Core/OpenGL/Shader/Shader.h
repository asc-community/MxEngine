#pragma once

#include "Core/Interfaces/IBindable.h"
#include <unordered_map>
#include <glm/ext.hpp>

namespace MomoEngine
{
	enum class ShaderType
	{
		FRAGMENT_SHADER = 0x8B30,
		VERTEX_SHADER = 0x8B31,
	};

	class Shader : IBindable
	{
		#ifdef _DEBUG
		std::string vertexShaderPath, fragmentShaderPath;
		#endif
		mutable std::unordered_map<std::string, int> uniformCache;

		unsigned int CompileShader(ShaderType type, std::string& source, const std::string& name) const;
		unsigned int CreateProgram(unsigned int vertexShader, unsigned int fragmentShader) const;
		int GetUniformLocation(const std::string& uniformName) const;
	public:
		Shader();
		Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		Shader(const Shader&) = delete;
		Shader(Shader&& shader) noexcept;
		~Shader();
		
		void Load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

		void SetUniformFloat(const std::string& name, float f) const;
		void SetUniformVec3(const std::string& name, float f1, float f2, float f3) const;
		void SetUniformVec4(const std::string& name, float f1, float f2, float f3, float f4) const;
		void SetUniformMat4(const std::string& name, const glm::mat4x4& matrix) const;
		void SetUniformMat3(const std::string& name, const glm::mat3x3& matrix) const;
		void SetUniformInt(const std::string& name, int i) const;

		// Унаследовано через IBindable
		virtual void Bind() const override;
		virtual void Unbind() const override;
	};
}
