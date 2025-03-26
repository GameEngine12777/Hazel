#include "hzpch.h"
#include "Application.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Log.h"

#include <GLFW/glfw3.h>

Hazel::Application::Application()
{
	m_Window = std::unique_ptr<Window>(Window::Create());
}

Hazel::Application::~Application()
{
}

void Hazel::Application::Run()
{
	WindowResizeEvent e(1280, 720);
	if (e.IsInCategory(EventCategoryApplication))
	{
		HZ_TRACE(e.ToString());
	}
	if (e.IsInCategory(EventCategoryInput))
	{
		HZ_TRACE(e.ToString());
	}

	while (m_Running)
	{
		glClearColor(1, 0, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		m_Window->OnUpdate();
	}
}

Hazel::Application* Hazel::CreateApplication()
{
	return new Application();
}
