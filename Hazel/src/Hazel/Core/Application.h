#pragma once

#include "Hazel/Core/Base.h"
#include "Hazel/Core/Window.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Core/LayerStack.h"

namespace Hazel {
	class ImGuiLayer;
	class Shader;
	class VertexBuffer;
	class IndexBuffer;
	class VertexArray;

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			HZ_CORE_ASSERT(index < Count, "");
			return Args[index];
		}
	};

	class Application
	{
	public:
		Application(const std::string& name = "Hazel App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());;
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

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }

		ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		ApplicationCommandLineArgs m_CommandLineArgs;
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	// To be defined in Client
	Application* CreateApplication(ApplicationCommandLineArgs args);
}

