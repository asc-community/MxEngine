#pragma once

#include <iostream>
#include <string>

#include "Utilities/SingletonHolder/SingletonHolder.h"

namespace MomoEngine
{
	class LoggerImpl
	{
		std::ostream* error   = &std::cout;
		std::ostream* debug   = &std::cout;
		std::ostream* warning = &std::cout;
		bool useDebug = true;
		bool useWarning = true;
		bool useError = true;
	public:
		LoggerImpl() = default;
		LoggerImpl(const LoggerImpl&) = delete;
		LoggerImpl(LoggerImpl&&) = delete;

		void Error  (std::string_view invoker, std::string_view message) const;
		void Debug  (std::string_view invoker, std::string_view message) const;
		void Warning(std::string_view invoker, std::string_view message) const;
		LoggerImpl& UseErrorStream(std::ostream* error);
		LoggerImpl& UseWarningStream(std::ostream* warning);
		LoggerImpl& UseDebugStream(std::ostream* debug);
		LoggerImpl& UseDebug(bool value = true);
		LoggerImpl& UseWarning(bool value = true);
		LoggerImpl& UseError(bool value = true);
	};

	using Logger = SingletonHolder<LoggerImpl>;
}