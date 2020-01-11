#pragma once

#define CHAISCRIPT_NO_THREADS
#include <chaiscript/chaiscript.hpp>

#define CHAI_IMPORT(SIGNATURE, NAME) chai.add(chaiscript::fun(SIGNATURE), #NAME)