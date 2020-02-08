#pragma once

#include "CreationPolicy.h"
#include "LifetimePolicy.h"
#include "Utilities/Threading/ThreadingModel.h"
#include <cassert>

// Andrei's Alexandrescu SingletonHolder (see "Modern C++ Design" ch. 6)

namespace MxEngine
{
	template<
		typename T,
		template<typename> class CreationPolicy = CreateWithNew,
		template<typename> class LifetimePolicy = DefaultLifetime,
		template<typename> class ThreadingModel = SingleThreaded
	>
	class SingletonHolder
	{
		using InstanceType = typename ThreadingModel<T>::VolatileType;

		static InstanceType* instance;
		static bool destroyed;

		static inline void DestroySingleton()
		{
			assert(instance != nullptr);
			CreationPolicy<T>::Destroy(instance);
			instance = nullptr;
			destroyed = true;
		}
	public:
		SingletonHolder() = delete;
		SingletonHolder(const SingletonHolder&) = delete;
		SingletonHolder& operator=(const SingletonHolder&) = delete;

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

	#define SINGLETON_TEMPLATE(T, C, L, M) template<typename T, template<typename> typename C, template<typename> typename L, template<typename> typename M>

	SINGLETON_TEMPLATE(T, C, L, M)
	bool SingletonHolder<T, C, L, M>::destroyed = false;

	SINGLETON_TEMPLATE(T, C, L, M)
	typename SingletonHolder<T, C, L, M>::InstanceType* SingletonHolder<T, C, L, M>::instance = nullptr;

	#undef SINGLETON_TEMPLATE
}