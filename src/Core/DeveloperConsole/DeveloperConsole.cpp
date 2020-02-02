#include "DeveloperConsole.h"
#include "Utilities/ImGui/GraphicConsole.h"
#include "Utilities/Profiler/Profiler.h"

MomoEngine::DeveloperConsole::DeveloperConsole()
{
    MAKE_SCOPE_PROFILER("DeveloperConsole::Init");
    this->engine = Alloc<chaiscript::ChaiScript>();
    this->console = Alloc<GraphicConsole>();
    this->AddVariable("console", std::ref(*this));

    this->AddReference("print", &DeveloperConsole::Log);
    this->AddReference("clear", &DeveloperConsole::ClearLog);
    this->AddReference("history", &DeveloperConsole::PrintHistory);

    this->engine->eval(R"(global print = fun[print](x) { console.print("${ x }"); }; )");
    this->Log("This console is powered by ChaiScript: http://chaiscript.com");

    this->console->SetEventCallback([this](const char* text)
        {
            try
            {
                std::string script = text;
                this->engine->eval(script);
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

MomoEngine::DeveloperConsole::~DeveloperConsole()
{
    Free(this->engine);
    Free(this->console);
}

void MomoEngine::DeveloperConsole::Log(const std::string& message)
{
    this->console->PrintLog("%s", message.c_str());
}

void MomoEngine::DeveloperConsole::ClearLog()
{
    this->console->ClearLog();
}

void MomoEngine::DeveloperConsole::PrintHistory()
{
    this->console->PrintHistory();
}

void MomoEngine::DeveloperConsole::_SetSize(float width, float height)
{
    this->console->SetSize({ width, height });
}

void MomoEngine::DeveloperConsole::_Draw() const
{
    this->console->Draw("Developer Console");
}
