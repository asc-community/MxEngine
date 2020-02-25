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

#include "Utilities/LifetimeManager/LifetimeManager.h"

#include <memory>
#include <functional>
#include <unordered_set>
#include <typeindex>
#include <map>

namespace MxEngine
{
	#define MAKE_EVENT_BASE(name) struct name { inline virtual size_t GetEventType() const = 0; virtual ~name() = default; }

	#define MAKE_EVENT \
	template<typename T> friend class EventDispatcher;\
	public: inline virtual size_t GetEventType() const override { return this->eventType; } private:\
	inline static size_t eventType

	template<typename EventBase>
	class EventDispatcher
	{
		using EventCallback = std::function<void(EventBase&)>;
		using EventStorage = std::vector<std::pair<std::string, EventCallback>>;
		using CallbackManager = LifetimeManager<EventCallback, std::string, EventStorage>;
		using EventList = std::vector<std::unique_ptr<EventBase>>;
		using EventTypeMap = std::map<std::type_index, size_t>;

		EventList events;
		CallbackManager callbacks;
		EventTypeMap typeMap;

		inline void ProcessEvent(EventBase& event)
		{
			for (const auto& callback : this->callbacks.Get())
			{
				callback.second(event);
			}
		}

	public:
		template<typename EventType>
		inline void RegisterEventType()
		{
			if (EventType::eventType == 0)
			{
				auto& typeId = typeid(EventType);
				if (this->typeMap.find(typeId) == this->typeMap.end())
				{
					this->typeMap.emplace(typeId, typeMap.size() + 1);
				}
				EventType::eventType = this->typeMap[typeId];
			}
		}

		template<typename EventType>
		void AddEventListener(const std::string& name, std::function<void(EventType&)> func)
		{
			this->RegisterEventType<EventType>();
			this->callbacks.Add(name, [func = std::move(func)](EventBase& e)
			{
				if (e.GetEventType() == EventType::eventType)
					func((EventType&)e);
			});
		}

		template<typename EventType, typename FunctionType>
		void AddEventListener(const std::string& name, FunctionType&& func)
		{
			this->AddEventListener(name, std::function<void(EventType&)>(std::forward<FunctionType>(func)));
		}

		void RemoveEventListener(const std::string& name)
		{
			this->callbacks.Remove(name);
		}

		void Invoke(EventBase& event)
		{
			this->callbacks.Update();
			this->ProcessEvent(event);
		}

		void AddEvent(std::unique_ptr<EventBase> event)
		{
			this->events.push_back(std::move(event));
		}

		void InvokeAll()
		{
			this->callbacks.Update();
			for (size_t i = 0; i < this->events.size(); i++)
			{
				this->ProcessEvent(*this->events[i]);
			}
			this->events.clear();
		}
	};
}