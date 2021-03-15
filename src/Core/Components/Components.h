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

#include "Instancing/InstanceFactory.h"
#include "Lighting/DirectionalLight.h"
#include "Lighting/SpotLight.h"
#include "Lighting/PointLight.h"
#include "Rendering/MeshRenderer.h"
#include "Rendering/MeshSource.h"
#include "Rendering/MeshLOD.h"
#include "Rendering/Skybox.h"
#include "Rendering/DebugDraw.h"
#include "Rendering/ParticleSystem.h"
#include "Camera/CameraController.h"
#include "Camera/CameraEffects.h"
#include "Camera/CameraToneMapping.h"
#include "Camera/CameraSSR.h"
#include "Camera/CameraSSGI.h"
#include "Camera/CameraSSAO.h"
#include "Camera/InputController.h"
#include "Camera/VRCameraController.h"
#include "Audio/AudioListener.h"
#include "Audio/AudioSource.h"
#include "Physics/BoxCollider.h"
#include "Physics/SphereCollider.h"
#include "Physics/CylinderCollider.h"
#include "Physics/CapsuleCollider.h"
#include "Physics/CompoundCollider.h"
#include "Physics/CharacterController.h"
#include "Physics/RigidBody.h"
#include "Scripting/Script.h"
#include "Transform.h"
#include "Behaviour.h"

namespace MxEngine
{
    void RegisterComponents();
}