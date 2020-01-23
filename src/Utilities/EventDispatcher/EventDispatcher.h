#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace MomoEngine
{
	#define MAKE_EVENT_BASE(name) struct name { inline virtual size_t GetEventType() const = 0; virtual ~name() = default; }

	#define MAKE_EVENT \
	template<typename T> friend class EventDispatcher;\
	public: inline virtual size_t GetEventType() const override { return this->eventType; } private:\
	inline static size_t eventType

	template<typename EventBase>
	class EventDispatcher
	{
		using CallbackList = std::unordered_multimap<std::string, std::function<void(EventBase&)>>;
		using EventList = std::vector<std::unique_ptr<EventBase>>;
		using CallbackQueue = std::vector<std::pair<std::string, std::function<void(EventBase&)>>>;
		CallbackList callbacks;
		CallbackQueue callbackQueue;
		EventList events;

		size_t eventTypeId = 0;

		template<typename EventType>
		inline void RegisterEventType()
		{
			static size_t typeId = ++this->eventTypeId;
			EventType::eventType = typeId;
		}

		inline void FreeCallbackQueue()
		{
			if (!this->callbackQueue.empty())
			{
				for (auto& callback : this->callbackQueue)
				{
					this->callbacks.emplace(std::move(callback.first), std::move(callback.second));
				}
			}
			callbackQueue.clear();
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
			this->callbacks.erase(name);
		}

		void Invoke(EventBase& event)
		{
			this->FreeCallbackQueue();
			this->ProcessEvent(event);
		}

		void AddEvent(std::unique_ptr<EventBase> event)
		{
			this->events.push_back(std::move(event));
		}

		void InvokeAll()
		{
			FreeCallbackQueue();
			for (size_t i = 0; i < this->events.size(); i++)
			{
				this->ProcessEvent(*this->events[i]);
			}
			this->events.clear();
		}
	};
}