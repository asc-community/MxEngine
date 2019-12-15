#pragma once

#include <iostream>
#include <string>

class Logger
{
	std::ostream* error = &std::cerr;
	std::ostream* debug = &std::cerr;
public:
	void Error(const std::string& invoker, const std::string& message) const;
	void Debug(const std::string& invoker, const std::string& message) const;
	static Logger& Get();
};