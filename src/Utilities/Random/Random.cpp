#include "Random.h"

void MxEngine::Random::SetSeed(unsigned int seed)
{
    srand(seed);
}

float MxEngine::Random::GetFloat()
{
    return float(rand()) / Random::Max;
}

int MxEngine::Random::Get(int lower, int upper)
{
    return int(GetFloat() * (upper - lower)) + lower;
}

float MxEngine::Random::Get(float lower, float upper)
{
    return GetFloat() * (upper - lower) + lower;
}
