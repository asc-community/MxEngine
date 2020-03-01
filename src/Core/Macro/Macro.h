#pragma once

#define MXENGINE_SCRIPTING_PYTHON
// #define MXENGINE_SCRIPTING_CHAISCRIPT

#define MXENGINE_CONCAT_IMPL(x, y) x##y
#define MXENGINE_CONCAT(x, y) MXENGINE_CONCAT_IMPL(x, y)

#define INVOKE_ONCE(...) static char MXENGINE_CONCAT(unused, __LINE__) = [&](){ __VA_ARGS__; return '\0'; }()