#pragma once

#include "Utilities/SingletonHolder/SingletonHolder.h"

struct GLFWwindow;

namespace MomoEngine
{	
	class GLInitilizerImpl
	{
	public:
		GLInitilizerImpl();
		~GLInitilizerImpl();
		GLInitilizerImpl(const GLInitilizerImpl&) = delete;
		GLInitilizerImpl(GLInitilizerImpl&&) = delete;
		void OnWindowCreate(GLFWwindow* window);
		void OnWindowClose(GLFWwindow* window);
		void OnWindowTick(GLFWwindow* window);
		inline void Init() { };
	};

	using GLInitilizer = SingletonHolder<GLInitilizerImpl>;
}