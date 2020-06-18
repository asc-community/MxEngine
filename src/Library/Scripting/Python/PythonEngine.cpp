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

#include "PythonEngine.h"
#if defined(MXENGINE_USE_PYTHON)

#include "Core/Application/Application.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Utilities/ECS/ComponentFactory.h"
#include "Core/Resources/ResourceFactory.h"

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
    def __init__(self, std_stream):
        self.value = ''
        self.std = std_stream
    def write(self, txt):
        self.value += txt
        if self.std is not None:
            self.std.write(txt)
    def flush(self):
        pass
)";
        this->Execute(IOHandler);
        
        // redirecting stdout and stderr
        this->MirrorOutStream(false);
        this->MirrorErrorStream(false);

        auto ctxPtr = reinterpret_cast<uintptr_t>(Application::Get());
        auto fileManagerPtr = reinterpret_cast<uintptr_t>(FileManager::GetImpl());
        auto uuidGenPtr = reinterpret_cast<uintptr_t>(UUIDGenerator::GetImpl());
        auto graphicPtr = reinterpret_cast<uintptr_t>(GraphicFactory::GetImpl());
        auto componentPtr = reinterpret_cast<uintptr_t>(ComponentFactory::GetImpl());
        auto mxobjectPtr = reinterpret_cast<uintptr_t>(MxObject::Factory::GetImpl());
        auto resourcePtr = reinterpret_cast<uintptr_t>(ResourceFactory::GetImpl());
        auto contextInitScript = Format("mx_engine.MxEngineSetContextPointer({}, {}, {}, {}, {}, {}, {})", 
            ctxPtr, fileManagerPtr, uuidGenPtr, graphicPtr, componentPtr, mxobjectPtr, resourcePtr);
        this->Execute(contextInitScript.c_str());
        this->Execute("mx = mx_engine.get_context()");
    }

    PythonEngine::BoxedValue PythonEngine::Execute(const char* code)
    {
        return BoxedValue{ };
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
                this->lastOutput = ToMxString((std::string)python::extract<std::string>(output));
                if(!this->lastOutput.empty())
                    this->lastOutput.pop_back(); // delete last '\n'

                this->pythonNamespace["outputHandler"].attr("Value") = "";
            }
            return result;
        }
        catch (python::error_already_set&)
        {
            ::PyErr_Print();
            try
            {
                python::object msg = this->pythonNamespace["errorHandler"].attr("value");
                this->lastError = ToMxString((std::string)python::extract<std::string>(msg));
            }
            catch (python::error_already_set&)
            {
                this->lastError = "cannot get python error. Probably python module is not initialized correctly\n";
            }
            if (!this->lastError.empty())
                this->lastError.pop_back(); // delete last '\n'

            this->lastOutput.clear();
            ::PyErr_Clear();
            return BoxedValue();
        }
    }

    void PythonEngine::MirrorOutStream(bool value)
    {
        if (value)
            this->Execute("outputHandler = MxEngineIOHandler(sys.__stdout__)");
        else
            this->Execute("outputHandler = MxEngineIOHandler(None)");

        this->Execute("sys.stdout = outputHandler");
    }

    void PythonEngine::MirrorErrorStream(bool value)
    {
        if (value)
            this->Execute("errorHandler = MxEngineIOHandler(sys.__stderr__)");
        else
            this->Execute("errorHandler = MxEngineIOHandler(None)");

        this->Execute("sys.stderr = errorHandler");
    }

    PythonEngine::PythonModule& PythonEngine::GetModule()
    {
        return this->pythonModule;
    }

    PythonEngine::PythonNamespace& PythonEngine::GetNamespace()
    {
        return this->pythonNamespace;
    }

    const MxString& PythonEngine::GetErrorMessage() const
    {
        return this->lastError;
    }

    bool PythonEngine::HasErrors() const
    {
        return !this->lastError.empty();
    }

    const MxString& PythonEngine::GetOutput() const
    {
        return this->lastOutput;
    }
}
#endif