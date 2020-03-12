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
#include "Utilities/GenericStorage/GenericStorage.h"
#include "Core/Interfaces/IEvent.h"
#include "Core/DeveloperConsole/DeveloperConsole.h"
#include "Core/Interfaces/GraphicAPI/Window.h"
#include "Core/RenderController/RenderController.h"
#include "Core/MxObject/MxObject.h"
#include "Core/ResourceStorage/ResourceStorage.h"
#include "Utilities/LifetimeManager/LifetimeManager.h"
#include "Utilities/Counter/Counter.h"
#include "Utilities/FileSystem/FileSystem.h"

namespace MxEngine
{
	class LoggerImpl;

	class Application
	{
		struct ModuleManager
		{
			ModuleManager(Application* app);
			~ModuleManager();
		} manager;

		using ObjectStorage = LifetimeManager<UniqueRef<MxObject>>;
		using AppResourceManager = GenericStorage<ResourceStorage, Texture, Mesh, Shader>;
	private:
		static inline Application* Current = nullptr;

		UniqueRef<AppResourceManager> resourceManager = MakeUnique<AppResourceManager>();
		UniqueRef<Window> window;
		ObjectStorage objects;
		MxObject defaultInstance;
		RenderController renderer;
		AppEventDispatcher dispatcher;
		DeveloperConsole console;
		Counter resourceIdCounter;
		bool shouldClose = false;
		bool debugMeshDraw = false;

		void CreateConsoleBindings(DeveloperConsole& console);
		void DrawObjects(bool meshes) const;
		void InvokeUpdate();
	protected:
		FilePath ResourcePath;
		TimeStep TimeDelta;
		int CounterFPS;

		Application();

		virtual void OnCreate();
		virtual void OnUpdate();
		virtual void OnDestroy();

		void CreateContext();
	public:

		Mesh* LoadMesh(const FilePath& filepath);
		MxObject& CreateObject(const std::string& name, const FilePath& path);
		MxObject& AddObject(const std::string& name, UniqueRef<MxObject> object);
		MxObject& CopyObject(const std::string& name, const std::string& existingObject);
		MxObject& GetObject(const std::string& name);
		void DestroyObject(const std::string& name);
		const ObjectStorage::Storage& GetObjectList();
		void InvalidateObjects();
		Texture* CreateTexture(const FilePath& texture, bool genMipmaps = true, bool flipImage = true);
		Shader* CreateShader(const FilePath& vertex, const FilePath& fragment);
		void ExecuteScript(const FilePath& script);

		void ToggleDeveloperConsole(bool isVisible);
		void ToggleMeshDrawing(bool state = true);

		AppEventDispatcher& GetEventDispatcher();
		RenderController& GetRenderer();
		LoggerImpl& GetLogger();
		DeveloperConsole& GetConsole();
		Window& GetWindow();
		template<typename Resource>
		AppResourceManager::ConcreteStorage<Resource>& GetResourceManager();
		Counter::CounterType GenerateResourceId();
		float GetTimeDelta() const;
		void Run();
		void CloseApplication();
		virtual ~Application();

		static Application* Get();
		static void Set(Application* application);
	};

	template<typename Resource>
	inline Application::AppResourceManager::ConcreteStorage<Resource>& Application::GetResourceManager()
	{
		return this->resourceManager->Get<Resource>();
	}
}