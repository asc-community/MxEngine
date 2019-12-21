#pragma once

#include "Utilities/SingletonHolder/SingletonHolder.h"

namespace MomoEngine
{
	class GLInitilizerImpl
	{
	public:
		GLInitilizerImpl();
		~GLInitilizerImpl();
		GLInitilizerImpl(const GLInitilizerImpl&) = delete;
		GLInitilizerImpl(GLInitilizerImpl&&) = delete;
		void IntializeWindow();
		void OnWindowCreate();
	};

	using GLInitilizer = SingletonHolder<GLInitilizerImpl>;
}