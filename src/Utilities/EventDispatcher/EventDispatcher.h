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

#include <functional>
#include <algorithm>

#include "Utilities/Profiler/Profiler.h"
#include "Utilities/Memory/Memory.h"
#include "Utilities/STL/MxHashMap.h"
#include "Utilities/STL/MxVector.h"

namespace MxEngine
{
    /*!
    EventDispatcher class is used to handle all events inside MxEngine. Events can either be dispatch for Application (global) or
    for currently active scene. Note that events are NOT dispatched when developer console is opened and instead sheduled until it close
    */
    template<typename EventBase>
    class EventDispatcherImpl
    {
        using CallbackBaseFunction = std::function<void(EventBase&)>;
        using NamedCallback = std::pair<MxString, CallbackBaseFunction>;
        using CallbackList = MxVector<NamedCallback>;
        using EventList = MxVector<UniqueRef<EventBase>>;
        using EventTypeIndex = uint32_t;

        /*!
        list of scheduled all events
        */
        EventList events;
        /*!
        maps event id to list of event listeners of that id 
        */
        MxHashMap<EventTypeIndex, CallbackList> callbacks;
        /*!
        schedules listeners which will be added next frame. 
        This cache exists to prevent crushes when user wants to add new listener inside other listener callback.
        */
        MxHashMap<EventTypeIndex, CallbackList> toAddCache;
        /*!
        shedules listeners which will be removed next frame. 
        This cache exists to prevent crushes when user wants to remove event listener inside other listener callback (or even perform self-removal).
        */
        MxVector<MxString> toRemoveCache;

        /*!
        immediately invokes all listeners of event, if any exists
        \param event event to dispatch
        */
        inline void ProcessEvent(EventBase& event)
        {
            MAKE_SCOPE_PROFILER(typeid(event).name());
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
            callbacks.erase(it, callbacks.end());
        }

        /*!
        wraps event listener into callback with base event as argument. Actual type is retrieved inside callback
        \param name name of listener to add
        \param func listener callback function
        */
        template<typename EventType>
        void AddEventListenerImpl(const MxString& name, std::function<void(EventType&)> func)
        {
            this->template AddCallbackImpl<EventType>(name, [func = std::move(func)](EventBase& e)
            {
                if (e.GetEventType() == EventType::eventType)
                    func(static_cast<EventType&>(e));
            });
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
        adds new event listener to dispatcher
        Note that multiple listeners may have same name. If so, deleting by name will result in removing all of them
        \param name name of listener (used for deleting listener)
        \param func listener callback functor
        */
        template<typename EventType>
        void AddEventListener(const MxString& name, std::function<void(EventType&)> func)
        {
            this->AddEventListenerImpl(name, std::move(func));
        }

        /*!
        adds new event listener to dispatcher
        Note that multiple listeners may have same name. If so, deleting by name will result in removing all of them
        \param name name of listener (used for deleting listener)
        \param func listener callback functor
        */
        template<typename T, typename FunctionType>
        void AddEventListener(const MxString& name, FunctionType&& func)
        {
            this->AddEventListenerImpl(name, std::function<void(T&)>{ std::forward<FunctionType>(func) });
        }

        /*!
        removes all event listeners by their names
        \param name name of listeners to be deleted
        */
        void RemoveEventListener(const MxString& name)
        {
            this->toRemoveCache.push_back(name);
            
            for (auto& [event, callbacks] : this->toAddCache)
            {
                RemoveEventByName(callbacks, name);
            }
        }
        
        /*!
        Immediately invokes event of specific type
        \param event event to dispatch
        */
        template<typename Event>
        void Invoke(Event& event)
        {
            this->FlushEvents();
            this->ProcessEvent(event);
        }

        /*!
        Adds event to event queue.
        \param event event to shedule dispatch
        */
        void AddEvent(UniqueRef<EventBase> event)
        {
            this->events.push_back(std::move(event));
        }

        /*!
        Invokes all shedules events in the order they were added
        */
        void InvokeAll()
        {
            this->FlushEvents();

            for (size_t i = 0; i < this->events.size(); i++)
            {
                this->ProcessEvent(*this->events[i]);
            }
            this->events.clear();
        }

        /*!
        Checks if event with specific name is in listener queue 
        \param name name of event
        \returns true if event listener present, false otherwise
        */
        bool HasEventListenerWithName(const MxString& name) 
        {
            this->FlushEvents();
            for (const auto& [event, callbacks] : this->callbacks)
            {
                for (const auto& [callbackName, func] : callbacks)
                {
                    if (callbackName == name)
                        return true;
                }
            }
            return false;
        }
    };
}