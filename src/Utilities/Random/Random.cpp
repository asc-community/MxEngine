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

#include "Random.h"
#include "Core/Macro/Macro.h"

namespace MxEngine
{
    template<typename T>
    T GetRandomImpl(Random::Generator& gen, T lower, T upper)
    {
        MX_ASSERT(lower <= upper);
        auto period = upper - lower + 1; // Get(..., 3, 3) -> 3
        auto number = (T)gen();
        return lower + number % period;
    }

    void Random::SetSeed(Generator::result_type seed)
    {
        Random::mersenne64.seed(seed);
    }

    float Random::GetFloat()
    {
        return (float)mersenne64() / (mersenne64.max() - 1);
    }

    int32_t Random::Get(int32_t lower, int32_t upper)
    {
        return GetRandomImpl(mersenne64, lower, upper);
    }

    int64_t Random::Get(int64_t lower, int64_t upper)
    {
        return GetRandomImpl(mersenne64, lower, upper);
    }

    float Random::Get(float lower, float upper)
    {
        return lower + Random::GetFloat() * (upper - lower);
    }
}