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

#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>
#include <algorithm>

namespace MxEngine
{
	using Vector2 = glm::vec<2, float>;
	using Vector3 = glm::vec<3, float>;
	using Vector4 = glm::vec<4, float>;

	using VectorInt2 = glm::vec<2, int>;
	using VectorInt3 = glm::vec<3, int>;
	using VectorInt4 = glm::vec<4, int>;

	using Matrix2x2 = glm::mat2x2;
	using Matrix2x3 = glm::mat2x3;
	using Matrix3x3 = glm::mat3x3;
	using Matrix2x4 = glm::mat2x4;
	using Matrix3x4 = glm::mat3x4;
	using Matrix4x4 = glm::mat4x4;

	using Quaternion = glm::quat;

	template<size_t Length, typename Type>
	using Vector = glm::vec<Length, Type>;

	template<size_t Columns, size_t Rows, typename Type>
	using Matrix = glm::mat<Columns, Rows, Type>;

	inline Vector2 MakeVector2(float x, float y)
	{
		return Vector2(x, y);
	}

	inline Vector3 MakeVector3(float x, float y, float z)
	{
		return Vector3(x, y, z);
	}

	inline Vector4 MakeVector4(float x, float y, float z, float w)
	{
		return Vector4(x, y, z, w);
	}

	inline Vector2 MakeVector2(float value)
	{
		return Vector2(value);
	}

	inline Vector3 MakeVector3(float value)
	{
		return Vector3(value);
	}

	inline Vector4 MakeVector4(float value)
	{
		return Vector4(value);
	}

	inline Vector3 Cross(const Vector3& v1, const Vector3& v2)
	{
		return glm::cross(v1, v2);
	}

	inline Matrix4x4 MakeViewMatrix(const Vector3& eye, const Vector3& center, const Vector3& up)
	{
		return glm::lookAt(eye, center, up);
	}

	inline Matrix4x4 MakePerspectiveMatrix(float fov, float aspect, float znear, float zfar)
	{
		return glm::perspective(fov, aspect, znear, zfar);
	}

