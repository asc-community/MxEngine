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

#include "FileManager.h"
#include "Utilities/Profiler/Profiler.h"
#include "Utilities/Memory/Memory.h"
#include "Utilities/Format/Format.h"
#include <portable-file-dialogs.h>
#undef CreateDirectory


namespace MxEngine
{
    MxString FileManager::OpenFileDialog(const MxString& types, const MxString& description)
    {
        auto selection = pfd::open_file("Select file", FileManager::GetRoot().string(),
            { description.c_str(), types.c_str(), "All Files", "*" }, pfd::opt::multiselect).result();
        return selection.empty() ? "" : ToMxString(selection.front());

    }

    MxString FileManager::SaveFileDialog(const MxString& types, const MxString& description)
    {
        auto selection = pfd::save_file("Save file", FileManager::GetRoot().string(),
            { description.c_str(), types.c_str(), "All Files", "*" }).result();
        return ToMxString(selection);
    }

    void FileManager::InitializeRootDirectory(const FilePath& directory)
    {
        if (!File::Exists(directory))
        {
            File::CreateDirectory(directory);
            MXLOG_DEBUG("MxEngine::FileManager", "creating directory: " + ToMxString(directory));
        }

        namespace fs = std::filesystem;
        auto it = fs::recursive_directory_iterator(directory, fs::directory_options::skip_permission_denied);
        for (const auto& entry : it)
        {
            if (entry.is_regular_file())
            {
                FileManager::AddFile(entry.path());
            }
        }
    }

    const FilePath& FileManager::GetFilePath(StringId filename)
    {
        if (!FileManager::FileExists(filename))
        {
            static FilePath empty;
            return empty;
        }
        return manager->filetable[filename];
    }

    FilePath FileManager::GetEngineShaderFolder()
    {
        return FileManager::GetWorkingDirectory() / "Engine" / "Shaders";
    }

    FilePath FileManager::GetEngineRuntimeFolder()
    {
        return FileManager::GetWorkingDirectory() / "Engine" / "Runtime";
    }

    bool FileManager::FileExists(StringId filename)
    {
        return manager->filetable.find(filename) != manager->filetable.end();
    }

    FilePath FileManager::SearchForExtensionsInDirectory(const FilePath& directory, const MxString& extension)
    {
        namespace fs = std::filesystem;
        FilePath ext = ToFilePath(extension);
        auto it = fs::recursive_directory_iterator(directory);
        for (const auto& entry : it)
        {
            if (entry.path().extension() == ext)
            {
                return entry.path();
            }
        }
        return FilePath();
    }

    FilePath FileManager::SearchInDirectory(const FilePath& directory, const MxString& filename)
    {
        return SearchInDirectory(directory, ToFilePath(filename));
    }

    FilePath FileManager::SearchInDirectory(const FilePath& directory, const FilePath& filename)
    {
        if (!File::Exists(directory)) return FilePath();

        namespace fs = std::filesystem;
        auto it = fs::recursive_directory_iterator(directory);
        for (const auto& entry : it)
        {
            if (entry.path().filename() == filename)
            {
                return entry.path();
            }
        }
        return FilePath();
    }

    void FileManager::AddFile(const FilePath& file)
    {
        auto filename = file.string(); // we need to transform Resources\path\to.something -> path/to.something
        std::replace_if(filename.begin(), filename.end(), [](char c) { return c == FilePath::preferred_separator; }, '/');
        filename.erase(filename.begin(), filename.begin() + manager->rootPathSize + 1);

        auto filehash = MakeStringId(filename);
        if (manager->filetable.find(filehash) != manager->filetable.end())
        {
            MXLOG_WARNING("MxEngine::FileManager", MxFormat("hash of file \"{0}\" conflicts with other one in the project: {1}", filename, manager->filetable[filehash].string()));
        }
        else
        {
            MXLOG_DEBUG("MxEngine::FileManager", MxFormat("file added to the project: {0}", filename));
        }
        manager->filetable.emplace(filehash, file);
    }

    void FileManager::Init()
    {
        manager = Alloc<FileManagerImpl>();
    }

    void FileManager::Clone(FileManagerImpl* other)
    {
        manager = other;
    }

    FileManagerImpl* FileManager::GetImpl()
    {
        return manager;
    }

    FilePath& FileManager::GetRoot()
    {
        return manager->root;
    }

    FilePath FileManager::GetWorkingDirectory()
    {
        return std::filesystem::current_path();
    }

    void FileManager::SetRoot(const FilePath& rootPath)
    {
        MAKE_SCOPE_TIMER("MxEngine::FileManager", "FileManager::Init()");
        MAKE_SCOPE_PROFILER("FileManager::Init()");

        if (rootPath.is_relative())
        {
            manager->root = std::filesystem::current_path() / rootPath;
        }
        else
        {
            manager->root = rootPath;
        }
        MXLOG_DEBUG("MxEngine::FileManager", "setting root directory to: " + ToMxString(manager->root));

        manager->rootPathSize = rootPath.string().size();
        FileManager::InitializeRootDirectory(rootPath);
    }
}