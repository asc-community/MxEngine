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

#pragma once

#include "Utilities/FileSystem/File.h"
#include "Vendors/json/json.hpp"

namespace MxEngine
{
    using JsonFile = nlohmann::json;

    namespace Json
    {
        inline JsonFile LoadJson(File& file)
        {
            return nlohmann::json::parse(file.ReadAllText());
        }

        inline void SaveJson(File& file, const JsonFile& json)
        {
            file << std::setw(2) << json;
        }
    }
}

#include "Utilities/Math/Math.h"
#include "Core/Event/Events/KeyEvent.h"

namespace nlohmann
{
    using namespace MxEngine;

    void to_json(JsonFile& j, const Vector3& v);
    void from_json(const JsonFile& j, Vector3& v);
    void to_json(JsonFile& j, const Vector2& v);
    void from_json(const JsonFile& j, Vector2& v);
    void to_json(JsonFile& j, const MxString& str);
    void from_json(const JsonFile& j, MxString& str);
    void to_json(JsonFile& j, KeyCode key);
    void from_json(const JsonFile& j, KeyCode& key);
}