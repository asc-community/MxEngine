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

#include "CreationPolicy.h"
#include "LifetimePolicy.h"
#include "Utilities/Threading/ThreadingModel.h"
#include "Core/Macro/Macro.h"

namespace MxEngine
{
	/*!
	this is Andrei's Alexandrescu SingletonHolder (see "Modern C++ Design" ch. 6)
	it uses policy-based design which allows to configure class behaviour at compile-time
	*/
	template<
		typename T,
		template<typename> class CreationPolicy = CreateWithNew,
		template<typename> class LifetimePolicy = DefaultLifetime,
		template<typename> class ThreadingModel = SingleThreaded
	>
	class SingletonHolder
	{
		using InstanceType = typename ThreadingModel<T>::VolatileType;

		/*!
		singleton instance of class
		*/
		inline static InstanceType* instance = nullptr;
		/*!
		is singleton already destroyed or not
		*/
		inline static bool destroyed = false;

		/*!
		destroys sigleton object depending on policy
		*/
		static inline void DestroySingleton()
		{
			MX_ASSERT(instance != nullptr);
			CreationPolicy<T>::Destroy(instance);
			instance = nullptr;
			destroyed = true;
		}
	public:
		SingletonHolder() = delete;
		SingletonHolder(const SingletonHolder&) = delete;
		SingletonHolder& operator=(const SingletonHolder&) = delete;

		/*!
		singleton inner object getter. Creates instance, if it not exists, handles destroyed reference access
		\returns reference to singleton object instance
		*/
		static inline T& Instance()
		{
			if (instance == nullptr)
			{
				typename ThreadingModel<T>::Lock guard;
				guard;
				if (destroyed)
				{
					LifetimePolicy<T>::OnDeadReference();
					destroyed = false;
				}
				instance = CreationPolicy<T>::Create();
				LifetimePolicy<T>::ScheduleDestruction(instance, &DestroySingleton);
			}
			return *instance;
		}
	};
}