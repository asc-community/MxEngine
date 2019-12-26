#pragma once

#include <string>

namespace MomoEngine
{
	using TimeStep = float;

	std::string BeautifyTime(TimeStep time);

	struct Time
	{
		static TimeStep Current();
	};
}