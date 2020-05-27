// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
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

#pragma once

#include <iostream>
#include <string>

#include "Utilities/SingletonHolder/SingletonHolder.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
	/*!
	Logger class is a singleton which is used by whole engine to output warnings, info, debug messages and errors
	it provides output stream forwarding and ignoring, colored output on some platforms and stacktrace for errors
	*/
	class LoggerImpl
	{
		std::ostream* error   = &std::cout;
		std::ostream* debug   = &std::cout;
		std::ostream* warning = &std::cout;
		bool useDebug = true;
		bool useWarning = true;
		bool useError = true;
		bool useStackTrace = true;
	public:
		LoggerImpl() = default;
		LoggerImpl(const LoggerImpl&) = delete;
		LoggerImpl(LoggerImpl&&) = delete;

		/*!
		prints error to an error stream (defaults to std::cout). Error is colored red. By default stacktrace is also printed (may cause performance issues on some platforms)
		\param invoker name of module which invokes the error
		\param message error message
		*/
		void Error  (const MxString& invoker, const MxString& message) const;
		/*!
		prints debug info to an debug stream (defaults to std::cout). Debug message is colored light-grey. Stacktrace is not printed
		\param invoker name of module which invokes the debug message
		\param message debug message
		*/
		void Debug  (const MxString& invoker, const MxString& message) const;
		/*!
		prints warning to an warning stream (defaults to std::cout). Warning is colored yellow. Stacktrace is not printed
		\param invoker name of module which invokes the warning
		\param message warning message
		*/
		void Warning(const MxString& invoker, const MxString& message) const;
		/*!
		prints stacktrace to error stream if stacktracing is enabled (defaults std::cout). 
		Logger function itself, CRT functions or other system functions are ignored and not printed
		*/
		void StackTrace() const;
		/*!
		sets error stream. nullptr as parameter disables error output
		\param error new stream pointer
		*/
		LoggerImpl& UseErrorStream(std::ostream* error);
		/*!
		sets warning stream. nullptr as parameter disables warning output
		\param error new stream pointer
		*/
		LoggerImpl& UseWarningStream(std::ostream* warning);
		/*!
		sets debug stream. nullptr as parameter disables debug output
		\param error new stream pointer
		*/
		LoggerImpl& UseDebugStream(std::ostream* debug);
		/*!
		enables / disables debug stream output
		*/
		LoggerImpl& UseDebug(bool value = true);
		/*!
		enables / disables warning stream output
		*/
		LoggerImpl& UseWarning(bool value = true);
		/*!
		enables / disables error stream output
		*/
		LoggerImpl& UseError(bool value = true);
		/*!
		enables / disables stacktrace output (see StackTrace() function)
		*/
		LoggerImpl& UseStackTrace(bool value = true);
	};

	using Logger = SingletonHolder<LoggerImpl>;

	// macro for fast debug messages insert. Prints warnings in format [MX_DBG Warning]: { your message }
	#define MX_DBG(msg) Logger::Instance().Warning("MX_DBG", msg)
}