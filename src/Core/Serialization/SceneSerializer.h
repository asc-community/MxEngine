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

#include "Utilities/Json/Json.h"

namespace MxEngine
{
    class MxObject;
    class TransformComponent;
    class Behaviour;
    class DirectionalLight;
    class PointLight;
    class SpotLight;
    class InstanceFactory;
    class Instance;
    class CameraController;
    class CameraSSR;
    class CameraEffects;
    class CameraToneMapping;
    class VRCameraController;
    class InputController;
    class DebugDraw;
    class MeshRenderer;
    class MeshSource;
    class Skybox;
    class MeshLOD;
    class AudioListener;
    class AudioSource;
    class RigidBody;
    class CharacterController;
    class BoxCollider;
    class SphereCollider;
    class CapsuleCollider;
    class CylinderCollider;
    class CompoundCollider;
    class Script;

    void Serialize(JsonFile& json, const MxObject&);
    void Serialize(JsonFile& json, const TransformComponent&);
    void Serialize(JsonFile& json, const Behaviour&);
    void Serialize(JsonFile& json, const DirectionalLight&);
    void Serialize(JsonFile& json, const PointLight&);
    void Serialize(JsonFile& json, const SpotLight&);
    void Serialize(JsonFile& json, const InstanceFactory&);
    void Serialize(JsonFile& json, const Instance&);
    void Serialize(JsonFile& json, const CameraController&);
    void Serialize(JsonFile& json, const CameraSSR&);
    void Serialize(JsonFile& json, const CameraToneMapping&);
    void Serialize(JsonFile& json, const CameraEffects&);
    void Serialize(JsonFile& json, const VRCameraController&);
    void Serialize(JsonFile& json, const InputController&);
    void Serialize(JsonFile& json, const MeshRenderer&);
    void Serialize(JsonFile& json, const MeshSource&);
    void Serialize(JsonFile& json, const Skybox&);
    void Serialize(JsonFile& json, const MeshLOD&);
    void Serialize(JsonFile& json, const DebugDraw&);
    void Serialize(JsonFile& json, const AudioListener&);
    void Serialize(JsonFile& json, const AudioSource&);
    void Serialize(JsonFile& json, const RigidBody&);
    void Serialize(JsonFile& json, const CharacterController&);
    void Serialize(JsonFile& json, const BoxCollider&);
    void Serialize(JsonFile& json, const SphereCollider&);
    void Serialize(JsonFile& json, const CapsuleCollider&);
    void Serialize(JsonFile& json, const CylinderCollider&);
    void Serialize(JsonFile& json, const CompoundCollider&);
    void Serialize(JsonFile& json, const Script&);

    void SerializeScripts(JsonFile& json);
    void SerializeMaterials(JsonFile& json);
    void SerializeMeshes(JsonFile& json);
    void SerializeAudios(JsonFile& json);
    void SerializeCubeMaps(JsonFile& json);
    void SerializeTextures(JsonFile& json);

    class SceneSerializer
    {
        static void SerializeGlobals(JsonFile& json);
        static void SerializeObjects(JsonFile& json);
        static void SerializeResources(JsonFile& json);
    public:
        static JsonFile Serialize();
    };
}