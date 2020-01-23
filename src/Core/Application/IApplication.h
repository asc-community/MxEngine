#pragma once

#include "Core/Window/Window.h"
#include "Core/Renderer/Renderer.h"
#include "Utilities/Time/Time.h"
#include "Utilities/ImGuiConsole/ImGuiConsole.h"
#include "Core/Interfaces/IEvent.h"
#include <unordered_map>

namespace MomoEngine
{
	struct IApplication
	{
	private:
		using ObjectStorage = std::unordered_map<std::string, GLInstance>;
		Window window;
		ObjectStorage objects;
		GLInstance defaultInstance;
		chaiscript::ChaiScript* chaiScript;
		friend class ChaiScriptApplication;
	protected:
		AppEventDispatcher Dispatcher;
		ImGuiConsole Console;
		std::string ResourcePath;
		TimeStep TimeDelta;
		int CounterFPS;
		IApplication();

		void SetWindow(Window window);
		GLInstance& CreateObject(const std::string& name, const std::string& path);
		GLInstance& GetObject(const std::string& name);
		void DestroyObject(const std::string& name);

		void DrawObjects(bool meshes = false) const;

		virtual void OnCreate () = 0;
		virtual void OnUpdate () = 0;
		virtual void OnDestroy() = 0;

		void CloseApplication();
		void CreateDefaultContext();
	public:
		AppEventDispatcher& GetEventDispatcher();
		RendererImpl& GetRenderer();
		Window& GetWindow();
		TimeStep GetTimeDelta() const;
		void Run();
		virtual ~IApplication();
	};

	class ChaiScriptApplication
	{
	public:
		static void Init(chaiscript::ChaiScript& chai, IApplication* app);
	};
}