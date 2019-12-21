#pragma once

#include <iostream>
#include <string>

#include "Utilities/SingletonHolder/SingletonHolder.h"

namespace MomoEngine
{
	class LoggerImpl
	{
		std::ostream* error   = &std::cerr;
		std::ostream* debug   = &std::cerr;
		std::ostream* warning = &std::cerr;
	public:
		LoggerImpl() = default;
		LoggerImpl(const LoggerImpl&) = delete;
		LoggerImpl(LoggerImpl&&) = delete;

		void Error  (const std::string& invoker, const std::string& message) const;
		void Debug  (const std::string& invoker, const std::string& message) const;
		void Warning(const std::string& invoker, const std::string& message) const;
	};

	using Logger = SingletonHolder<LoggerImpl>;
}