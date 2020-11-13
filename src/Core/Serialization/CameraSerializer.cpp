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

#include "Core/Serialization/SceneSerializer.h"
#include "Core/Components/Camera/CameraController.h"
#include "Core/Components/Camera/FrustrumCamera.h"
#include "Core/Components/Camera/PerspectiveCamera.h"
#include "Core/Components/Camera/OrthographicCamera.h"
#include "Core/Components/Camera/CameraSSR.h"
#include "Core/Components/Camera/CameraToneMapping.h"
#include "Core/Components/Camera/CameraEffects.h"
#include "Core/Components/Camera/VRCameraController.h"
#include "Core/Components/Camera/InputController.h"

namespace MxEngine
{
    void Serialize(JsonFile& json, CameraBase& base)
    {
        json["aspect-ratio"] = base.GetAspectRatio();
        json["center"] = base.GetProjectionCenter();
        json["z-near"] = base.GetZNear();
        json["z-far"] = base.GetZFar();
        json["zoom"] = base.GetZoom();
    }

    void Serialize(JsonFile& json, CameraController& controller)
    {
        json["move-speed"] = controller.GetMoveSpeed();
        json["rotate-speed"] = controller.GetRotateSpeed();
        json["direction"] = controller.GetDirection();
        json["up-vector"] = controller.GetUpVector();
        json["forward-vector"] = controller.GetForwardVector();
        json["right-vector"] = controller.GetRightVector();
        json["listens-resize"] = controller.ListensWindowResizeEvent();
        json["is-rendered"] = controller.IsRendered();
        json["base-type"] = controller.GetCameraType();
        Serialize(json["base"], controller.Camera);
    }

    void Serialize(JsonFile& json, CameraSSR& ssr)
    {
        json["steps"] = ssr.GetSteps();
        json["max-angle"] = ssr.GetMaxCosAngle();
        json["distance"] = ssr.GetMaxDistance();
        json["thickness"] = ssr.GetThickness();
    }

    void Serialize(JsonFile& json, CameraToneMapping& mapping)
    {
        auto& aces = mapping.GetACESCoefficients();
        auto& grading = mapping.GetColorGrading();
        json["aces"]["A"] = aces.A;
        json["aces"]["B"] = aces.B;
        json["aces"]["C"] = aces.C;
        json["aces"]["D"] = aces.D;
        json["aces"]["E"] = aces.E;
        json["aces"]["F"] = aces.F;
        json["color-grading"]["R"] = grading.R;
        json["color-grading"]["G"] = grading.G;
        json["color-grading"]["B"] = grading.B;
        json["color-scale"] = mapping.GetColorScale();
        json["exposure"] = mapping.GetExposure();
        json["eye-adaptation-speed"] = mapping.GetEyeAdaptationSpeed();
        json["eye-adaptation-threshold"] = mapping.GetEyeAdaptationThreshold();
        json["gamma"] = mapping.GetGamma();
        json["max-luminance"] = mapping.GetMaxLuminance();
        json["min-luminance"] = mapping.GetMinLuminance();
        json["white-point"] = mapping.GetWhitePoint();
    }

    void Serialize(JsonFile& json, VRCameraController& vr)
    {
        json["eye-distance"] = vr.EyeDistance;
        json["focus-distance"] = vr.FocusDistance;
        json["left-eye-id"] = vr.LeftEye.GetHandle();
        json["right-eye-id"] = vr.RightEye.GetHandle();
    }

    void Serialize(JsonFile& json, InputController& input)
    {
        json["horizontal-rotation"] = input.IsHorizontalRotationBound();
        json["vertical-rotation"] = input.IsVerticalRotationBound();
        json["up-key"] = input.GetUpKeyBinding();
        json["down-key"] = input.GetDownKeyBinding();
        json["left-key"] = input.GetLeftKeyBinding();
        json["right-key"] = input.GetRightKeyBinding();
        json["forward-key"] = input.GetForwardKeyBinding();
        json["back-key"] = input.GetBackKeyBinding();
    }

    void Serialize(JsonFile& json, CameraEffects& effects)
    {
        json["fxxa-enabled"] = effects.IsFXAAEnabled();
        json["ao-intensity"] = effects.GetAmbientOcclusionIntensity();
        json["ao-radius"] = effects.GetAmbientOcclusionRadius();
        json["ao-samples"] = effects.GetAmbientOcclusionSamples();
        json["bloom-iterations"] = effects.GetBloomIterations();
        json["bloom-weight"] = effects.GetBloomWeight();
        json["fog-color"] = effects.GetFogColor();
        json["fog-density"] = effects.GetFogDensity();
        json["fog-distance"] = effects.GetFogDistance();
        json["ca-distortion"] = effects.GetChromaticAberrationDistortion();
        json["ca-intensity"] = effects.GetChromaticAberrationIntensity();
        json["ca-min-distance"] = effects.GetChromaticAberrationMinDistance();
        json["vignette-intensity"] = effects.GetVignetteIntensity();
        json["vignette-radius"] = effects.GetVignetteRadius();
    }
}