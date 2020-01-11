#pragma once

#include "Core/Window/Window.h"
#include "Core/Renderer/Renderer.h"
#include "Utilities/Time/Time.h"
#include "Utilities/ImGuiConsole/ImGuiConsole.h"
#include <unordered_map>

namespace MomoEngine
{
	struct IApplication
	{
		using ObjectStorage = std::unordered_map<std::string, GLInstance>;
		ObjectStorage objects;
		GLInstance defaultInstance;
		chaiscript::ChaiScript* chaiScript;
		friend class ChaiScriptApplication;
	protected:
		ImGuiConsole Console;
		std::string ResourcePath;
		Window Window;
		TimeStep TimeDelta;
		int CounterFPS;
		IApplication();

		RendererImpl& GetRenderer();
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
		void Run();
		virtual ~IApplication();
	};

	class ChaiScriptApplication
	{
	public:
		static void Init(chaiscript::ChaiScript& chai, IApplication* app);
	};
}