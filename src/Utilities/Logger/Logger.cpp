// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "Logger.h"
#ifdef _WIN32
#define BOOST_STACKTRACE_USE_WINDBG_CACHED
#include <Windows.h>
#endif

#include <boost/stacktrace.hpp>

namespace MxEngine
{
	void LoggerImpl::Error(const std::string& invoker, const std::string& message) const
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
			this->StackTrace();
		}
	}

	void LoggerImpl::Debug(const std::string& invoker, const std::string& message) const
	{
		if (debug != nullptr && useDebug)
		{
			*debug << '[' << invoker << " debug]: " << message << std::endl;
		}
	}

	void LoggerImpl::Warning(const std::string& invoker, const std::string& message) const
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

	void LoggerImpl::StackTrace() const
	{
		if (error != nullptr && useStackTrace)
		{
			auto st = boost::stacktrace::stacktrace().as_vector();
			for (size_t i = 0, size = st.size(); i < size; i++)
			{
				auto function = st[i].name();
				auto filename = st[i].source_file();
				if ((function.find("MxEngine::LoggerImpl") == function.npos) &&
					(function.find("boost::stacktrace") == function.npos) &&
					!filename.empty())
				{
					*error << "  at " << function;
					*error << " in " << filename << ':' << st[i].source_line();
					*error << " | " << st[i].address() << '\n';
				}
			}
		}
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

	LoggerImpl& LoggerImpl::UseStackTrace(bool value)
	{
		this->useStackTrace = value;
		return *this;
	}
}