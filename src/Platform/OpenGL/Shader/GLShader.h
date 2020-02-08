#pragma once

#include "Core/Interfaces/GraphicAPI/Shader.h"
#include "Utilities/Math/Math.h"

#include <unordered_map>

namespace MxEngine
{
	class GLShader : public Shader
	{
        enum class ShaderType
        {
            FRAGMENT_SHADER = 0x8B30,
            VERTEX_SHADER = 0x8B31,
        };

		#ifdef _DEBUG
		std::string vertexShaderPath, fragmentShaderPath;
		#endif
        using UniformType = int;
        using UniformCache = std::unordered_map<std::string, UniformType>;
        using ShaderId = unsigned int;

		mutable UniformCache uniformCache;

        std::string ReadFile(const std::string& path) const;
		ShaderId CompileShader(ShaderType type, std::string& source, const std::string& name) const;
		IBindable::IdType CreateProgram(ShaderId vertexShader, ShaderId fragmentShader) const;
		UniformType GetUniformLocation(const std::string& uniformName) const;
	public:
		GLShader();
		GLShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		GLShader(const GLShader&) = delete;
		GLShader(GLShader&& shader) noexcept;
		~GLShader();

        // Inherited via IShader
        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual void Load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) override;
        virtual void SetUniformFloat(const std::string& name, float f) const override;
        virtual void SetUniformVec3(const std::string& name, float f1, float f2, float f3) const override;
        virtual void SetUniformVec4(const std::string& name, float f1, float f2, float f3, float f4) const override;
        virtual void SetUniformVec3(const std::string& name, const Vector3& vec) const override;
        virtual void SetUniformVec4(const std::string& name, const Vector3& vec) const override;
        virtual void SetUniformMat4(const std::string& name, const Matrix4x4& matrix) const override;
        virtual void SetUniformMat3(const std::string& name, const Matrix3x3& matrix) const override;
        virtual void SetUniformInt(const std::string& name, int i) const override;
    };
}
