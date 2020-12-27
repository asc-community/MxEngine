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

#include "ShaderPreprocessor.h"
#include "Utilities/STL/MxVector.h"
#include "Utilities/Logging/Logger.h"
#include <regex>

namespace MxEngine
{
    ShaderPreprocessor::ShaderPreprocessor(const MxString& shaderSource)
        : source(shaderSource)
    {
        
    }

    auto FindAllIncludePaths(const MxString& source)
    {
        // path, #include string
        MxVector<std::pair<MxString, MxString>> includes;

        std::regex r(R"(#include\s+\"(.+)\")");
        std::regex_iterator pathIt(source.begin(), source.end(), r);
        std::regex_iterator<const char*> pathEnd;

        for (; pathIt != pathEnd; pathIt++)
        {
            MxString includeCommand(source.begin() + pathIt->position(0), source.begin() + pathIt->position(0) + pathIt->length(0));
            MxString filepath      (source.begin() + pathIt->position(1), source.begin() + pathIt->position(1) + pathIt->length(1));
            includes.emplace_back(std::move(filepath), std::move(includeCommand));
        }

        return includes;
    }

    ShaderPreprocessor& ShaderPreprocessor::LoadIncludes(const FilePath& lookupPath)
    {
        #if defined(MXENGINE_DEBUG)
        this->areIncludeFilePathsLoaded = true;
        #endif

        auto paths = FindAllIncludePaths(this->source);
        for (const auto& [path, include] : paths)
        {
            auto filepath = lookupPath / path.c_str();
            if (!File::Exists(filepath))
            {
                MXLOG_ERROR("ShaderPreprocessor::LoadIncludes", "included file was not found: " + path);
                return *this;
            }
            #if defined(MXENGINE_DEBUG)
            this->includeFilePaths.push_back(path);
            #endif

            auto it = this->source.find(include);
            if (it != this->source.npos) this->source.erase(it, include.size());
            
            this->source = File::ReadAllText(filepath) + this->source;
        }
        // maybe there are new includes in included files
        if (!paths.empty()) this->LoadIncludes(lookupPath);

        return *this;
    }

    ShaderPreprocessor& ShaderPreprocessor::EmitPrefixLine(const MxString& line)
    {
        this->source = line + '\n' + this->source;
        return *this;
    }

    ShaderPreprocessor& ShaderPreprocessor::EmitPostfixLine(const MxString& line)
    {
        this->source += '\n';
        this->source += line;
        return *this;
    }


    MxVector<MxString> emptyFilePathList;

    const MxVector<MxString>& ShaderPreprocessor::GetIncludeFiles() const
    {
        #if defined(MXENGINE_DEBUG)
        if (!this->areIncludeFilePathsLoaded)
        {
            MXLOG_WARNING("MxEngine::ShaderPreprocessor", "included filepaths are not loaded as LoadIncludes() was not called");
        }
        return this->includeFilePaths;
        #else
        MXLOG_WARNING("MxEngine::ShaderPreprocessor", "included filepaths are not saved in non-debug build");
        return emptyFilePathList;
        #endif
    }

    const MxString& ShaderPreprocessor::GetResult()
    {
        return this->source;
    }
}