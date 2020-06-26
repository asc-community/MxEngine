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

#include "File.h"
#include "Utilities/Logger/Logger.h"

#include <map>

namespace MxEngine
{
    std::map<int, int> FileModeTable =
    {
        { File::READ, std::fstream::in },
        { File::WRITE, std::fstream::out },
        { File::BINARY, std::fstream::binary },
        { File::READ | File::BINARY, std::fstream::in | std::fstream::binary },
        { File::WRITE | File::BINARY, std::fstream::out | std::fstream::binary },
        { File::READ | File::WRITE, std::fstream::in | std::fstream::out },
        { File::READ | File::WRITE | File::BINARY, std::fstream::in | std::fstream::out | std::fstream::binary },
    };

    File::File(const FilePath& path, int mode)
    {
        this->Open(path, mode);
    }

    File::File(const MxString& path, int mode)
    {
        this->Open(path, mode);
    }

    File::File(const char* path, int mode)
    {
        this->Open(path, mode);
    }

    FileStream& File::GetStream()
    {
        return this->fileStream;
    }

    bool File::IsOpen() const
    {
        return this->fileStream.is_open() && this->fileStream.good();
    }

    void File::Open(FilePath path, int mode)
    {
        this->filePath = std::move(path);
        if (!File::Exists(this->filePath))
        {
            if ((mode & File::WRITE) == 0)
            {
                Logger::Instance().Error("MxEngine::File", "file was not found: " + ToMxString(this->filePath));
                return;
            }
        }
        this->fileStream.open(this->filePath, FileModeTable[mode]);
    }

    void File::Open(const MxString& path, int mode)
    {
        FilePath filepath = path.c_str();
        this->Open(std::move(filepath), mode);
    }

    void File::Open(const char* path, int mode)
    {
        this->Open((FilePath)path, mode);
    }

    void File::Close()
    {
        this->fileStream.close();
    }

    File::FileData File::ReadAllText()
    {
        std::string content;
        if (!this->IsOpen())
        {
            Logger::Instance().Error("MxEngine::File", "file was not opened before reading: " + ToMxString(this->filePath));
        }
        content.assign(std::istreambuf_iterator<char>(this->fileStream), std::istreambuf_iterator<char>());
        return ToMxString(content);
    }

    const FilePath& File::GetPath() const
    {
        return this->filePath;
    }

    File::FileData File::ReadAllText(const FilePath& path)
    {
        return File(path, FileMode::READ).ReadAllText();
    }

    File::FileData File::ReadAllText(const MxString& path)
    {
        return File(path, FileMode::READ).ReadAllText();
    }

    File::FileData File::ReadAllText(const char* path)
    {
        return File(path, FileMode::READ).ReadAllText();
    }

    bool File::Exists(const FilePath& path)
    {
        return std::filesystem::exists(path);
    }

    bool File::Exists(const MxString& path)
    {
        return std::filesystem::exists(path.c_str());
    }

    bool File::Exists(const char* path)
    {
        return std::filesystem::exists(path);
    }

    bool File::IsFile(const MxString& path)
    {
        return std::filesystem::is_regular_file(path.c_str());
    }

    bool File::IsFile(const FilePath& path)
    {
        return std::filesystem::is_regular_file(path);
    }

    bool File::IsFile(const char* path)
    {
        return std::filesystem::is_regular_file(path);
    }

    bool File::IsDirectory(const MxString& path)
    {
        return std::filesystem::is_directory(path.c_str());
    }

    bool File::IsDirectory(const FilePath& path)
    {
        return std::filesystem::is_directory(path.c_str());
    }

    bool File::IsDirectory(const char* path)
    {
        return std::filesystem::is_directory(path);
    }

    FileSystemTime File::LastModifiedTime(const FilePath& path)
    {
        if (!File::Exists(path))
        {
            Logger::Instance().Warning("MxEngine::File", "file was not found: " + ToMxString(path));
            return FileSystemTime();
        }
        return std::filesystem::last_write_time(path);
    }

    FileSystemTime File::LastModifiedTime(const MxString& path)
    {
        if (!File::Exists(path))
        {
            Logger::Instance().Warning("MxEngine::File", "file was not found: " + path);
            return FileSystemTime();
        }
        return std::filesystem::last_write_time(path.c_str());
    }

    FileSystemTime File::LastModifiedTime(const char* path)
    {
        if (!File::Exists(path))
        {
            Logger::Instance().Warning("MxEngine::File", "file was not found: " + (MxString)path);
            return FileSystemTime();
        }
        return std::filesystem::last_write_time(path);
    }

    void File::CreateDirectory(const FilePath& path)
    {
        if (!File::Exists(path))
            std::filesystem::create_directory(path);
    }

    void File::CreateDirectory(const MxString& path)
    {
        if (!File::Exists(path))
            std::filesystem::create_directory(path.c_str());
    }

    void File::CreateDirectory(const char* path)
    {
        if (!File::Exists(path))
            std::filesystem::create_directory(path);
    }

    void File::WriteBytes(const uint8_t* bytes, size_t size)
    {
        this->fileStream.write((const char*)bytes, size);
    }
}