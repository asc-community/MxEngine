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

#include <RuntimeObjectSystem/ObjectInterfacePerModule.h>
#include <RuntimeObjectSystem/RuntimeProtector.h>
#include <RuntimeObjectSystem/IObject.h>

#include "Core/Application/GlobalContextSerializer.h"

#if defined(MXENGINE_WINDOWS)
#define MXENGINE_RUNTIME_LINKLIBRARY(library) RUNTIME_COMPILER_LINKLIBRARY(library)
#elif defined(MXENGINE_LINUX)
#define MXENGINE_RUNTIME_LINKLIBRARY(library) RUNTIME_COMPILER_LINKLIBRARY("-l" library)
#elif defined(MXENGINE_MACOS)
#define MXENGINE_RUNTIME_LINKLIBRARY(library) RUNTIME_COMPILER_LINKLIBRARY("-framework " library)
#endif

namespace MxEngine
{
	namespace
	{
		auto suppressUnusedFunction = GetTrackingInfoFunc<0>(0);
	}

	struct SciptableInterface : public IObject, public RuntimeProtector
	{
		enum ScriptableID : InterfaceID
		{
			ID = IID_ENDInterfaceID,
		};

		struct
		{
			ScriptableMethod Method = ScriptableMethod::ON_CREATE;
			MxObject* Self = nullptr;
		} CurrentState;

		virtual void InitializeModuleContext(void* context) { GlobalContextSerializer::Deserialize(context); }

		virtual void ProtectedFunc()
		{
			switch (this->CurrentState.Method)
			{
			case ScriptableMethod::ON_CREATE:
				this->OnCreate(*this->CurrentState.Self);
				break;
			case ScriptableMethod::ON_RELOAD:
				this->OnReload(*this->CurrentState.Self);
				break;
			case ScriptableMethod::ON_UPDATE:
				this->OnUpdate(*this->CurrentState.Self);
				break;
			default:
				break;
			}
		}

		// overriten in derived classes
		virtual void OnCreate(MxObject& self) { }
		virtual void OnReload(MxObject& self) { }
		virtual void OnUpdate(MxObject& self) { }
	};

	class Scriptable : public TInterface<SciptableInterface::ID, SciptableInterface> { };

	#define MXENGINE_RUNTIME_EDITOR(script) REGISTERCLASS(script) \
		static_assert(MxEngine::AssertEquality<sizeof(script), sizeof(MxEngine::Scriptable)>::value, "scripts must not contains non-static fields")
}