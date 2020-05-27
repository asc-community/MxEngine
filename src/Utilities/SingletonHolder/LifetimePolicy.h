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

// Andrei's Alexandrescu SingletonHolder (see "Modern C++ Design" ch. 6)

namespace MxEngine
{
	using AtExitFunctionPointer = void (*)();

	/*!
	lifetime policy of singleton which shedules destruction function call at program exit
	*/
	template <class T>
	class DefaultLifetime
	{
	public:
		static inline void ScheduleDestruction(T*, AtExitFunctionPointer func)
		{
			std::atexit(func);
		}

		static inline void OnDeadReference() { }
	};

	/*!
	lifetime policy of singleton which does not destroy an object
	*/
	template <class T>
	class NoDestroy
	{
	public:
		static inline void ScheduleDestruction(T*, AtExitFunctionPointer) { }

		static inline void OnDeadReference() { }
	};

	/*!
	lifetime policy of singleton which shedules object destruction, but allow it be recreated many times
	*/
	template <class T>
	class PhoenixSingleton
	{
	public:
		static inline void ScheduleDestruction(T*, AtExitFunctionPointer func)
		{
			if (!destroyedOnce)
				std::atexit(func);
		}

		static inline void OnDeadReference()
		{
			destroyedOnce = true;
		}

	private:
		inline static bool destroyedOnce = false;
	};
}