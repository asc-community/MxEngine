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

#include "Profiler.h"

namespace MxEngine
{
	void ProfileSession::WriteJsonHeader()
	{
		if (!this->IsValid()) return;
		output << "{\n";
		output << "  \"traceEvents\": [\n";
	}

	void ProfileSession::WriteJsonFooter()
	{
		if (!this->IsValid()) return;
		output << "\n  ]\n";
		output << '}';
	}

	bool ProfileSession::IsValid() const
	{
		return this->output.IsOpen();
	}

	size_t ProfileSession::GetEntryCount() const
	{
		return this->entriesCount;
	}

	void ProfileSession::StartSession(const MxString& filename)
	{
		if (output.IsOpen()) output.Close();
		output.Open(filename.c_str(), File::WRITE);
		this->WriteJsonHeader();
	}

	void ProfileSession::WriteJsonEntry(const char* function, TimeStep begin, TimeStep delta)
	{
		if (!this->IsValid()) return;

		if (this->GetEntryCount() > 0)
		{
			output << ",\n";
		}
		this->entriesCount++;

		output << "	{";
		output << "\"pid\": 0, ";
		output << "\"tid\": 0, ";
		output << "\"ts\": " << std::to_string(uint64_t((double)begin * 1000000)) << ", ";
		output << "\"dur\": " << std::to_string(uint64_t((double)delta * 1000000)) << ", ";
		output << "\"ph\": \"X\", ";
		output << "\"name\": \"" << function << '\"';
		output << "}";
	}

	void ProfileSession::EndSession()
	{
		if (!this->IsValid()) return;
		this->WriteJsonFooter();
		output.Close();
	}
}