// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in sourceand binary forms, with or without
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

#include "DeveloperConsole.h"
#include "Library/Scripting/ScriptEngine.h"
#include "Utilities/ImGui/GraphicConsole.h"
#include "Utilities/Profiler/Profiler.h"

MxEngine::DeveloperConsole::DeveloperConsole()
{
    MAKE_SCOPE_PROFILER("DeveloperConsole::Init");
    this->engine = Alloc<ScriptEngine>();
    this->console = Alloc<GraphicConsole>();
    this->engine->AddVariable("console", *this);

    this->engine->AddReference("print", &DeveloperConsole::Log);
    this->engine->AddReference("clear", &DeveloperConsole::ClearLog);
    this->engine->AddReference("history", &DeveloperConsole::PrintHistory);

    this->engine->GetInterpreter().eval(R"(global print = fun[print](x) { console.print("${ x }"); }; )");

    // workaround to fix performance issues (https://github.com/ChaiScript/ChaiScript/issues/514)
    this->engine->AddTypeConversion<int, float>();
    this->engine->AddTypeConversion<int, double>();
    this->engine->AddTypeConversion<float, int>();
    this->engine->AddTypeConversion<float, double>();
    this->engine->AddTypeConversion<double, float>();
    this->engine->AddTypeConversion<double, int>();

    this->console->SetEventCallback([this](const char* text)
        {
            try
            {
                std::string script = text;
                this->engine->GetInterpreter().eval(script);
            }
            catch (std::exception& e)
            {
                std::string error = e.what();
                size_t idx = error.find("Error:");
                if (idx != error.npos)
                {
                    error.erase(idx, idx + 6);
                    error = "[error]: " + error;
                }
                this->Log(error);
            }
        });
}

MxEngine::DeveloperConsole::~DeveloperConsole()
{
    Free(this->engine);
    Free(this->console);
}

void MxEngine::DeveloperConsole::Log(const std::string& message)
{
    this->console->PrintLog("%s", message.c_str());
}

void MxEngine::DeveloperConsole::ClearLog()
{
    this->console->ClearLog();
}

void MxEngine::DeveloperConsole::PrintHistory()
{
    this->console->PrintHistory();
}

void MxEngine::DeveloperConsole::OnRender()
{
    if (this->shouldRender)
    {
        ImGui::SetNextWindowPos({ 0, 0 });
        this->console->Draw("Developer Console");
        ImGui::End();
    }
}

void MxEngine::DeveloperConsole::SetSize(const Vector2& size)
{
    this->console->SetSize({ size.x, size.y });
}

void MxEngine::DeveloperConsole::Toggle(bool isVisible)
{
    this->shouldRender = isVisible;
}

MxEngine::ScriptEngine& MxEngine::DeveloperConsole::GetEngine()
{
    return *this->engine;
}

MxEngine::Vector2 MxEngine::DeveloperConsole::GetSize() const
{
    return Vector2(this->console->GetSize().x, this->console->GetSize().y);
}

bool MxEngine::DeveloperConsole::IsToggled() const
{
    return this->shouldRender;
}
