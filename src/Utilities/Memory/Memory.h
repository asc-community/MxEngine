#pragma once

#include <memory>

namespace MomoEngine
{
	template<typename T>
	using UniqueRef = std::unique_ptr<T>;

	template<typename T, typename... Args>
	UniqueRef<T> MakeUnique(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
}