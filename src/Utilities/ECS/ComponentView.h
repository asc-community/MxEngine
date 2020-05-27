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

#include "Utilities/AbstractFactory/AbstractFactory.h"
#include "Utilities/VectorPool/VectorPool.h"

namespace MxEngine
{
    /*!
    component view class is used as a wrapper for vector Pool container.
    It was created because vector Pool contains ManagedResource<T> objects,
    but we want to see only T when iterating over components in a Pool
    */
    template<typename T>
    class ComponentView
    {
    public:
        using Pool = VectorPool<ManagedResource<T>>;

        /*!
        wrapper around vector Pool iterator. Actually does nothing more than forwards all methods to wrapped iterator
        */
        class ComponentIterator
        {
        public:
            using PoolIterator = typename Pool::PoolIterator;
        private:
            /*!
            wrapped iterator
            */
            PoolIterator it;
        public:
            /*!
            constructs component iterator object
            \param it wrapped iterator of vector Pool of components
            */
            ComponentIterator(PoolIterator it) : it(it) {}

            /*!
            increments wrapped iterator
            \returns component iterator before increment
            */
            ComponentIterator operator++(int)
            {
                ComponentIterator copy = *this;
                it++;
                return copy;
            }

            /*!
            increments wrapped iterator
            \returns component iterator after increment
            */
            ComponentIterator operator++()
            {
                ++it;
                return *this;
            }

            /*!
            decrements wrapped iterator
            \returns component iterator before decrement
            */
            ComponentIterator operator--(int)
            {
                ComponentIterator copy = *this;
                it--;
                return copy;
            }

            /*!
            decrements wrapped iterator
            \returns component iterator after decrement
            */
            ComponentIterator operator--()
            {
                --it;
                return *this;
            }

            /*!
            getter for component through vector Pool iterator
            \returns pointer to T, instread of ManagedResource<T>*
            */
            T* operator->()
            {
                return &it->value;
            }

            /*!
            getter for component through vector Pool iterator
            \returns const pointer to T, instread of const ManagedResource<T>*
            */
            const T* operator->() const
            {
                return &it->value;
            }

            /*!
            getter for component through vector Pool iterator
            \returns reference to T, instread of ManagedResource<T>&
            */
            T& operator*()
            {
                return it->value;
            }

            /*!
            getter for component through vector Pool iterator
            \returns const reference to T, instread of const ManagedResource<T>&
            */
            const T& operator*() const
            {
                return it->value;
            }

            /*!
            compares two component iterators, forwards comparison to vector Pool iterators
            \returns true if vector Pool iterators are equal, false either
            */
            bool operator==(const ComponentIterator& other) const
            {
                return it == other.it;
            }

            /*!
            compares two component iterators, forwards comparison to vector Pool iterators
            \returns true if vector Pool iterators are not equal, false either
            */
            bool operator!=(const ComponentIterator& other) const
            {
                return it != other.it;
            }
        };
    private:
        /*!
        reference to vector Pool object
        */
        Pool& ref;
    public:
        /*!
        constructs wrapper around vector Pool
        \param ref reference to wrapped vector Pool
        */
        explicit ComponentView(Pool& ref) : ref(ref) {}

        /*!
        begin of vector Pool
        \returns component iterator, pointing to the first element of vector Pool
        */
        ComponentIterator begin()
        {
            return ComponentIterator{ ref.begin() };
        }

        /*!
        begin of vector Pool
        \returns const component iterator, pointing to the first element of vector Pool
        */
        const ComponentIterator begin() const
        {
            return ComponentIterator{ ref.begin() };
        }

        /*!
        end of vector Pool
        \returns component iterator, pointing to the end of vector Pool
        */
        ComponentIterator end()
        {
            return ComponentIterator{ ref.end() };
        }

        /*!
        end of vector Pool
        \returns const component iterator, pointing to the end of vector Pool
        */
        const ComponentIterator end() const
        {
            return ComponentIterator{ ref.end() };
        }
    };
}