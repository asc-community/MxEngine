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

#include "ALUtilities.h"
#include "Utilities/Logging/Logger.h"
#include "Utilities/Format/Format.h"

namespace MxEngine
{
    void AlClearErrors()
    {
        while (alGetError() != AL_NO_ERROR);
    }

    bool AlLogCall(const char* function, const char* file, int line)

    {
        bool success = true;
        while (ALenum error = alGetError())
        {
            success = false;
            const char* message = "";
            switch (error)
            {
            case AL_INVALID_NAME:
                message = "AL_INVALID_NAME";
                break;
            case AL_INVALID_ENUM:
                message = "AL_INVALID_ENUM";
                break;
            case AL_INVALID_VALUE:
                message = "AL_INVALID_VALUE";
                break;
            case AL_INVALID_OPERATION:
                message = "AL_INVALID_OPERATION";
                break;
            case AL_OUT_OF_MEMORY:
                message = "AL_OUT_OF_MEMORY";
                break;
            }
            MXLOG_ERROR("OpenAL::ErrorHandler", MxFormat("{0} error in {1}, {2} ({3})", message, function, file, line));
        }
        return success;
    }

    bool ALIsInitialized()
    {
        return alcGetCurrentContext() != nullptr;
    }
}