	inline Matrix4x4 MakeInversePerspectiveMatrix(float fov, float aspect, float znear, float zfar)
	{
		assert(std::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
		// zfar is unused. It is okay, as -1.0f in Result[2][3] means infinity zfar

		float const tanHalfFovy = std::tan(fov / 2.0f);

		Matrix4x4 Result(0.0f);
		Result[0][0] = 1.0f / (tanHalfFovy * aspect);
		Result[1][1] = 1.0f / tanHalfFovy;
		Result[2][3] = -1.0f;
		Result[3][2] = znear;
		return Result;
	}

	inline Matrix4x4 MakeOrthographicMatrix(float left, float right, float bottom, float top, float znear, float zfar)
	{
		return glm::ortho(left, right, bottom, top, znear, zfar);
	}

	inline Matrix4x4 MakeBiasMatrix()
	{
		Matrix4x4 Result(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f
		);
		return Result;
	}

	template<typename T>
	inline T Normalize(const T& value)
	{
		return glm::normalize(value);
	}

	template<typename T>
	inline auto Length(const T& value)
	{
		return glm::length(value);
	}

	template<typename T>
	inline auto Length2(const T& value)
	{
		return glm::length2(value);
	}

	inline Matrix4x4 Translate(const Matrix4x4& mat, const Vector3& vec)
	{
		return glm::translate(mat, vec);
	}

	inline Matrix4x4 Scale(const Matrix4x4& mat, const Vector3& vec)
	{
		return glm::scale(mat, vec);
	}

	inline Matrix4x4 Scale(const Matrix4x4& mat, float value)
	{
		return glm::scale(mat, MakeVector3(value));
	}

	inline Matrix4x4 Rotate(const Matrix4x4& mat, float angle, const Vector3& axis)
	{
		return glm::rotate(mat, angle, axis);
	}

	inline Matrix4x4 ToMatrix(const Quaternion& q)
	{
		return glm::toMat4(q);
	}

	inline Quaternion MakeQuaternion(float angle, const Vector3& axis)
	{
		return glm::angleAxis(angle, axis);
	}

	inline Vector3 MakeEulerAngles(const Quaternion& q)
	{
		return glm::eulerAngles(q);
	}

	inline Quaternion Lerp(const Quaternion& q1, const Quaternion q2, float a)
	{
		return glm::lerp(q1, q2, a);
	}

	inline Quaternion Slerp(const Quaternion& q1, const Quaternion q2, float a)
	{
		return glm::slerp(q1, q2, a);
	}

	template<size_t Columns, size_t Rows, typename T>
	inline Matrix<Columns, Rows, T> Transpose(const Matrix<Columns, Rows, T>& mat)
	{
		return glm::transpose(mat);
	}

	template<size_t Columns, size_t Rows, typename T>
	inline Matrix<Columns, Rows, T> Inverse(const Matrix<Columns, Rows, T>& mat)
	{
		return glm::inverse(mat);
	}

	template<typename T>
	inline constexpr T Clamp(const T& value, const T& low, const T& high)
	{
		return glm::clamp(value, low, high);
	}

	template<typename T, typename U>
	inline constexpr decltype(std::declval<T>() + std::declval<U>()) Max(const T& v1, const U& v2)
	{
		return (v1 > v2 ? v1 : v2);
	}

	template<typename T, typename U>
	inline constexpr decltype(std::declval<T>() + std::declval<U>()) Min(const T& v1, const U& v2)
	{
		return (v1 < v2 ? v1 : v2);
	}

	template<typename T>
	inline constexpr T Radians(const T& degrees)
	{
		return glm::radians(degrees);
	}

	template<typename T>
	inline constexpr T Degrees(const T& radians)
	{
		return glm::degrees(radians);
	}

	template<typename T>
	inline constexpr T Zero()
	{
		return glm::zero<T>();
	}

	template<typename T>
	inline constexpr T One()
	{
		return glm::one<T>();
	}

	template<typename T>
	inline constexpr T Pi()
	{
		return glm::pi<T>();
	}

	template<typename T>
	inline constexpr T epsilon()
	{
		return glm::epsilon<T>();
	}

	template<typename T>
	inline constexpr T TwoPi()
	{
		return glm::two_pi<T>();
	}

	template<typename T>
	inline constexpr T RootPi()
	{
		return glm::root_pi<T>();
	}

	template<typename T>
	inline constexpr T HalfPi()
	{
		return glm::half_pi<T>();
	}

	template<typename T>
	inline constexpr T ThreeOverTwoPi()
	{
		return glm::three_over_two_pi<T>();
	}

	template<typename T>
	inline constexpr T QuarterPi()
	{
		return glm::quarter_pi<T>();
	}

	template<typename T>
	inline constexpr T OneOverPi()
	{
		return glm::one_over_pi<T>();
	}

	template<typename T>
	inline constexpr T OneOverTwoPi()
	{
		return glm::one_over_two_pi<T>();
	}

	template<typename T>
	inline constexpr T TwoOverPi()
	{
		return glm::two_over_pi<T>();
	}

	template<typename T>
	inline constexpr T FourOverPi()
	{
		return glm::four_over_pi<T>();
	}

	template<typename T>
	inline constexpr T TwoOverRootPi()
	{
		return glm::two_over_root_pi<T>();
	}

	template<typename T>
	inline constexpr T OneOverRootTwo()
	{
		return glm::one_over_root_two<T>();
	}

	template<typename T>
	inline constexpr T RootHalfPi()
	{
		return glm::root_half_pi<T>();
	}

	template<typename T>
	inline constexpr T RootTwoPi()
	{
		return glm::root_two_pi<T>();
	}

	template<typename T>
	inline constexpr T RootLnFour()
	{
		return glm::root_ln_four<T>();
	}

	template<typename T>
	inline constexpr T e()
	{
		return glm::e<T>();
	}

	template<typename T>
	inline constexpr T Euler()
	{
		return glm::euler<T>();
	}

	template<typename T>
	inline constexpr T RootTwo()
	{
		return glm::root_two<T>();
	}

	template<typename T>
	inline constexpr T RootThree()
	{
		return glm::root_three<T>();
	}

	template<typename T>
	inline constexpr T RootFive()
	{
		return glm::root_five<T>();
	}

	template<typename T>
	inline constexpr T LnTwo()
	{
		return glm::ln_two<T>();
	}

	template<typename T>
	inline constexpr T LnTen()
	{
		return glm::ln_ten<T>();
	}

	template<typename T>
	inline constexpr T LnLnTwo()
	{
		return glm::ln_ln_two<T>();
	}

	template<typename T>
	inline constexpr T Third()
	{
		return glm::third<T>();
	}

	template<typename T>
	inline constexpr T TwoThirds()
	{
		return glm::two_thirds<T>();
	}

	template<typename T>
	inline constexpr T GoldenRatio()
	{
		return glm::golden_ratio<T>();
	}

	inline std::pair<Vector3, Vector3>MinMaxComponents(Vector3* verteces, size_t size)
	{
		Vector3 maxCoords(-1.0f * std::numeric_limits<float>::max());
		Vector3 minCoords(std::numeric_limits<float>::max());
		for (size_t i = 0; i < size; i++)
		{
			minCoords.x = std::min(minCoords.x, verteces[i].x);
			minCoords.y = std::min(minCoords.y, verteces[i].y);
			minCoords.z = std::min(minCoords.z, verteces[i].z);

			maxCoords.x = std::max(maxCoords.x, verteces[i].x);
			maxCoords.y = std::max(maxCoords.y, verteces[i].y);
			maxCoords.z = std::max(maxCoords.z, verteces[i].z);
		}
		return { minCoords, maxCoords };
	}

	inline Vector3 FindCenter(Vector3* verteces, size_t size)
	{
		auto coords = MinMaxComponents(verteces, size);
		return (coords.first + coords.second) * 0.5f;
	}

	constexpr size_t Log2(size_t n)
	{
		return ((n <= 2) ? 1 : 1 + Log2(n / 2));
	}
}
