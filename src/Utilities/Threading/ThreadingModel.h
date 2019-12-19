#pragma once

namespace MomoEngine
{
	template<typename T>
	class SingleThreaded
	{
	public:
		using VolatileType = T;
		struct Lock {};
	};
}