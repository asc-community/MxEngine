#pragma once

namespace MxEngine
{
	template<typename T>
	class SingleThreaded
	{
	public:
		using VolatileType = T;
		struct Lock {};
	};
}