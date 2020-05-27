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

#include "Utilities/Time/Time.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/SingletonHolder/SingletonHolder.h"

#include <fstream>
#include <string>

namespace MxEngine
{
	/*!
	profile session is a special singleton object which periodically writes log to a json file
	it is used inside ScopeProfiler to write time measurements. After application exit log can be viewed at chrome://tracing page
	*/
	class ProfileSession
	{
		/*!
		json file to which profile log is outputted
		*/
		std::ofstream stream;
		/*!
		count of json log entries (is used internally to create json file)
		*/
		size_t entriesCount = 0;

		/*!
		writes header of json file, i.e "{ traceEvents: [ ..."
		*/
		void WriteJsonHeader();
		/*!
		writes footer of json file, i.e "] }"
		*/
		void WriteJsonFooter();
	public:
		/*!
		checks if json file is opened
		\returns true if json file can be written to, false either
		*/
		bool IsValid() const;
		/*!
		getter for entriesCount
		\returns number of json entries
		*/
		size_t GetEntryCount() const;
		/*!
		creates json file or clears it if it exists and writes json header to it
		\param filename file to output json to
		*/
		void StartSession(const MxString& filename);
		/*!
		writes json entry, consisting of process id, start/end time, function name
		\param function called function name 
		\param begin start timepoint of function execution
		\param delta duration of function execution
		*/
		void WriteJsonEntry(const char* function, TimeStep begin, TimeStep delta);
		/*!
		ends profile measurement, writing json footer and saving json file to disk
		*/
		void EndSession();
	};

	using Profiler = SingletonHolder<ProfileSession>;

	/*!
	scope profiler is a class which measures how much time take the function execution 
	it saves timestep om its creation, and writes json entry to ProfileSession on its destruction
	*/
	class ScopeProfiler
	{
		/*!
		construction time point and start of function execution
		*/
		TimeStep start;
		/*!
		function name which is measured
		*/
		const char* function;
		/*!
		reference to json profile logger
		*/
		ProfileSession& profiler;
	public:
		/*!
		creates scope profiler and fixes timepoint as start of function call
		\param profiler reference to json profile logger
		\param function function name which is measured
		*/
		inline ScopeProfiler(ProfileSession& profiler, const char* function)
			: start(Time::Current()), function(function), profiler(profiler) { }

		/*!
		destroyed scope profiler, forcing it to write json entry to profiler
		*/
		inline ~ScopeProfiler()
		{
			TimeStep end = Time::Current();
			this->profiler.WriteJsonEntry(this->function, this->start, end - start);
		}
	};

	/*!
	scope timer is a class which logs how much time the function execution took
	logging is done using Logger class via debug stream
	*/
	class ScopeTimer
	{
		/*!
		time point when function execution started
		*/
		TimeStep start;
		/*!
		function name. Is string, because when calling Logger::Debug it is appended to "calling" string
		*/
		MxString function;
		/*!
		invoker of function. Can be class, object or namespace which called the function
		*/
		std::string_view invoker;
	public:
		/*!
		creates scope timer, writing to debug log stream that the function call is started to execute
		\param invoker object/class/namespace which called the function
		\param function function name which is executed
		*/
		inline ScopeTimer(std::string_view invoker, std::string_view function)
			: start(Time::Current()), invoker(invoker), function(function.data())
		{
			Logger::Instance().Debug(this->invoker.data(), "calling " + this->function);
		}

		/*!
		destroys scope timer, writing to debug log stream that the function execution is ended
		*/
		inline ~ScopeTimer()
		{
			TimeStep end = Time::Current();
			MxString delta = BeautifyTime(end - start);
			Logger::Instance().Debug(this->invoker.data(), this->function + " finished in " + delta);
		}
	};

// wrapper aroung ScopeProfiler
#define MAKE_SCOPE_PROFILER(function) ScopeProfiler MXENGINE_CONCAT(_profiler, __LINE__)(Profiler::Instance(), function)
// wrapper around ScopeTimer
#define MAKE_SCOPE_TIMER(invoker, function) ScopeTimer MXENGINE_CONCAT(_timer, __LINE__)(invoker, function)
}