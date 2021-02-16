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

#include "File.h"
#include "Utilities/String/String.h"
#include "Utilities/STL/MxHashMap.h"

namespace MxEngine
{

    struct FileManagerImpl
    {
        MxHashMap<StringId, FilePath> filetable;
    };

    class FileManager
    {
        inline static FileManagerImpl* manager = nullptr;
        static StringId AddFile(const FilePath& file);
    public:
        static MxString OpenFileDialog(const MxString& types = "", const MxString& description = "All Files");
        static MxString SaveFileDialog(const MxString& types = "", const MxString& description = "All Files");
        static void Init();
        static const FilePath& GetFilePath(StringId filename);
        static FilePath GetEngineShaderDirectory();
        static FilePath GetEngineTextureDirectory();
        static FilePath GetEngineModelDirectory();
        static bool FileExists(StringId filename);
        static FilePath SearchForExtensionsInDirectory(const FilePath& directory, const MxString& extension);
        static FilePath SearchFileInDirectory(const FilePath& directory, const MxString& filename);
        static FilePath SearchFileInDirectory(const FilePath& directory, const FilePath& filename);
        static FilePath SearchSubDirectoryInDirectory(const FilePath& directory, const FilePath& filename);
        static FilePath SearchSubDirectoryInDirectory(const FilePath& directory, const MxString& filename);
        static FilePath GetRelativePath(const FilePath& path, const FilePath& directory);
        static FilePath GetProximatePath(const FilePath& path, const FilePath& directory);
        static StringId RegisterExternalResource(const FilePath& path);
        static bool IsInDirectory(const FilePath& path, const FilePath& directory);
        static void Copy(const FilePath& from, const FilePath& to);
        static void InitializeRootDirectory(const FilePath& directory);

        static void Clone(FileManagerImpl* other);
        static FileManagerImpl* GetImpl();
        static FilePath GetWorkingDirectory();
    };
}