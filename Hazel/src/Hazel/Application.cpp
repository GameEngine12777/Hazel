#include "hzpch.h"
#include "Application.h"
#include "Hazel/Log.h"

#include "Hazel/ImGui/ImGuiLayer.h"

#include "Input.h"

#include <glad/glad.h>

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

		// 创建 imgui 图层
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			/**
			* 创建 VAO
			*/
			glGenVertexArrays(1, &m_VertexArray);
			glBindVertexArray(m_VertexArray);

			glGenBuffers(1, &m_VertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

			float vertices[3 * 3] = {
				-0.5f, -0.5f, 0.0f,
				 0.5f, -0.5f, 0.0f,
				 0.0f,  0.5f, 0.0f
			};

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

			glGenBuffers(1, &m_IndexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

			unsigned int indices[3] = { 0, 1, 2 };
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

			/**
			* 更新图层
			* 处理渲染时，应该先画最远的Layer，再画最近的Layer
			*/
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

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
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}