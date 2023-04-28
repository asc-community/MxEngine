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

#include "Core/Application/Runtime.h"
#include "Components.h"
#include "Utilities/Factory/FactoryImpl.h"

namespace MxEngine
{
    template<typename T>
    void RegisterComponent()
    {
        Runtime::RegisterComponent<T>();
    }

    #define TEMPLATE_INSTANCIATE_RESOURCE(T) template class Resource<T, Factory<T>>; template class Factory<T>
    #define TEMPLATE_INSTANCIATE_COMPONENT(T) template class Resource<T, ComponentFactory>

    TEMPLATE_INSTANCIATE_RESOURCE(CubeMap            );
    TEMPLATE_INSTANCIATE_RESOURCE(FrameBuffer        );
    TEMPLATE_INSTANCIATE_RESOURCE(IndexBuffer        );
    TEMPLATE_INSTANCIATE_RESOURCE(RenderBuffer       );
    TEMPLATE_INSTANCIATE_RESOURCE(Shader             );
    TEMPLATE_INSTANCIATE_RESOURCE(Texture            );
    TEMPLATE_INSTANCIATE_RESOURCE(VertexArray        );
    TEMPLATE_INSTANCIATE_RESOURCE(VertexBuffer       );
    TEMPLATE_INSTANCIATE_RESOURCE(ShaderStorageBuffer);
    TEMPLATE_INSTANCIATE_RESOURCE(ComputeShader      );
    TEMPLATE_INSTANCIATE_RESOURCE(Material           );
    TEMPLATE_INSTANCIATE_RESOURCE(Mesh               );
    TEMPLATE_INSTANCIATE_RESOURCE(AudioBuffer        );
    TEMPLATE_INSTANCIATE_RESOURCE(AudioPlayer        );
    TEMPLATE_INSTANCIATE_RESOURCE(BoxShape           );
    TEMPLATE_INSTANCIATE_RESOURCE(SphereShape        );
    TEMPLATE_INSTANCIATE_RESOURCE(CylinderShape      );
    TEMPLATE_INSTANCIATE_RESOURCE(CapsuleShape       );
    TEMPLATE_INSTANCIATE_RESOURCE(CompoundShape      );
    TEMPLATE_INSTANCIATE_RESOURCE(NativeRigidBody    );
    TEMPLATE_INSTANCIATE_RESOURCE(MxObject           );

    TEMPLATE_INSTANCIATE_COMPONENT(CameraController   );
    TEMPLATE_INSTANCIATE_COMPONENT(InputController    );
    TEMPLATE_INSTANCIATE_COMPONENT(RigidBody          );
    TEMPLATE_INSTANCIATE_COMPONENT(CharacterController);
    TEMPLATE_INSTANCIATE_COMPONENT(BoxCollider        );
    TEMPLATE_INSTANCIATE_COMPONENT(Script             );
    TEMPLATE_INSTANCIATE_COMPONENT(Behaviour          );
    TEMPLATE_INSTANCIATE_COMPONENT(InstanceFactory    );
    TEMPLATE_INSTANCIATE_COMPONENT(Instance           );
    TEMPLATE_INSTANCIATE_COMPONENT(Skybox             );
    TEMPLATE_INSTANCIATE_COMPONENT(DebugDraw          );
    TEMPLATE_INSTANCIATE_COMPONENT(MeshRenderer       );
    TEMPLATE_INSTANCIATE_COMPONENT(MeshSource         );
    TEMPLATE_INSTANCIATE_COMPONENT(MeshLOD            );
    TEMPLATE_INSTANCIATE_COMPONENT(ParticleSystem     );
    TEMPLATE_INSTANCIATE_COMPONENT(DirectionalLight   );
    TEMPLATE_INSTANCIATE_COMPONENT(PointLight         );
    TEMPLATE_INSTANCIATE_COMPONENT(SpotLight          );
    TEMPLATE_INSTANCIATE_COMPONENT(CameraEffects      );
    TEMPLATE_INSTANCIATE_COMPONENT(CameraSSR          );
    TEMPLATE_INSTANCIATE_COMPONENT(CameraSSGI         );
    TEMPLATE_INSTANCIATE_COMPONENT(CameraSSAO         );
    TEMPLATE_INSTANCIATE_COMPONENT(CameraGodRay       );
    TEMPLATE_INSTANCIATE_COMPONENT(CameraToneMapping  );
    TEMPLATE_INSTANCIATE_COMPONENT(VRCameraController );
    TEMPLATE_INSTANCIATE_COMPONENT(AudioSource        );
    TEMPLATE_INSTANCIATE_COMPONENT(AudioListener      );
    TEMPLATE_INSTANCIATE_COMPONENT(SphereCollider     );
    TEMPLATE_INSTANCIATE_COMPONENT(CylinderCollider   );
    TEMPLATE_INSTANCIATE_COMPONENT(CapsuleCollider    );
    TEMPLATE_INSTANCIATE_COMPONENT(CompoundCollider   );

    void RegisterComponents()
    {
        RegisterComponent<CameraController   >();
        RegisterComponent<InputController    >();
        RegisterComponent<MeshSource         >();
        RegisterComponent<BoxCollider        >();
        RegisterComponent<SphereCollider     >();
        RegisterComponent<CylinderCollider   >();
        RegisterComponent<CapsuleCollider    >();
        RegisterComponent<CompoundCollider   >();
        RegisterComponent<RigidBody          >();
        RegisterComponent<CharacterController>();
        RegisterComponent<Script             >();
        RegisterComponent<Behaviour          >();
        RegisterComponent<InstanceFactory    >();
        RegisterComponent<Instance           >();
        RegisterComponent<Skybox             >();
        RegisterComponent<DebugDraw          >();
        RegisterComponent<MeshRenderer       >();
        RegisterComponent<MeshLOD            >();
        RegisterComponent<ParticleSystem     >();
        RegisterComponent<DirectionalLight   >();
        RegisterComponent<PointLight         >();
        RegisterComponent<SpotLight          >();
        RegisterComponent<CameraEffects      >();
        RegisterComponent<CameraSSR          >();
        RegisterComponent<CameraSSGI         >();
        RegisterComponent<CameraSSAO         >();
        RegisterComponent<CameraGodRay       >();
        RegisterComponent<CameraToneMapping  >();
        RegisterComponent<VRCameraController >();
        RegisterComponent<AudioSource        >();
        RegisterComponent<AudioListener      >();
    }
}