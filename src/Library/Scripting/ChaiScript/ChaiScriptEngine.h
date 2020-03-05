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

#include "Core/Macro/Macro.h"

#if defined(MXENGINE_USE_CHAISCRIPT)

#define CHAISCRIPT_NO_THREADS
#include <chaiscript/chaiscript.hpp>

namespace MxEngine
{
	class ChaiScriptEngine
	{
		chaiscript::ChaiScript engine;
	public:
		using BoxedValue = chaiscript::Boxed_Value;

		inline chaiscript::ChaiScript& GetInterpreter()
		{
			return engine;
		}

		inline BoxedValue Execute(const std::string& code)
		{
			return engine.eval(code);
		}

		template<typename T, typename U>
		void AddTypeConversion()
		{
			engine.add(chaiscript::type_conversion<T, U>());
		}

		template<typename T>
		void AddReference(const std::string& name, T&& value)
		{
			engine.add(chaiscript::fun(std::forward<T>(value)), name);
		}

		template<typename T, typename U, typename... Args>
		void AddReference(const std::string& name, T* object, U(T::* value)(Args...))
		{
			engine.add(chaiscript::fun(value, object), name);
		}

		template<typename T, typename U, typename... Args>
		void AddReference(const std::string& name, T* object, U(T::* value)(Args...) const)
		{
			engine.add(chaiscript::fun(value, object), name);
		}

		template<typename T, typename U, typename... Args>
		void AddReference(const std::string& name, U(T::* value)(Args...))
		{
			engine.add(chaiscript::fun(value), name);
		}

		template<typename T>
		void AddVariable(const std::string& name, T& value)
		{
			engine.add_global(chaiscript::var(std::ref(value)), name);
		}

		template<typename T>
		void AddType(const std::string& name, bool isAssignable = true)
		{
			using namespace chaiscript::bootstrap::standard_library;

			engine.add(chaiscript::user_type<T>(), name);
			if(isAssignable) engine.add(assignable_type<T>(name));
		}

		template<typename T, typename... Args>
		void AddTypeConstructor(const std::string& name)
		{
			engine.add(chaiscript::constructor<T(Args...)>(), name);
		}
	};
}
#endif