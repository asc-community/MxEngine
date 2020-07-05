#pragma once

#include <cstdint>

namespace MxEngine
{
    enum class VerbosityType : uint8_t
    {
        DEBUG   = 0,
        INFO    = 1,
        WARNING = 2,
        ERROR   = 3,
        FATAL   = 4,
    };
    
    enum class VerbosityLevel : uint8_t
    {
        ALL         = 0,
        NO_DEBUG    = 1,
        NO_INFO     = 2,
        ONLY_ERRORS = 3,
        ONLY_FATAL  = 4,
    };
}