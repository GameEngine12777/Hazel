#pragma once

#include "Hazel/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel {
	/**
	* 初始化 OpenGL 得渲染上下文
	*/
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_WindowHandle;
	};

}