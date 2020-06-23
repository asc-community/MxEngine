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

#include <memory>
#include <functional>
#include <unordered_set>
#include <typeindex>
#include <map>
#include <algorithm>

#include "Utilities/Profiler/Profiler.h"
#include "Utilities/Memory/Memory.h"
#include "Utilities/String/String.h" // is used in macro, so analyzer may say it is unused, but its not
#include "Utilities/STL/MxVector.h"

namespace MxEngine
{
	/*
	creates base event class and adds GetEventType() pure virtual method. Used for EventDispatcher class
	*/
	#define MAKE_EVENT_BASE(name) struct name { inline virtual uint32_t GetEventType() const = 0; virtual ~name() = default; }

	/*
	inserted into class body of derived classes from base event. Using compile-time hash from class name to generate type id
	*/
	#define MAKE_EVENT(class_name) \
	template<typename T> friend class EventDispatcher;\
	public: inline virtual uint32_t GetEventType() const override { return eventType; } private:\
	constexpr static uint32_t eventType = STRING_ID(#class_name)

	/*!
	EventDispatcher class is used to handle all events inside MxEngine. Events can either be dispatch for Application (global) or
	for currently active scene. Note that events are NOT dispatched when developer console is opened and instead sheduled until it close
	*/
	template<typename EventBase>
	class EventDispatcher
	{
		using CallbackBaseFunction = std::function<void(EventBase&)>;
		using NamedCallback = std::pair<MxString, CallbackBaseFunction>;
		using CallbackList = MxVector<NamedCallback>;
		using EventList = MxVector<UniqueRef<EventBase>>;
		using EventTypeIndex = uint32_t;

		/*!
		list of scheduled all events (executed once per frame by Application class)
		*/
		EventList events;
		/*!
		maps event id to list of event listeners of that id 
		*/
		std::unordered_map<EventTypeIndex, CallbackList> callbacks;
		/*!
		schedules listeners which will be added next frame. 
		This cache exists because sometimes user wants to add new listener inside other listener callback, which may result in crash.
		*/
		std::unordered_map<EventTypeIndex, CallbackList> toAddCache;
		/*!
		shedules listeners which will be removed next frame. 
		This cache exists because sometimes user wants to remove event listener inside other listener callback (or even perform self-removal), which may result in crash.
		*/
		MxVector<MxString> toRemoveCache;

		/*!
		immediately invokes all listeners of event, if any exists
		\param event event to dispatch
		*/
		inline void ProcessEvent(EventBase& event)
		{
			auto& eventCallbacks = this->callbacks[event.GetEventType()];
			for (const auto& [name, callback] : eventCallbacks)
			{
				if (std::find(this->toRemoveCache.begin(), this->toRemoveCache.end(), name) == this->toRemoveCache.end())
				{
					callback(event);
				}
			}
		}

		/*!
		adds new listener callback to cache queue
		\param name callback name (used for removal)
		\param func callback functor
		*/
		template<typename EventType>
		inline void AddCallbackImpl(MxString name, CallbackBaseFunction&& func)
		{
			this->toAddCache[EventType::eventType].emplace_back(std::move(name), std::move(func));
		}

		/*!
		immediately removes all listeners with same name from list
		\param callbacks list of listeners callbacks
		\param name name of listeners to search for
		*/
		inline void RemoveEventByName(CallbackList& callbacks, const MxString& name)
		{
			auto it = std::remove_if(callbacks.begin(), callbacks.end(), [&name](const auto& p)
			{
				return p.first == name;
			});
			callbacks.resize(it - callbacks.begin());
		}
	public:
		/*!
		performs cache update, removing events from toRemoveCache list and adding events from toAddCache list
		*/
		inline void FlushEvents()
		{
			for (auto it = this->toRemoveCache.begin(); it != this->toRemoveCache.end(); it++)
			{
				for (auto& [event, callbacks] : this->callbacks)
				{
					RemoveEventByName(callbacks, *it);
				}
			}
			this->toRemoveCache.clear();

			for (auto it = this->toAddCache.begin(); it != this->toAddCache.end(); it++)
			{
				auto& [event, funcs] = *it;
				for (auto& func : funcs)
				{
					callbacks[event].push_back(std::move(func));
				}
			}
			for (auto& list : this->toAddCache) list.second.clear();
		}

		/*!
		adds new event listener to dispatcher (listener placed in waiting queue until next frame).
		Note that multiple listeners may have same name. If so, deleting by name will result in removing all of them
		\param name name of listener (used for deleting listener)
		\param func listener callback functor
		*/
		template<typename EventType>
		void AddEventListener(const MxString& name, std::function<void(EventType&)> func)
		{
			this->template AddCallbackImpl<EventType>(name, [func = std::move(func)](EventBase& e)
			{
				if (e.GetEventType() == EventType::eventType)
					func(static_cast<EventType&>(e));
			});
		}

		/*!
		adds new event listener to dispatcher (listener is placed in waiting queue until next frame).
		Note that multiple listeners may have same name. If so, deleting by name will result in removing all of them
		\param name name of listener (used for deleting listener)
		\param func listener callback functor
		*/
		template<typename FunctionType>
		void AddEventListener(const MxString& name, FunctionType&& func)
		{
			this->AddEventListener(name, std::function(std::forward<FunctionType>(func)));
		}

		/*!
		removes all event listeners by their names (action is placed in waiting queue until next frame)
		\param name name of listeners to be deleted
		*/
		void RemoveEventListener(const MxString& name)
		{
			this->toRemoveCache.push_back(name);
		}
		
		/*!
		Immediately invokes event of specific type. Note that invokation also forces queues to be invalidated
		\param event event to dispatch
		*/
		template<typename Event>
		void Invoke(Event& event)
		{
			this->FlushEvents();
			this->ProcessEvent(event);
		}

		/*!
		Adds event to event queue. All such events will be dispatched in next frames in the order they were added
		\param event event to shedule dispatch
		*/
		void AddEvent(UniqueRef<EventBase> event)
		{
			this->events.push_back(std::move(event));
		}

		/*!
		Invokes all shedules events in the order they were added. Note that invoke also forces queues to be invalidated
		*/
		void InvokeAll()
		{
			this->FlushEvents();

			for (size_t i = 0; i < this->events.size(); i++)
			{
				MAKE_SCOPE_PROFILER(typeid(*this->events[i]).name());
				this->ProcessEvent(*this->events[i]);
			}
			this->events.clear();
		}
	};
}