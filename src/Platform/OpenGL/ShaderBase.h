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

#pragma once

#include "Utilities/Math/Math.h"
#include "Core/Macro/Macro.h"
#include "Utilities/STL/MxString.h"
#include "Utilities/STL/MxVector.h"
#include "Utilities/STL/MxHashMap.h"

namespace MxEngine
{
	class ShaderBase
	{
	public:
		using UniformIdType = int;
		using ShaderId = unsigned int;
		using BindableId = unsigned int;
		using ShaderTypeEnum = int;

		class UniformCache
		{
			MxHashMap<MxString, UniformIdType> cache;
			BindableId shaderId;

		public:
			constexpr static UniformIdType InvalidLocation = -1;

			UniformCache(BindableId shaderId);

			UniformIdType GetUniformLocation(const char* uniformName);
			UniformIdType GetUniformLocationSilent(const char* uniformName);
		};
	private:
		static BindableId CurrentlyAttachedShader;

		BindableId id = 0;
		mutable UniformCache uniformCache;

		void FreeProgram();
	protected:
		static BindableId CreateProgram(const ShaderId* ids, size_t shaderCount);
		template<typename FilePath> static ShaderId CreateShader(ShaderTypeEnum type, const MxString& sourceCode, const FilePath& filepath);
		template<typename FilePath> static MxVector<MxString> GetShaderIncludeFiles(const MxString& sourceCode, const FilePath& filepath);
		static void DeleteShader(ShaderId id);

		void SetNewNativeHandle(BindableId id);
	public:
		static MxString GetShaderVersionString();

		ShaderBase();
		~ShaderBase();
		ShaderBase(const ShaderBase&) = delete;
		ShaderBase(ShaderBase&& shader) noexcept;
		ShaderBase& operator=(const ShaderBase&) = delete;
		ShaderBase& operator=(ShaderBase&& shader) noexcept;

		void Bind() const;
		void Unbind() const;
		BindableId GetNativeHandle() const;

		void InvalidateUniformCache();
		void IgnoreNonExistingUniform(const MxString& name) const;
		void IgnoreNonExistingUniform(const char* name) const;
		UniformIdType GetUniformLocation(const MxString& name) const;
		UniformIdType GetUniformLocation(const char* name) const;

		void SetUniform(const MxString& name, float             f) const;
		void SetUniform(const MxString& name, const Vector2&    v) const;
		void SetUniform(const MxString& name, const Vector3&    v) const;
		void SetUniform(const MxString& name, const Vector4&    v) const;
		void SetUniform(const MxString& name, const VectorInt2& v) const;
		void SetUniform(const MxString& name, const VectorInt3& v) const;
		void SetUniform(const MxString& name, const VectorInt4& v) const;
		void SetUniform(const MxString& name, const Matrix2x2&  m) const;
		void SetUniform(const MxString& name, const Matrix3x3&  m) const;
		void SetUniform(const MxString& name, const Matrix4x4&  m) const;
		void SetUniform(const MxString& name, int               i) const;
		void SetUniform(const MxString& name, bool              b) const;
	};
}
