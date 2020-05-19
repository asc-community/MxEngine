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

#include "Core/Application/Application.h"
#include "Core/Event/Events/FpsUpdateEvent.h"
#include "Utilities/FileSystem/FileManager.h"

namespace MxEngine
{
	class ShaderBinding
	{
		std::string handle;
		Shader** shader;
	public:
		ShaderBinding(const std::string& eventHandle, Shader** shader = nullptr)
			: handle(eventHandle), shader(shader)
		{
			
		}

		inline void Bind(const std::string& vertex, const std::string& fragment)
		{
			auto vertexhash = MakeStringId(vertex);
			auto fragmenthash = MakeStringId(fragment);
			if (shader == nullptr) return;
			if (!FileModule::FileExists(vertexhash))
			{
				Logger::Instance().Error("MxEngine::ShaderBinding", "file does not exists: " + vertex);
				return;
			}
			if (!FileModule::FileExists(fragmenthash))
			{
				Logger::Instance().Error("MxEngine::ShaderBinding", "file does not exists: " + fragment);
				return;
			}

			auto context = Application::Get();
			context->GetEventDispatcher().AddEventListener(handle, 
			[shader = shader, name = "MxShader" + handle, vertex = vertex, fragment = fragment,
				vertexTime = FileSystemTime(), fragmentTime = FileSystemTime()] (FpsUpdateEvent& e) mutable
			{
				auto newVertexTime   = File::LastModifiedTime(FileModule::GetFilePath(MakeStringId(vertex)));
				auto newFragmentTime = File::LastModifiedTime(FileModule::GetFilePath(MakeStringId(fragment)));
				if (vertexTime < newVertexTime || fragmentTime < newFragmentTime)
				{
					*shader = Application::Get()->GetCurrentScene().LoadShader(name, vertex, fragment);
					Logger::Instance().Debug("MxEngine::ShaderBinding", "updated shader: " + name);

					vertexTime   = std::move(newVertexTime);
					fragmentTime = std::move(newFragmentTime);
				}
			});
		}

		inline void Bind(const std::string& vertex, const std::string& geometry, const std::string& fragment)
		{
			auto& directory = Application::Get()->GetCurrentScene().GetDirectory();
			if (shader == nullptr) return;
			auto vertexhash = MakeStringId(vertex);
			auto geometryhash = MakeStringId(geometry);
			auto fragmenthash = MakeStringId(fragment);
			if (!FileModule::FileExists(vertexhash))
			{
				Logger::Instance().Error("MxEngine::ShaderBinding", "file does not exists: " + vertex);
				return;
			}
			if (!FileModule::FileExists(geometryhash))
			{
				Logger::Instance().Error("MxEngine::ShaderBinding", "file does not exists: " + geometry);
				return;
			}
			if (!FileModule::FileExists(fragmenthash))
			{
				Logger::Instance().Error("MxEngine::ShaderBinding", "file does not exists: " + fragment);
				return;
			}

			auto context = Application::Get();
			context->GetEventDispatcher().AddEventListener(handle,
				[shader = shader, name = "MxShader" + handle, vertex = vertex, fragment = fragment, geometry = geometry,
				vertexTime = FileSystemTime(), geometryTime = FileSystemTime(), fragmentTime = FileSystemTime()](FpsUpdateEvent& e) mutable
			{
				auto newVertexTime   = File::LastModifiedTime(FileModule::GetFilePath(MakeStringId(vertex)));
				auto newGeometryTime = File::LastModifiedTime(FileModule::GetFilePath(MakeStringId(geometry)));
				auto newFragmentTime = File::LastModifiedTime(FileModule::GetFilePath(MakeStringId(fragment)));
				if (vertexTime < newVertexTime || geometryTime < newGeometryTime || fragmentTime < newFragmentTime)
				{
					*shader = Application::Get()->GetCurrentScene().LoadShader(name, vertex, geometry, fragment);
					Logger::Instance().Debug("MxEngine::ShaderBinding", "updated shader: " + name);

					vertexTime   = std::move(newVertexTime);
					geometryTime = std::move(newGeometryTime);
					fragmentTime = std::move(newFragmentTime);
				}
			});
		}

		inline void Unbind()
		{
			auto context = Application::Get();
			context->GetEventDispatcher().RemoveEventListener(handle);
		}
	};
}