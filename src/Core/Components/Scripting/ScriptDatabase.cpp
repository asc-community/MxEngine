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

#include "ScriptDatabase.h"

namespace MxEngine
{
    const ScriptDatabase::GenericType& ScriptDatabase::GetGeneric(const char* name) const
    {
        MX_ASSERT(this->Contains(name));
        return this->database.find_as(name)->second;
    }

    const ScriptDatabase::GenericType& ScriptDatabase::GetGeneric(const MxString& name) const
    {
        MX_ASSERT(this->Contains(name));
        return this->database.find(name)->second;
    }

    void ScriptDatabase::Remove(const char* name)
    {
        auto it = this->database.find_as(name);
        if (it != this->database.end())
            this->database.erase(it);
    }

    void ScriptDatabase::Remove(const MxString& name)
    {
        auto it = this->database.find(name);
        if (it != this->database.end())
            this->database.erase(it);
    }

    bool ScriptDatabase::Contains(const char* name) const
    {
        return this->database.find_as(name) != this->database.end();
    }

    bool ScriptDatabase::Contains(const MxString& name) const
    {
        return this->database.find(name) != this->database.end();
    }

    MXENGINE_REFLECT_TYPE
    {
        rttr::registration::class_<ScriptDatabase>("ScriptDatabase")
            .constructor<>()
            .property("_database", &ScriptDatabase::GetDatabase, &ScriptDatabase::SetDatabase)
            (
                rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
                rttr::metadata(EditorInfo::CUSTOM_VIEW, GUI::EditorExtra<ScriptDatabase>),
                rttr::metadata(SerializeInfo::CUSTOM_SERIALIZE, SerializeExtra<ScriptDatabase>),
                rttr::metadata(SerializeInfo::CUSTOM_DESERIALIZE, DeserializeExtra<ScriptDatabase>)
            );
    }
}