#include "Random.h"

template<typename T>
T GetRandomImpl(MxEngine::Random::Generator& gen, T lower, T upper)
{
    MX_ASSERT(lower <= upper);
    auto period = upper - lower + 1; // Get(..., 3, 3) -> 3
    auto number = gen();
    return lower + number % period;
}

void MxEngine::Random::SetSeed(uint64_t seed)
{
    Random::mersenne64.seed(seed);
}

float MxEngine::Random::GetFloat()
{
    return (float)mersenne64() / (mersenne64.max() - 1);
}

int32_t MxEngine::Random::Get(int32_t lower, int32_t upper)
{
    return GetRandomImpl(mersenne64, lower, upper);
}

int64_t MxEngine::Random::Get(int64_t lower, int64_t upper)
{
    return GetRandomImpl(mersenne64, lower, upper);
}

float MxEngine::Random::Get(float lower, float upper)
{
    return lower + MxEngine::Random::GetFloat() * (upper - lower);
}
