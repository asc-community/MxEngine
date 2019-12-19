#include "Logger.h"

namespace MomoEngine
{
	void LoggerImpl::Error(const std::string& invoker, const std::string& message) const
	{
		*error << '[' << invoker << " error]: " << message << std::endl;
	}

	void LoggerImpl::Debug(const std::string& invoker, const std::string& message) const
	{
		#ifdef _DEBUG
		*debug << '[' << invoker << " debug]: " << message << std::endl;
		#endif
	}

	void LoggerImpl::Warning(const std::string& invoker, const std::string& message) const
	{
		*warning << '[' << invoker << " warning]: " << message << std::endl;
	}
}