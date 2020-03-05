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

#include "Utilities/Time/Time.h"
#include "Utilities/SingletonHolder/SingletonHolder.h"
#include "Core/Interfaces/IEvent.h"
#include "Core/DeveloperConsole/DeveloperConsole.h"
#include "Core/Interfaces/GraphicAPI/Window.h"
#include "Core/RenderController/RenderController.h"
#include "Core/MxObject/MxObject.h"
#include <unordered_map>

#include <filesystem>

namespace MxEngine
{
	enum class RenderEngine
	{
		OpenGL,
	};

	class LoggerImpl;

	class Application
	{
		struct ModuleManager
		{
			ModuleManager(Application* app);
			~ModuleManager();
		} manager;

		using ObjectStorage = LifetimeManager<UniqueRef<MxObject>>;
	private:
		UniqueRef<Window> window;
		ObjectStorage objects;
		MxObject defaultInstance;
		RenderController renderer;
		AppEventDispatcher dispatcher;
		DeveloperConsole console;
		bool shouldClose = false;
		bool debugMeshDraw = false;

		void CreateConsoleBindings(DeveloperConsole& console);
		void DrawObjects(bool meshes) const;
		void InvokeUpdate();
	protected:
		std::filesystem::path ResourcePath;
		TimeStep TimeDelta;
		int CounterFPS;

		Application();

		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDestroy();

		void CreateContext();
	public:
		Ref<RenderObjectContainer> LoadObjectBase(const std::filesystem::path& filepath);
		MxObject& CreateObject(const std::string& name, const std::filesystem::path& path);
		MxObject& AddObject(const std::string& name, UniqueRef<MxObject> object);
		MxObject& CopyObject(const std::string& name, const std::string& existingObject);
		const ObjectStorage::Storage& GetObjectList();
		void InvalidateObjects();
		Ref<Texture> CreateTexture(const std::filesystem::path& texture, bool genMipmaps = true, bool flipImage = true);
		Ref<Shader> CreateShader(const std::filesystem::path& vertex, const std::filesystem::path& fragment);
		MxObject& GetObject(const std::string& name);
		void DestroyObject(const std::string& name);
		void ToggleDeveloperConsole(bool isVisible);
		void ToggleMeshDrawing(bool state = true);

		AppEventDispatcher& GetEventDispatcher();
		RenderController& GetRenderer();
		LoggerImpl& GetLogger();
		DeveloperConsole& GetConsole();
		Window& GetWindow();
		float GetTimeDelta() const;
		void Run();
		void CloseApplication();
		virtual ~Application();
	};

	using Context = SingletonHolder<Application*>;
}