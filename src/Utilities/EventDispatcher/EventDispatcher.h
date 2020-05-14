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

#include <memory>
#include <functional>
#include <unordered_set>
#include <typeindex>
#include <map>
#include <algorithm>

#include "Utilities/Profiler/Profiler.h"
#include "Utilities/String/String.h"
#include "Utilities/Memory/Memory.h"

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
		using NamedCallback = std::pair<std::string, CallbackBaseFunction>;
		using CallbackList = std::vector<NamedCallback>;
		using EventList = std::vector<UniqueRef<EventBase>>;
		using EventTypeIndex = uint32_t;

		/*!
		list of sheduled all events (executed once per frame by Application class)
		*/
		EventList events;
		/*!
		maps event id to list of event listeners of that id 
		*/
		std::unordered_map<EventTypeIndex, CallbackList> callbacks;
		/*!
		shedules listeners which will be added next frame. 
		This cache exists because sometimes user wants to add new listener inside other listener callback, which may result in crash.
		*/
		std::unordered_map<EventTypeIndex, CallbackList> toAddCache;
		/*!
		shedules listeners which will be removed next frame. 
		This cache exists because sometimes user wants to remove event listener inside other listener callback (or even perform self-removal), which may result in crash.
		*/
		std::vector<std::string> toRemoveCache;

		/*!
		immediately invokes all listeners of event, if any exists
		\param event event to dispatch
		*/
		inline void ProcessEvent(EventBase& event)
		{
			auto& eventCallbacks = this->callbacks[event.GetEventType()];
			for (const auto& [name, callback] : eventCallbacks)
			{
				callback(event);
			}
		}

		/*!
		adds new listener callback to cache queue
		\param name callback name (used for removal)
		\param func callback functor
		*/
		template<typename EventType>
		inline void AddCallbackImpl(std::string name, CallbackBaseFunction&& func)
		{
			this->toAddCache[EventType::eventType].emplace_back(std::move(name), std::move(func));
		}

		/*!
		immediately removes all listeners with same name from list
		\param callbacks list of listeners callbacks
		\param name name of listeners to search for
		*/
		inline void RemoveEventByName(CallbackList& callbacks, const std::string& name)
		{
			auto it = std::remove_if(callbacks.begin(), callbacks.end(), [&name](const auto& p)
			{
				return p.first == name;
			});
			callbacks.resize(it - callbacks.begin());
		}

		/*!
		performs cache update, removing events from toRemoveCache list and adding events from toAddCache list
		*/
		inline void RemoveQueuedEvents()
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

	public:
		/*!
		adds new event listener to dispatcher (listener placed in waiting queue until next frame).
		Note that multiple listeners may have same name. If so, deleting by name will result in removing all of them
		\param name name of listener (used for deleting listener)
		\param func listener callback functor
		*/
		template<typename EventType>
		void AddEventListener(const std::string& name, std::function<void(EventType&)> func)
		{
			this->AddCallbackImpl<EventType>(name, [func = std::move(func)](EventBase& e)
			{
				if (e.GetEventType() == EventType::eventType)
					func((EventType&)e);
			});
		}

		/*!
		adds new event listener to dispatcher (listener is placed in waiting queue until next frame).
		Note that multiple listeners may have same name. If so, deleting by name will result in removing all of them
		\param name name of listener (used for deleting listener)
		\param func listener callback functor
		*/
		template<typename FunctionType>
		void AddEventListener(const std::string& name, FunctionType&& func)
		{
			this->AddEventListener(name, std::function(std::forward<FunctionType>(func)));
		}

		/*!
		removes all event listeners by their names (action is placed in waiting queue until next frame)
		\param name name of listeners to be deleted
		*/
		void RemoveEventListener(const std::string& name)
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
			this->RemoveQueuedEvents();
			auto& eventCallbacks = this->callbacks[event.GetEventType()];
			for (const auto& [name, callback] : eventCallbacks)
			{
				callback(event);
			}
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
		Invokes all shedules events in the order they were added. Note that invokation also forces queues to be invalidated
		*/
		void InvokeAll()
		{
			this->RemoveQueuedEvents();

			for (size_t i = 0; i < this->events.size(); i++)
			{
				MAKE_SCOPE_PROFILER(typeid(*this->events[i]).name());
				this->ProcessEvent(*this->events[i]);
			}
			this->events.clear();
		}
	};
}