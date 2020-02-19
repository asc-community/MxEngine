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

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <unordered_set>

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
		using CallbackList = std::vector<std::pair<std::string, std::function<void(EventBase&)>>>;
		using EventList = std::vector<std::unique_ptr<EventBase>>;
		using RemoveList = std::unordered_set<std::string>;
		CallbackList callbacks;
		RemoveList removeQueue;
		CallbackList callbackQueue;
		EventList events;

		size_t eventTypeId = 0;

		template<typename EventType>
		inline void RegisterEventType()
		{
			static size_t typeId = ++this->eventTypeId;
			EventType::eventType = typeId;
		}

		inline void FreeQueues()
		{
			if (!removeQueue.empty())
			{
				for (auto it = this->callbacks.begin(); it != this->callbacks.end();)
				{
					if (removeQueue.find(it->first) != this->removeQueue.end())
						it = this->callbacks.erase(it);
					else
						it++;
				}
				removeQueue.clear();
			}
			if (!callbackQueue.empty())
			{
				this->callbacks.insert(
					this->callbacks.end(),
					std::make_move_iterator(callbackQueue.begin()),
					std::make_move_iterator(callbackQueue.end())
				);
				callbackQueue.clear();
			}
		}

		inline void ProcessEvent(EventBase& event)
		{
			for (const auto& callback : this->callbacks)
			{
				callback.second(event);
			}
		}

	public:

		template<typename EventType>
		void AddEventListener(const std::string& name, std::function<void(EventType&)> func)
		{
			RegisterEventType<EventType>();
			this->callbackQueue.emplace_back(name, [func = std::move(func)](EventBase& e)
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
			this->removeQueue.insert(name);
		}

		void Invoke(EventBase& event)
		{
			this->FreeQueues();
			this->ProcessEvent(event);
		}

		void AddEvent(std::unique_ptr<EventBase> event)
		{
			this->events.push_back(std::move(event));
		}

		void InvokeAll()
		{
			FreeQueues();
			for (size_t i = 0; i < this->events.size(); i++)
			{
				this->ProcessEvent(*this->events[i]);
			}
			this->events.clear();
		}
	};
}