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

#include "Counter.h"

namespace MxEngine
{
    /*!
    constructs new Counter object
    \param first initial value of counter. Defaults to 0
    */
    Counter::Counter(Counter::CounterType first)
        : counter(first) {  }

    /*!
    this function does not affect Counter object in any way
    \returns current value of Counter
    */
    Counter::CounterType Counter::Get() const
    {
        return this->counter;
    }

    /*!
    increments inner counter by one
    \returns value after increment
    */
    Counter::CounterType Counter::operator++()
    {
        return ++this->counter;
    }

    /*!
    increments inner counter by one
    \returns value before increment
    */
    Counter::CounterType Counter::operator++(int)
    {
        return this->counter++;
    }

    /*!
    increment counter by a specific value
    \param value value to increment counter by
    */
    void Counter::Add(Counter::CounterType value)
    {
        this->counter += value;
    }

    /*!
    sets inner counter to some value.
    \param first value to set. Defaults to 0
    */
    void Counter::Reset(Counter::CounterType first)
    {
        this->counter = first;
    }
}