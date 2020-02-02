#include "Time.h"

// only for time
#include <GLFW/glfw3.h>

MomoEngine::TimeStep MomoEngine::Time::Current()
{
    return (MomoEngine::TimeStep)glfwGetTime();
}

std::string MomoEngine::BeautifyTime(TimeStep time)
{
	if (time > 1.0f)
	{
		int timeInt = int(time * 100);
		return std::to_string(timeInt / 100) + "." + std::to_string(timeInt % 100) + "s";
	}
	else
	{
		int timeInt = int(time * 1000 * 100);
		return std::to_string(timeInt / 100) + "." + std::to_string(timeInt % 100) + "ms";
	}
}
