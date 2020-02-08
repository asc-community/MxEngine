#pragma once

#include <string>

namespace MxEngine
{
	using TimeStep = float;

	std::string BeautifyTime(TimeStep time);

	struct Time
	{
		static TimeStep Current();
	};
}