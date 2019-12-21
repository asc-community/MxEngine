#pragma once

#include <exception>

namespace MomoEngine
{
	using AtExitFunctionPointer = void (*)();

	template <class T>
	class DefaultLifetime
	{
	public:
		static inline void ScheduleDestruction(T*, AtExitFunctionPointer func)
		{
			std::atexit(func);
		}

		static inline void OnDeadReference()
		{
			throw std::exception("[singleton error]: dead reference detected");
		}
	};

	template <class T>
	class NoDestroy
	{
	public:
		static inline void ScheduleDestruction(T*, AtExitFunctionPointer) { }

		static inline void OnDeadReference() { }
	};

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
		static bool destroyedOnce;
	};
	template<typename T> bool PhoenixSingleton<T>::destroyedOnce = false;
}