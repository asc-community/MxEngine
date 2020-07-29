#include "Memory.h"
#include <cstdlib>

void* operator new[](size_t size, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
{
    return malloc(size);
}

void* operator new[](size_t size, size_t align, size_t offset, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
{
    return malloc(size);
}
