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
    this->Log("This console is powered by ChaiScript: http://chaiscript.com");

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
