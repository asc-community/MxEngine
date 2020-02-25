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

#pragma once

#include <iostream>
#include <string>

#include "Utilities/SingletonHolder/SingletonHolder.h"

namespace MxEngine
{
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

		void Error  (const std::string& invoker, const std::string& message) const;
		void Debug  (const std::string& invoker, const std::string& message) const;
		void Warning(const std::string& invoker, const std::string& message) const;
		void StackTrace() const;
		LoggerImpl& UseErrorStream(std::ostream* error);
		LoggerImpl& UseWarningStream(std::ostream* warning);
		LoggerImpl& UseDebugStream(std::ostream* debug);
		LoggerImpl& UseDebug(bool value = true);
		LoggerImpl& UseWarning(bool value = true);
		LoggerImpl& UseError(bool value = true);
		LoggerImpl& UseStackTrace(bool value = true);
	};

	using Logger = SingletonHolder<LoggerImpl>;
}