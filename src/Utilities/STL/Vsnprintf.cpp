#include <Vendors/eastl/EASTL/string.h>
#include <cstdio>

namespace EA::StdC
{
    EASTL_EASTDC_API int Vsnprintf(char* EA_RESTRICT pDestination, size_t n, const char* EA_RESTRICT pFormat, va_list arguments)
    {
        return std::vsnprintf(pDestination, n, pFormat, arguments);
    }
}