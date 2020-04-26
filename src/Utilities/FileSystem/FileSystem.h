// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
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

#pragma once

#include <filesystem>
#include <fstream>

namespace MxEngine
{
    using FilePath = std::filesystem::path;
    using FileStream = std::fstream;
    using FileSystemTime = std::filesystem::file_time_type;

    class File
    {
        FileStream fileStream;
        FilePath filePath;
    public:
        enum FileMode
        {
            READ = 0x1,
            WRITE = 0x2,
            BINARY = 0x4,
        };
        using FileData = std::string;

        File() = default;
        File(const FilePath& path, FileMode mode = FileMode::READ);
        File(const std::string& path, FileMode mode = FileMode::READ);
        File(File&) = delete;
        File(File&&) = default;
        File& operator=(const File&) = delete;
        File& operator=(File&&) = default;
        ~File() = default;

        FileStream& GetStream();
        bool IsOpen() const;
        void Open(FilePath path, FileMode mode = FileMode::READ);
        void Open(const std::string& path, FileMode mode = FileMode::READ);
        FileData ReadAllText();
        FilePath GetPath() const;

        template<typename T>
        File& operator>>(T& value);
        template<typename T>
        File& operator<<(T& value);

        static FileData ReadAllText(const FilePath& path);
        static FileData ReadAllText(const std::string& path);
        static bool Exists(const FilePath& path);
        static FileSystemTime LastModifiedTime(const FilePath& path);
    };

    template<typename T>
    inline File& File::operator>>(T& value)
    {
        this->fileStream >> value;
        return *this;
    }

    template<typename T>
    inline File& File::operator<<(T& value)
    {
        this->fileStream << value;
        return *this;
    }
}