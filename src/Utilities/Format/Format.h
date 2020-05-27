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

#include "Vendors/fmt/format.h"
#include "Utilities/STL/MxString.h"

namespace MxEngine
{
    /*!
    formats string. for more info see https://github.com/fmtlib/fmt documentation
    \param formatStr formatting string
    \param args variadic argument list
    \returns formatted string object
    */
    template<typename S, typename... Args, typename Char = fmt::char_t<S>>
    inline std::basic_string<Char> Format(const S& formatStr, Args&&... args)
    {
        return fmt::format(formatStr, std::forward<Args>(args)...);
    }

    template<typename S, typename... Args, typename Char = fmt::char_t<S>>
    inline MxString MxFormat(const S& formatStr, Args&&... args)
    {
        return MxString { Format(formatStr, std::forward<Args>(args)...).c_str() };
    }
}

template <>
struct fmt::formatter<MxEngine::MxString> : formatter<string_view>
{
    template <typename FormatContext>
    auto format(MxEngine::MxString str, FormatContext& ctx)
    {
        return formatter<string_view>::format(str.c_str(), ctx);
    }
};