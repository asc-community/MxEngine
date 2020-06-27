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

#include "Vendors/glm/glm.hpp"
#include "Vendors/glm/ext.hpp"
#include "Vendors/glm/gtx/quaternion.hpp"
#include "Core/Macro/Macro.h"
#include <cmath>
#include <array>
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

	constexpr inline Vector2 MakeVector2(float x, float y)
	{
		return Vector2(x, y);
	}

	constexpr inline Vector3 MakeVector3(float x, float y, float z)
	{
		return Vector3(x, y, z);
	}

	constexpr inline Vector4 MakeVector4(float x, float y, float z, float w)
	{
		return Vector4(x, y, z, w);
	}

	constexpr inline Vector2 MakeVector2(float value)
	{
		return Vector2(value);
	}

	constexpr inline Vector3 MakeVector3(float value)
	{
		return Vector3(value);
	}

	constexpr inline Vector4 MakeVector4(float value)
	{
		return Vector4(value);
	}

	template<typename Vector>
	inline float Dot(const Vector& v1, const Vector& v2)
	{
		return glm::dot(v1, v2);
	}

	inline Vector3 Cross(const Vector3& v1, const Vector3& v2)
	{
		return glm::cross(v1, v2);
	}

	inline Quaternion LookAtRotation(const Vector3& direction, const Vector3& up)
	{
		return glm::quatLookAt(direction, up);
	}

	inline Matrix4x4 MakeViewMatrix(const Vector3& eye, const Vector3& center, const Vector3& up)
	{
		return glm::lookAt(eye, center, up);
	}

	inline Matrix4x4 MakePerspectiveMatrix(float fov, float aspect, float znear, float zfar)
	{
		return glm::perspective(fov, aspect, znear, zfar);
	}

	inline Matrix4x4 MakeReversedPerspectiveMatrix(float fov, float aspect, float znear, float zfar)
	{
		MX_ASSERT(std::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
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

	template<typename T, typename U, typename R>
	inline constexpr decltype(std::declval<T>() + std::declval<U>() + std::declval<R>()) Max(const T& v1, const U& v2, const R& v3)
	{
		return Max(Max(v1, v2), v3);
	}

	template<typename T, typename U>
	inline constexpr decltype(std::declval<T>() + std::declval<U>()) Min(const T& v1, const U& v2)
	{
		return (v1 < v2 ? v1 : v2);
	}

	template<typename T, typename U, typename R>
	inline constexpr decltype(std::declval<T>() + std::declval<U>() + std::declval<R>()) Min(const T& v1, const U& v2, const R& v3)
	{
		return Min(Min(v1, v2), v3);
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

	/*!
	computes safe sqrt for any floating point value
	\param x value from which sqrt(x) is computed
	\returns sqrt(x) if x >= 0.0f, -sqrt(-x) if x < 0.0f
	*/
	inline constexpr float SignedSqrt(float x)
	{
		if (x < 0.0f) return -std::sqrt(-x);
		else return std::sqrt(x);
	}

	/*!
	computes angle in radians between two vectors
	\param v1 first  vector (can be not normalized)
	\param v2 second vector (can be not normalized)
	\returns angle between vectors in range [-pi/2; pi/2]
	*/
	template<typename Vector>
	inline float Angle(const Vector& v1, const Vector& v2)
	{
		return std::acos(Dot(v1, v2) / (Length(v1) * Length(v2)));
	}

	inline constexpr float Sqr(float x)
	{
		return x * x;
	}

	/*!
	computes log2 of integer value at compile time
	\param n value from which log2(n) is computed
	\returns log2(n), floored to nearest value, i.e. pow(2, log2(n)) <= n
	*/
	inline constexpr size_t Log2(size_t n)
	{
		return ((n == 1) ? 0 : 1 + Log2(n / 2));
	}

	/*!
	returns nearest power of two which is less or equal to input (1024 -> 1024, 1023 -> 512, 1025 -> 1024)
	\param n value to floor from
	\returns power of two not greater than n
	*/
	inline constexpr size_t FloorToPow2(size_t n)
	{
		return static_cast<size_t>(1) << Log2(n);
	}

	/*!
	returns nearest power of two which is greater or equal to input (1024 -> 1024, 1023 -> 1024, 1025 -> 2048)
	\param n value to ceil from
	\returns power of two not less than n
	*/
	inline constexpr size_t CeilToPow2(size_t n)
	{
		return static_cast<size_t>(1) << Log2(n * 2 - 1);
	}

	/*!
	applies radians->degrees transformation for each element of vector
	\param vec vector of radians values
	\returns vector of degrees values
	*/
	template<typename T>
	inline auto DegreesVec(T vec)
		-> decltype(vec.length(), vec[0], vec)
	{
		T result = vec;
		for (typename T::length_type i = 0; i < vec.length(); i++)
			result[i] = Degrees(vec[i]);
		return result;
	}

	/*!
	applies degrees->radians transformation for each element of vector
	\param vec vector of degrees values
	\returns vector of radians values
	*/
	template<typename T>
	inline auto RadiansVec(T vec)
		-> decltype(vec.length(), vec[0], vec)
	{
		T result = vec;
		for (typename T::length_type i = 0; i < vec.length(); i++)
			result[i] = Radians(vec[i]);
		return result;
	}

	/*!
	computes max components of two vectors
	\param v1 first  vector
	\param v2 second vector
	\returns vector of max components from v1 and v2
	*/
	template<typename T>
	inline T VectorMax(const T& v1, const T& v2)
	{
		T result(0.0f);
		for (typename T::length_type i = 0; i < result.length(); i++)
		{
			result[i] = std::max(v1[i], v2[i]);
		}
		return result;
	}

	/*!
	computes min components of two vectors
	\param v1 first  vector
	\param v2 second vector
	\returns vector of min components from v1 and v2
	*/
	template<typename T>
	inline T VectorMin(const T& v1, const T& v2)
	{
		T result(0.0f);
		for (typename T::length_type i = 0; i < result.length(); i++)
		{
			result[i] = std::min(v1[i], v2[i]);
		}
		return result;
	}

	/*!
	computes pair of vectors with min and max coords inside verteces array
	\param verteces pointer to an array of Vector3
	\param size number of verteces to compute
	\returns (min components, max components) vector pair
	*/
	inline std::pair<Vector3, Vector3>MinMaxComponents(Vector3* verteces, size_t size)
	{
		Vector3 maxCoords(-1.0f * std::numeric_limits<float>::max());
		Vector3 minCoords(std::numeric_limits<float>::max());
		for (size_t i = 0; i < size; i++)
		{
			minCoords = VectorMin(minCoords, verteces[i]);
			maxCoords = VectorMax(maxCoords, verteces[i]);
		}
		return { minCoords, maxCoords };
	}

	/*!
	compute (Tangent, Bitangent) vector pair using vertex positions and uv-coords
	\param v1 first  vertex position
	\param v2 second vertex position
	\param v3 third  vertex position
	\param t1 first  uv-coords
	\param t2 second uv-coords
	\param t3 third  uv-coords
	\returns (Tangent, Bitangent) pair in a form of array with size = 2
	*/
	inline constexpr std::array<Vector3, 2> ComputeTangentSpace(
		const Vector3& v1, const Vector3& v2, const Vector3& v3,
		const Vector2& t1, const Vector2& t2, const Vector2& t3
	)
	{
		// Edges of the triangle : postion delta
		auto deltaPos1 = v2 - v1;
		auto deltaPos2 = v3 - v1;

		// texture delta
		auto deltaT1 = t2 - t1;
		auto deltaT2 = t3 - t1;

		float r = 1.0f / (deltaT1.x * deltaT2.y - deltaT1.y * deltaT2.x);
		auto tangent = (deltaPos1 * deltaT2.y - deltaPos2 * deltaT1.y) * r;
		auto bitangent = (deltaPos2 * deltaT1.x - deltaPos1 * deltaT2.x) * r;

		return { tangent, bitangent };
	}

	/*!
	compute normal vector pair using triangle vertecies
	\param v1 first  vertex position of vertecies
	\param v2 second vertex position of vertecies
	\param v3 third  vertex position of vertecies
	\returns normalized normal vector
	*/
	inline Vector3 ComputeNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
	{
		auto deltaPos1 = v2 - v1;
		auto deltaPos2 = v3 - v1;

		return Normalize(Cross(deltaPos1, deltaPos2));
	}

	/*!
	creates rotation matrix from rottion angles applied as one-by-one
	\param xRot first  rotation applied around x-axis
	\param yRot second rotation applied around y-axis
	\param zRot third  rotation applied around z-axis
	\returns rotation matrix 3x3
	*/
	inline Matrix3x3 RotateAngles(float xRot, float yRot, float zRot)
	{
		Matrix3x3 ret;
		using std::sin;
		using std::cos;

		float s0 = sin(xRot), c0 = cos(xRot);
		float s1 = sin(yRot), c1 = cos(yRot);
		float s2 = sin(zRot), c2 = cos(zRot);
		constexpr int i = 0;
		constexpr int j = 1;
		constexpr int k = 2;

		ret[i][i] = c1 * c2;
		ret[k][k] = c0 * c1;

		if ((2 + i - j) % 3)
		{
			ret[j][i] = -c1 * s2;
			ret[k][i] = s1;

			ret[i][j] = c0 * s2 + s0 * s1 * c2;
			ret[j][j] = c0 * c2 - s0 * s1 * s2;
			ret[k][j] = -s0 * c1;

			ret[i][k] = s0 * s2 - c0 * s1 * c2;
			ret[j][k] = s0 * c2 + c0 * s1 * s2;
		}
		else
		{
			ret[j][i] = c1 * s2;
			ret[k][i] = -s1;

			ret[i][j] = -c0 * s2 + s0 * s1 * c2;
			ret[j][j] = c0 * c2 + s0 * s1 * s2;
			ret[k][j] = s0 * c1;

			ret[i][k] = s0 * s2 + c0 * s1 * c2;
			ret[j][k] = -s0 * c2 + c0 * s1 * s2;
		}
		return ret;
	}
}