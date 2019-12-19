#pragma once

namespace MomoEngine
{
	struct IApplication
	{
	protected:
		virtual void OnCreate() = 0;
		virtual void OnUpdate() = 0;
		virtual ~IApplication() = default;
	};
}