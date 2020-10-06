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

#include "Core/Resources/ACESCurve.h"
#include "Utilities/ECS/Component.h"

namespace MxEngine
{
	// TODO: split into multiple components
	class CameraEffects
	{
		MAKE_COMPONENT(CameraEffects);

		float gamma = 2.2f;
		float bloomWeight = 0.5f;
		float exposure = 1.0f;
		float colorMultiplier = 1.0f;
		float whitePoint = 1.0f;
		ACES coefficients;
		float vignetteRadius = 0.0f;
		float vignetteIntensity = 100.0f;
		float eyeAdaptation = 100.0f;
		float minLuminance = 0.0f;
		float maxLuminance = 100000.0f;
		float ssrThickness = 0.5f;
		float ssrMaxCosAngle = 0.5f;
		size_t ssrSteps = 0;
		float ssrMaxDistance = 3.0f;

		bool enableFXAA = false;
		bool toneMapping = false;
		uint8_t bloomIterations = 0;
	public:

		float GetGamma() const;
		float GetBloomWeight() const;
		float GetExposure() const;
		float GetColorScale() const;
		float GetWhitePoint() const;
		const ACES& GetACESCoefficients() const;
		float GetVignetteIntensity() const;
		float GetVignetteRadius() const;
		float GetEyeAdaptation() const;
		float GetMinLuminance() const;
		float GetMaxLuminance() const;
		bool IsFXAAEnabled() const;
		bool IsToneMappingEnabled() const;
		size_t GetBloomIterations() const;
		float GetSSRThickness() const;
		float GetSSRMaxCosAngle() const;
		float GetSSRSteps() const;
		float GetSSRMaxDistance() const;

		void SetGamma(float gamma);
		void SetBloomWeight(float weight);
		void SetExposure(float exposure);
		void SetColorScale(float mult);
		void SetWhitePoint(float point);
		void SetEyeAdaptation(float point);
		void SetMinLuminance(float lum);
		void SetMaxLuminance(float lum);
		void SetACESCoefficients(const ACES& aces);
		void SetVignetteRadius(float radius);
		void SetVignetteIntensity(float intensity);
		void ToggleFXAA(bool value);
		void ToggleToneMapping(bool value);
		void SetBloomIterations(size_t iterations);
		void SetSSRThickness(float thickness);
		void SetSSRMaxCosAngle(float angle);
		void SetSSRSteps(size_t steps);
		void SetSSRMaxDistance(float distance);
	};
}