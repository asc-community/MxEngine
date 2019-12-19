#pragma once

#include <cstdlib>

namespace MomoEngine
{
	template<typename T>
	class CreateWithNew
	{
	public:
		static inline T* Create()
		{
			return new T();
		}

		static inline void Destroy(T* obj)
		{
			delete obj;
		}
	};

	template<typename T>
	class CreateWithMalloc
	{
	public:
		static inline T* Create()
		{
			T* ptr = std::malloc(sizeof(T));
			if (ptr == nullptr) 
				return nullptr;
			return new(ptr) T();
		}

		static inline void Destroy(T* obj)
		{
			obj->~T();
			std::free(obj);
		}
	};

	template<typename T>
	class CreateStatic
	{
		union MaxAling { char t[sizeof(T)]; long double ld; long long ll; };
	public:
		static inline T* Create()
		{
			static MaxAling staticMemory;
			return new(&staticMemory) T();
		}

		static inline void Destroy(T* obj)
		{
			obj->~T();
		}
	};
}