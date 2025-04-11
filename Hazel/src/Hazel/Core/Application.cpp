#include "hzpch.h"
#include "Application.h"
#include "Hazel/Core/Log.h"

#include "Hazel/ImGui/ImGuiLayer.h"

#include "Input.h"

#include "Hazel/Renderer/Renderer.h"

#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/VertexArray.h"
#include "KeyCodes.h"
#include <GLFW/glfw3.h>

namespace Hazel
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		// 创建 窗口（不同得平台实例化得对象将不同，具体看平台 Create 实现）
		m_Window = std::unique_ptr<Window>(Window::Create());

		// 绑定执行事件
		m_Window->SetEventCallback(HZ_BIND_EVENT_FN(&Application::OnEvent));

		Renderer::Init();

		// 创建 imgui 图层
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			/**
			* 更新图层
			* 处理渲染时，应该先画最远的 Layer，再画最近的 Layer
			*/
			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			/**
			* 
			*/
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			// 更新视口
			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		/**
		* 以下列出各种事件的处理函数
		* 如果 e 是 WindowCloseEvent，那么这里传入的函数会被调用
		* 再列出其他情况的，例： 
		* 如果 e 是 WindowXXXEvent，那么这里传入的函数会被调用
		* dispatcher.Dispatch<WindowXXXEvent>(BIND_EVENT_FN(OnWindowXXX));
		*/
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(&Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(&Application::OnWindowResize));

		//if (e.GetEventType() == Hazel::EventType::KeyPressed)
		//{
		//	Hazel::KeyPressedEvent& Te = (Hazel::KeyPressedEvent&)e;
		//	if (Te.GetKeyCode() == HZ_KEY_A)
		//	{
		//		m_Camera.SetPosition({ -0.9f, 0.f, .0f });
		//		m_Camera.SetRotation(135.0f);
		//	}
		//}

		/**
		* 事件传递
		* 处理事件时正好相反，因为最上面一层的Layer才应该是接受event的对象
		*/
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			/**
			* 反向传递事件
			*/
			(*--it)->OnEvent(e);

			/**
			* 如果当前图层消耗掉了事件，将不再向下传递
			*/
			if (e.Handled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}