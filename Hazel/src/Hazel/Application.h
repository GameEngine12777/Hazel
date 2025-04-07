#pragma once

#include "Core.h"
#include "Window.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "LayerStack.h"

namespace Hazel {
	class ImGuiLayer;
	class Shader;
	class VertexBuffer;
	class IndexBuffer;
	class VertexArray;

	class HAZEL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		/**
		* 用来绑定 GLFW 窗口回调事件，然后分发到我们的 App 中
		* 这里用 Event 的基类来接受各种事件的派生类的对象
		*/
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<Shader> m_BlueShader;
		std::shared_ptr<VertexArray> m_SquareVA;
	private:
		static Application* s_Instance;
	};

	// To be defined in Client
	Application* CreateApplication();
}

