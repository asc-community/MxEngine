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

#include <array>
#include <vector>
#include "Core/Macro/Macro.h"
#include "Utilities/STL/MxVector.h"

namespace MxEngine
{
    /*!
    class with behave line STL container, but just holds pointer and size of data. Can be used as same as string view for strings
    note that in C++20 std::span is introduced, so array_view will no longer will be needed
    detailed documentation is not provided for this class, as it is quite obvious
    */
    template<typename T>
    class array_view
    {
        T* _data;
        size_t _size;
    public:
        array_view();
        array_view(T* data, size_t size);
        array_view(const array_view&) = default;
        array_view(array_view&&) = default;
        array_view& operator=(const array_view&) = default;
        array_view& operator=(array_view&&) = default;
        ~array_view() = default;
        template<size_t N>
        array_view(T(&array)[N]);
        template<size_t N>
        array_view(std::array<T, N>& array);
        array_view(std::vector<T>& vec);
        array_view(MxVector<T>& vec);
        template<typename RandomIt>
        array_view(RandomIt begin, RandomIt end);
        size_t size() const;
        bool empty() const;
        T* data();
        const T* data() const;
        T& operator[](size_t idx);
        const T& operator[](size_t idx) const;
        T& front();
        const T& front() const;
        T& back();
        const T& back() const;

        T* begin();
        T* end();
        const T* begin() const;
        const T* end() const;
    };

    template<typename T>
    using ArrayView = array_view<T>;

    template<typename T>
    inline array_view<T>::array_view()
    {
        this->_data = nullptr;
        this->_size = 0;
    }

    template<typename T>
    inline array_view<T>::array_view(T* data, size_t size)
    {
        this->_data = data;
        this->_size = size;
    }

    template<typename T>
    inline array_view<T>::array_view(std::vector<T>& vec)
    {
        this->_data = vec.data();
        this->_size = vec.size();
    }

    template<typename T>
    inline array_view<T>::array_view(MxVector<T>& vec)
    {
        this->_data = vec.data();
        this->_size = vec.size();
    }

    template<typename T>
    inline size_t array_view<T>::size() const
    {
        return this->_size;
    }

    template<typename T>
    inline bool array_view<T>::empty() const
    {
        return this->_size == 0;
    }

    template<typename T>
    inline T* array_view<T>::data()
    {
        return this->_data;
    }

    template<typename T>
    inline const T* array_view<T>::data() const
    {
        return this->_data;
    }

    template<typename T>
    inline T& array_view<T>::operator[](size_t idx)
    {
        MX_ASSERT(idx < this->_size);
        return this->_data[idx];
    }

    template<typename T>
    inline const T& array_view<T>::operator[](size_t idx) const
    {
        MX_ASSERT(idx < this->_size);
        return this->_data[idx];
    }

    template<typename T>
    inline T& array_view<T>::front()
    {
        MX_ASSERT(this->_size > 0);
        return this->_data[0];
    }

    template<typename T>
    inline const T& array_view<T>::front() const
    {
        MX_ASSERT(this->_size > 0);
        return this->_data[0];
    }

    template<typename T>
    inline T& array_view<T>::back()
    {
        MX_ASSERT(this->_size > 0);
        return this->_data[this->_size - 1];
    }

    template<typename T>
    inline const T& array_view<T>::back() const
    {
        MX_ASSERT(this->_size > 0);
        return this->_data[this->_size - 1];
    }

    template<typename T>
    inline T* array_view<T>::begin() //-V524
    {
        return this->_data;
    }

    template<typename T>
    inline T* array_view<T>::end()
    {
        return this->_data + this->_size;
    }

    template<typename T>
    inline const T* array_view<T>::begin() const //-V524
    {
        return this->_data;
    }

    template<typename T>
    inline const T* array_view<T>::end() const
    {
        return this->_data + this->_size;
    }

    template<typename T>
    template<size_t N>
    inline array_view<T>::array_view(T(&array)[N])
    {
        this->_data = array;
        this->_size = N;
    }

    template<typename T>
    template<size_t N>
    inline array_view<T>::array_view(std::array<T, N>& array)
    {
        this->_data = array.data();
        this->_size = N;
    }

    template<typename T>
    template<typename RandomIt>
    inline array_view<T>::array_view(RandomIt begin, RandomIt end)
    {
        this->_data = begin;
        this->_size = end - begin;
    }
    
    /*!
    constructs array view from one value
    \param value element which will be inside array_view
    \returns array_view of value
    */
    template<typename T>
    inline array_view<T> make_view(T& value)
    {
        return array_view<T>(&value, 1);
    }
}