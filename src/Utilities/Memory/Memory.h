#pragma once

#include <memory>

namespace MxEngine
{
	template<typename T>
	using UniqueRef = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	inline UniqueRef<T> MakeUnique(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	inline Ref<T> MakeRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

    template<typename T, typename... Args>
    inline T* Alloc(Args&&... args) { return new T(std::forward<Args>(args)...); }

    template<typename T>
    inline void Free(T* value) { delete value; }
}