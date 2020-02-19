// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in sourceand binary forms, with or without
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
#include "Core/Object/Object.h"
#include <unordered_map>

namespace MxEngine
{
    enum class RenderEngine
    {
        OpenGL,
    };

    class Application
    {
        struct ModuleManager
        {
            ModuleManager(Application* app);
            ~ModuleManager();
        } manager;
	private:
		using ObjectStorage = std::unordered_map<std::string, UniqueRef<Object>>;
		UniqueRef<Window> window;
		ObjectStorage objects;
		Object defaultInstance;
        RenderController renderer;
        bool shouldClose = false;
		bool debugMeshDraw = false;

        void CreateConsoleBindings(DeveloperConsole& console);
		void DrawObjects(bool meshes) const;
		void InvokeUpdate();
	protected:
		AppEventDispatcher Dispatcher;
        DeveloperConsole Console;
		std::string ResourcePath;
		TimeStep TimeDelta;
		int CounterFPS;

		Application();

		void ToggleMeshDrawing(bool state = true);

		virtual void OnCreate () = 0;
		virtual void OnUpdate () = 0;
		virtual void OnDestroy() = 0;

		void CreateContext();
	public:
		Ref<RenderObjectContainer> LoadObjectBase(const std::string& filepath);
		Object& CreateObject(const std::string& name, const std::string& path);
		Object& AddObject(const std::string& name, UniqueRef<Object> object);
		Ref<Texture> CreateTexture(const std::string& filepath, bool genMipmaps = true, bool flipImage = true);
		Ref<Shader> CreateShader(const std::string& vertexShaderPath, const std::string fragmentShaderPath);
		Object& GetObject(const std::string& name);
		void DestroyObject(const std::string& name);
		void ToggleDeveloperConsole(bool isVisible);

		AppEventDispatcher& GetEventDispatcher();
		RenderController& GetRenderer();
		Window& GetWindow();
		float GetTimeDelta() const;
		void Run();
		void CloseApplication();
		virtual ~Application();
	};

    using Context = SingletonHolder<Application*>;
}