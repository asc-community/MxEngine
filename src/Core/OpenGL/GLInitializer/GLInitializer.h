#pragma once

#include "../Utilities/SingletonHolder/SingletonHolder.h"

namespace MomoEngine
{
	class GLInitilizerImpl
	{
	public:
		GLInitilizerImpl();
		~GLInitilizerImpl();
		void IntializeWindow();
		void OnWindowCreate();
	};

	using GLInitilizer = SingletonHolder<GLInitilizerImpl>;
}