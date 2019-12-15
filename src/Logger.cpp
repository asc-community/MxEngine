#include "Logger.h"

void Logger::Error(const std::string& invoker, const std::string& message) const
{
	*error << '[' << invoker << " error]: " << message << std::endl;
}

Logger& Logger::Get()
{
	static Logger logger;
	return logger;
}

void Logger::Debug(const std::string& invoker, const std::string& message) const
{
	*debug << '[' << invoker << " debug]: " << message << std::endl;
}
