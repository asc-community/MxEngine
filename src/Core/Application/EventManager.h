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

#include "Core/Application/Application.h"

namespace MxEngine
{
    class EventManager
    {
    public:
		/*!
		adds new event listener to dispatcher (listener placed in waiting queue until next frame).
		Note that multiple listeners may have same name. If so, deleting by name will result in removing all of them
		\param name name of listener (used for deleting listener)
		\param func listener callback functor
		*/
		template<typename EventType>
		static void AddEventListener(const MxString& name, std::function<void(EventType&)> func)
		{
			Application::Get()->GetEventDispatcher().AddEventListener(name, std::move(func));
		}

		/*!
		adds new event listener to dispatcher (listener is placed in waiting queue until next frame).
		Note that multiple listeners may have same name. If so, deleting by name will result in removing all of them
		\param name name of listener (used for deleting listener)
		\param func listener callback functor
		*/
		template<typename FunctionType>
		static void AddEventListener(const MxString& name, FunctionType&& func)
		{
			Application::Get()->GetEventDispatcher().AddEventListener(name, std::forward<FunctionType>(func));
		}

		/*!
		removes all event listeners by their names (action is placed in waiting queue until next frame)
		\param name name of listeners to be deleted
		*/
		static void RemoveEventListener(const MxString& name)
		{
			Application::Get()->GetEventDispatcher().RemoveEventListener(name);
		}

		/*!
		Immediately invokes event of specific type. Note that invokation also forces queues to be invalidated
		\param event event to dispatch
		*/
		template<typename Event>
		static void Invoke(Event& event)
		{
			Application::Get()->GetEventDispatcher().Invoke(event);
		}

		/*!
		Adds event to event queue. All such events will be dispatched in next frames in the order they were added
		\param event event to shedule dispatch
		*/
		static void AddEvent(UniqueRef<IEvent> event)
		{
			Application::Get()->GetEventDispatcher().AddEvent(std::move(event));
		}

		/*!
		Invokes all shedules events in the order they were added. Note that invoke also forces queues to be invalidated
		*/
		static void InvokeAll()
		{
			Application::Get()->GetEventDispatcher().InvokeAll();
		}
    };
}