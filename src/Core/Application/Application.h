#pragma once

#include "Utilities/Time/Time.h"
#include "Utilities/SingletonHolder/SingletonHolder.h"
#include "Core/Interfaces/IEvent.h"
#include "Core/DeveloperConsole/DeveloperConsole.h"
#include "Core/Interfaces/GraphicAPI/Window.h"
#include "Core/RenderController/RenderController.h"
#include "Core/BaseObject/BaseObject.h"
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
		using ObjectStorage = std::unordered_map<std::string, ObjectInstance>;
		UniqueRef<Window> window;
		ObjectStorage objects;
		ObjectInstance defaultInstance;
        RenderController renderer;
        bool shouldClose = false;

        void CreateConsoleBindings(DeveloperConsole& console);
	protected:
		AppEventDispatcher Dispatcher;
        DeveloperConsole Console;
		std::string ResourcePath;
		TimeStep TimeDelta;
		int CounterFPS;

		Application();

		void SetWindow(UniqueRef<Window> window);
		ObjectInstance& CreateObject(const std::string& name, const std::string& path);
        Ref<Texture> CreateTexture(const std::string& filepath, bool genMipmaps = true, bool flipImage = true);
        Ref<Shader> CreateShader(const std::string& vertexShaderPath, const std::string fragmentShaderPath);
		ObjectInstance& GetObject(const std::string& name);
		void DestroyObject(const std::string& name);
        void ToggleDeveloperConsole(bool isVisible);

		void DrawObjects(bool meshes = false) const;

		virtual void OnCreate () = 0;
		virtual void OnUpdate () = 0;
		virtual void OnDestroy() = 0;

		void CloseApplication();
		void CreateContext();
	public:
		AppEventDispatcher& GetEventDispatcher();
		RenderController& GetRenderer();
		Window& GetWindow();
		TimeStep GetTimeDelta() const;
		void Run();
		virtual ~Application();
	};

    using Context = SingletonHolder<Application*>;
}