#pragma once

#include <iostream>
#include <string>

namespace MomoEngine
{
	class Logger
	{
		std::ostream* error   = &std::cerr;
		std::ostream* debug   = &std::cerr;
		std::ostream* warning = &std::cerr;
	public:
		void Error  (const std::string& invoker, const std::string& message) const;
		void Debug  (const std::string& invoker, const std::string& message) const;
		void Warning(const std::string& invoker, const std::string& message) const;
		static Logger& Get();
	};
}