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
#include <string>

#include "Utilities/Memory/Memory.h"
#include "Platform/GraphicAPI.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
	struct Material
	{
		GResource<Texture> map_Ka;
		GResource<Texture> map_Kd;
		GResource<Texture> map_Ks;
		GResource<Texture> map_Ke;
		GResource<Texture> map_d;
		GResource<Texture> map_normal;
		GResource<Texture> map_height;
		GResource<Texture> bump;

		float Ns = 128.0f;
		float Ni = 0.0f;
		float d = 1.0f;
		float Tr = 0.0f;
		Vector3 Tf{ 0.0f };
		Vector3 Ka{ 0.0f };
		Vector3 Kd{ 0.0f };
		Vector3 Ks{ 0.0f };
		Vector3 Ke{ 0.0f };
		int illum = 0;

		float f_Ka = 0.4f;
		float f_Kd = 0.6f;
		float reflection = 0.1f;
		float displacement = 0.025f;
	};
}