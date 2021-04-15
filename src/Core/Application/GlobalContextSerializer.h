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
// and /or other materials provided wfith the distribution.
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

#include "Utilities/StaticSerializer/StaticSerializer.h"
#include "Core/Application/Application.h"
#include "Core/MxObject/MxObject.h"
#include "Core/Resources/AssetManager.h"
#include "Core/Runtime/RuntimeCompiler.h"
#include "Core/Serialization/SceneSerializer.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Platform/Modules/PhysicsModule.h"
#include "Platform/Modules/GraphicModule.h"
#include "Platform/Modules/AudioModule.h"
#include "Platform/PhysicsAPI.h"

namespace MxEngine
{
	using GlobalContextSerializer = StaticSerializer<
		Application,
		Logger,
		FileManager,
		AudioModule,
		GraphicModule,
		PhysicsModule,
		UUIDGenerator,
		Factory<CubeMap>,
		Factory<FrameBuffer>,
		Factory<IndexBuffer>,
		Factory<RenderBuffer>,
		Factory<Shader>,
		Factory<Texture>,
		Factory<VertexArray>,
		Factory<VertexBuffer>,
		Factory<ShaderStorageBuffer>,
		Factory<ComputeShader>,
		ComponentFactory,
		Factory<Material>,
		Factory<Mesh>,
		Factory<AudioBuffer>,
		Factory<AudioPlayer>,
		Factory<BoxShape>,
		Factory<SphereShape>,
		Factory<CylinderShape>,
		Factory<CapsuleShape>,
		Factory<CompoundShape>,
		Factory<NativeRigidBody>,
		Factory<MxObject>,
		RuntimeCompiler,
		SceneSerializer
	>;
}