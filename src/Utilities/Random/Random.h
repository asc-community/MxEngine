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

#include <random>

namespace MxEngine
{
    /*!
    random generator is a small utility class which uses mersenne twister generator to produce random number
    it is global thread-safe class, which allows you to generate integer numbers with int64 precision
    */
    class Random
    {
    public:
        using RandomReturnType = std::int64_t;

        /*!
        Random generator inner type. Numbers are used from cppreference wiki page
        */
        using Generator = std::mersenne_twister_engine<std::uint_fast64_t, 64, 312, 156, 31,
            0xb5026f5aa96619e9, 29,
            0x5555555555555555, 17,
            0x71d67fffeda60000, 37,
            0xfff7eee000000000, 43, 6364136223846793005>;
    private:
        /*!
        generator instance. One per each thread. Use seed to initialize it to some value (maybe use thread id for that)
        */
        static thread_local inline Generator mersenne64;
    public:
        /*!
        max value which random generator can produce
        */
        static constexpr RandomReturnType Max = std::numeric_limits<RandomReturnType>::max();
        /*!
        min value which random generator can produce
        */
        static constexpr RandomReturnType Min = std::numeric_limits<RandomReturnType>::min();

        /*!
        sets seed for generator. Affects only current thread of execution
        \param seed new initial seed for generator
        */
        static void SetSeed(Generator::result_type seed);
        /*!
        generates random float
        \returns float in interval [0; 1]
        */
        static float GetFloat();
        /*!
        generates random int64 value
        \returns int64 in interval [lower; upper]
        */
        static int64_t Get(int64_t lower, int64_t upper);
        /*!
        generates random int32 value
        \returns int32 in interval [lower; upper]
        */
        static int32_t Get(int32_t lower, int32_t upper);
        /*!
        generates random float value
        \returns float in interval [lower; upper]
        */
        static float Get(float lower, float upper);
    };
}