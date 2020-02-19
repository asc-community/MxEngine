// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in sourceand binary forms, with or without
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