// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
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

#include "PythonEngine.h"
#if defined(MXENGINE_SCRIPTING_PYTHON)

#include "Core/Application/Application.h"
#include "Core/Interfaces/GraphicAPI/GraphicFactory.h"

namespace MxEngine
{
    PythonEngine::PythonEngine()
    {
        ::Py_Initialize();

        this->pythonModule = python::import("__main__");
        this->pythonNamespace = this->pythonModule.attr("__dict__");

        // importing MxEngine library & system
        this->Execute("import mx_engine");
        this->Execute("import sys");

        // creating IO handler
        const char* IOHandler = R"(
class MxEngineIOHandler:
    def __init__(self):
        self.value = ''
    def write(self, txt):
        self.value += txt)";
        this->Execute(IOHandler);
        
        // redirecting stdout and stderr
        this->Execute("errorHandler = MxEngineIOHandler()");
        this->Execute("sys.stderr = errorHandler");
        this->Execute("outputHandler = MxEngineIOHandler()");
        this->Execute("sys.stdout = outputHandler");

        // very unsafe. But idk how to do it the best way
        auto ctxPtr = reinterpret_cast<uint64_t>(Context::Instance());
        auto grfPtr = reinterpret_cast<uint64_t>(Graphics::Instance());
        this->Execute("mx_engine.MxEngineSetContextPointers(" + std::to_string(ctxPtr) + ", " + std::to_string(grfPtr) + ")");
        this->Execute("ctx = mx_engine.get_context()");
    }

    PythonEngine::BoxedValue PythonEngine::Execute(const std::string& code)
    {
        return this->Execute(code.c_str());
    }

    PythonEngine::BoxedValue PythonEngine::Execute(const char* code)
    {
        try
        {
            BoxedValue result = python::exec(code, this->pythonNamespace);
            // clear all errors (stderr)
            if(this->pythonNamespace.contains("errorHandler"))
                this->pythonNamespace["errorHandler"].attr("value") = "";
            this->lastError.clear();
            // get output (stdout)
            if (this->pythonNamespace.contains("outputHandler"))
            {
                python::object output = this->pythonNamespace["outputHandler"].attr("value");
                this->lastOutput = python::extract<std::string>(output);
                if(!this->lastOutput.empty())
                    this->lastOutput.pop_back(); // delete last '\n'

                this->pythonNamespace["outputHandler"].attr("value") = "";
            }
            return result;
        }
        catch (python::error_already_set&)
        {
            ::PyErr_Print();

            python::object msg = this->pythonNamespace["errorHandler"].attr("value");
            this->lastError = python::extract<std::string>(msg);
            if (!this->lastError.empty())
                this->lastError.pop_back(); // delete last '\n'

            this->lastOutput.clear();
            ::PyErr_Clear();
            return BoxedValue();
        }
    }

    const std::string& PythonEngine::GetErrorMessage() const
    {
        return this->lastError;
    }

    bool PythonEngine::HasErrors() const
    {
        return !this->lastError.empty();
    }

    const std::string& PythonEngine::GetOutput() const
    {
        return this->lastOutput;
    }
}
#endif