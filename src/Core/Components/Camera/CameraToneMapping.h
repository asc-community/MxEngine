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

#include "Utilities/ECS/Component.h"
#include "Core/Resources/ACESCurve.h"

namespace MxEngine
{
	class CameraToneMapping
	{
		MAKE_COMPONENT(CameraToneMapping);
	
	public:
		struct ColorChannels
		{
			Vector3 R{}, G{}, B{};
		};
	
	private:
		float gamma = 2.2f;
		float exposure = 1.0f;
		float whitePoint = 1.0f;
		float colorMultiplier = 1.0f;
		ACES coefficients;
		float eyeAdaptationSpeed = 0.1f;
		float eyeAdaptationThreshold = 0.1f;
		float minLuminance = 0.0f;
		float maxLuminance = 100000.0f;
		ColorChannels colorChannels = {
			Vector3{1.0f, 0.0f, 0.0f},
			Vector3{0.0f, 1.0f, 0.0f},
			Vector3{0.0f, 0.0f, 1.0f},
		};

	public:
		float GetGamma() const;
		const ColorChannels& GetColorGrading() const;
		float GetExposure() const;
		float GetColorScale() const;
		float GetWhitePoint() const;
		const ACES& GetACESCoefficients() const;
		float GetEyeAdaptationSpeed() const;
		float GetEyeAdaptationThreshold() const;
		float GetMinLuminance() const;
		float GetMaxLuminance() const;

		void SetColorGrading(const ColorChannels& channels);
		void SetGamma(float gamma);
		void SetExposure(float exposure);
		void SetColorScale(float mult);
		void SetWhitePoint(float point);
		void SetEyeAdaptationThreshold(float threshold);
		void SetEyeAdaptationSpeed(float speed);
		void SetMinLuminance(float lum);
		void SetMaxLuminance(float lum);
		void SetACESCoefficients(const ACES& aces);
	};
}