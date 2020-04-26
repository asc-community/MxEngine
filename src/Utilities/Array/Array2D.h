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

#include <vector>
#include "Utilities/Array/ArrayView.h"

namespace MxEngine
{
    // rectangular array sized [width * height] where element is access as array[width_index][height_index]
    template<typename T, template<typename> typename Base = std::vector>
    class Array2D
    {
    public:
        using BaseStorage = Base<T>;

    private:
        BaseStorage _vec;
        size_t _height;
    public:

        Array2D();
        Array2D(const Array2D&) = default;
        Array2D(Array2D&&) = default;
        Array2D& operator=(const Array2D&) = default;
        Array2D& operator=(Array2D&&) = default;
        ~Array2D() = default;
        size_t size() const;
        size_t width() const;
        size_t height() const;
        T* data();
        const T* data() const;
        ArrayView<T> operator[](size_t idx);
        ArrayView<const T> operator[](size_t idx) const;

        void resize(size_t width, size_t height, T value = T());
        void rearrange(size_t width, size_t height);
        void clear();

        auto begin() { return this->_vec.begin(); }
        auto end() { return  this->_vec.end(); }
        auto begin() const { return  this->_vec.begin(); }
        auto end() const { return  this->_vec.end(); }
    };

    template<typename T, template<typename> typename Base>
    inline Array2D<T, Base>::Array2D()
        : _height(0)
    {
    }

    template<typename T, template<typename> typename Base>
    inline size_t Array2D<T, Base>::size() const
    {
        return this->_vec.size();
    }

    template<typename T, template<typename> typename Base>
    inline size_t Array2D<T, Base>::height() const
    {
        return this->_height;
    }

    template<typename T, template<typename> typename Base>
    inline size_t Array2D<T, Base>::width() const
    {
        return this->_height == 0 ? 0 : this->_vec.size() / this->_height;
    }

    template<typename T, template<typename> typename Base>
    inline T* Array2D<T, Base>::data()
    {
        return this->_vec.data();
    }

    template<typename T, template<typename> typename Base>
    inline const T* Array2D<T, Base>::data() const
    {
        return this->_vec.data();
    }

    template<typename T, template<typename> typename Base>
    inline ArrayView<T> Array2D<T, Base>::operator[](size_t idx)
    {
        MX_ASSERT(idx * this->_height < this->_vec.size());
        return ArrayView<T>(_vec.data() + idx * this->_height, this->_height);
    }

    template<typename T, template<typename> typename Base>
    inline ArrayView<const T> Array2D<T, Base>::operator[](size_t idx) const
    {
        MX_ASSERT(idx * this->_height < this->_vec.size());
        return ArrayView<const T>(_vec.data() + idx * this->_height, this->_height);
    }

    template<typename T, template<typename> typename Base>
    inline void Array2D<T, Base>::resize(size_t width, size_t height, T value)
    {
        this->_vec.resize(width * height, value);
        this->_height = height;
    }

    template<typename T, template<typename> typename Base>
    inline void Array2D<T, Base>::rearrange(size_t width, size_t height)
    {
        MX_ASSERT(width * height == this->_vec.size());
        this->_height = height;
    }

    template<typename T, template<typename> typename Base>
    inline void Array2D<T, Base>::clear()
    {
        this->_vec.clear();
        this->_height = 0;
    }
}