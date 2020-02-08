#include "Logger.h"
#ifdef _WIN32
#include <Windows.h>
#endif

namespace MxEngine
{
	void LoggerImpl::Error(std::string_view invoker, std::string_view message) const
	{
		if (error != nullptr && useError)
		{
			#ifdef _WIN32
			auto handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
			::SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
			#endif
			*error << '[' << invoker << " error]: " << message << std::endl;
			#ifdef _WIN32
			::SetConsoleTextAttribute(handle, 7); // default
			#endif
		}
	}

	void LoggerImpl::Debug(std::string_view invoker, std::string_view message) const
	{
		if (debug != nullptr && useDebug)
		{
			*debug << '[' << invoker << " debug]: " << message << std::endl;
		}
	}

	void LoggerImpl::Warning(std::string_view invoker, std::string_view message) const
	{
		#ifdef _WIN32
		auto handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		::SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		#endif
		if (warning != nullptr && useWarning)
		{
			*warning << '[' << invoker << " warning]: " << message << std::endl;
		}
		#ifdef _WIN32
		::SetConsoleTextAttribute(handle, 7); // default
		#endif
	}

	LoggerImpl& LoggerImpl::UseErrorStream(std::ostream* error)
	{
		this->error = error;
		return *this;
	}

	LoggerImpl& LoggerImpl::UseWarningStream(std::ostream* warning)
	{
		this->warning = warning;
		return *this;
	}

	LoggerImpl& LoggerImpl::UseDebugStream(std::ostream* debug)
	{
		this->debug = debug;
		return *this;
	}

	LoggerImpl& LoggerImpl::UseDebug(bool value)
	{
		this->useDebug = value;
		return *this;
	}

	LoggerImpl& LoggerImpl::UseWarning(bool value)
	{
		this->useWarning = value;
		return *this;
	}

	LoggerImpl& LoggerImpl::UseError(bool value)
	{
		this->useError = value;
		return *this;
	}
}