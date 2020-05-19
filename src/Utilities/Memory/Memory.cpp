#include "Memory.h"

void* operator new[](size_t size, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
{
    return std::malloc(size);
}

void* operator new[](size_t size, size_t align, size_t offset, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
{
    return std::malloc(size);
}
