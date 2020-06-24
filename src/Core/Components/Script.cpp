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

#include "Script.h"
#include "Utilities/Logger/Logger.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Core/Application/Application.h"

namespace MxEngine
{
    Script::Script(StringId hash)
        : Script(FileManager::GetFilePath(hash)) { }

    Script::Script(const FilePath& path)
        : Script(ToMxString(path)) { }

    Script::Script(const MxString& path)
    {
        this->Load(path);
    }

    void Script::Load(StringId hash)
    {
        this->Load(FileManager::GetFilePath(hash));
    }

    void Script::Load(const FilePath& path)
    {
        this->Load(ToMxString(path));
    }

    void Script::Load(const MxString& path)
    {
        this->path = path;
        if (!File::Exists(this->path))
            Logger::Instance().Warning("MxEngine::Script", "script is not loaded, file was not found: " + path);

        this->UpdateContents();
    }

    void Script::UpdateContents()
    {
        auto currentTime = Time::Current();
        if (currentTime - this->lastUpdate > 1.0f)
        {
            this->lastUpdate = currentTime;
            if (File::Exists(this->path))
            {
                auto systemFileTime = File::LastModifiedTime(this->path);
                if (this->fileUpdate < systemFileTime)
                {
                    Logger::Instance().Debug("MxEngine::Script", "updated script: " + this->path);
                    this->data = File(this->path).ReadAllText();
                }

                this->fileUpdate = std::move(systemFileTime);
            }
        }
    }

    void Script::Execute()
    {
        auto& editor = Application::Get()->GetRuntimeEditor();
        MAKE_SCOPE_PROFILER("Application::ExecuteScript");
        editor.ExecuteScript(this->GetContent());
        if (editor.HasErrorsInExecution())
        {
            Logger::Instance().Error("Application::ExecuteScript", editor.GetLastErrorMessage());
        }
    }

    const MxString& Script::GetFilePath() const
    {
        return this->path;
    }

    const Script::ScriptData& Script::GetContent() const
    {
        return this->data;
    }

    Script::ScriptData& Script::GetContent()
    {
        return this->data;
    }
}