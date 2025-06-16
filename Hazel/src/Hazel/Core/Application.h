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

	/**
	 * 封装命令行参数的结构体，传递给 Application 使用。
	 */
	struct ApplicationCommandLineArgs
	{
		/** 参数数量（通常为 argc） */
		int Count = 0;
		/** 参数数组（通常为 argv） */
		char** Args = nullptr;

		/** 通过下标访问参数 */
		const char* operator[](int index) const
		{
			/** 断言防止越界访问 */
			HZ_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	/** 描述应用程序的基本信息 */
	struct ApplicationSpecification
	{
		/** 应用程序的名字 */
		std::string Name = "Hazel Application";
		/** 工作目录（可用于资源加载） */
		std::string WorkingDirectory;
		/** 命令行参数 */
		ApplicationCommandLineArgs CommandLineArgs;
	};

	/** 应用程序主类，封装窗口、事件循环、层系统等 */
	class Application
	{
	public:
		/** 获取当前运行的全局 Application 实例（单例模式） */
		inline static Application& Get() { return *s_Instance; }

	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		/** 应用主循环，负责驱动引擎运行（帧更新、事件分发等） */
		void Run();

		/**
		 * 处理事件分发入口，接收来自 GLFW 的事件并派发给各个层
		 * 利用 Event 的基类统一处理鼠标、键盘、窗口事件
		 */
		void OnEvent(Event& e);

		/** 添加一个普通层（LayerStack 底部插入：场景绘制... ） */
		void PushLayer(Layer* layer);
		/** 添加一个覆盖层（Overlay：UI、Debug 面板等，插入顶部） */
		void PushOverlay(Layer* layer);

		/** 获取窗口引用（例如用于设置VSync、获取大小等） */
		inline Window& GetWindow() { return *m_Window; }

		/** 主动关闭应用程序 */
		void Close();

		/** 获取 ImGui 层的指针（用于绘制 UI） */
		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		/** 获取应用程序初始化时的设定 */
		const ApplicationSpecification& GetSpecification() const { return m_Specification; }

	private:
		/** 处理窗口关闭事件（返回 true 表示事件已处理） */
		bool OnWindowClose(WindowCloseEvent& e);
		/** 处理窗口大小调整事件，判断是否最小化 */
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		/** 应用程序的初始化配置 */
		ApplicationSpecification m_Specification;

		/** 应用主窗口（封装 GLFW 或 Windows API） */
		Scope<Window> m_Window;
		/** ImGui 层指针 */
		ImGuiLayer* m_ImGuiLayer;
		/** 应用是否仍在运行 */
		bool m_Running = true;
		/** 应用是否处于最小化状态（影响渲染更新） */
		bool m_Minimized = false;
		/** 层堆栈，管理所有 Layer 和 Overlay */
		LayerStack m_LayerStack;

		/** 上一帧的时间戳，用于计算 DeltaTime */
		float m_LastFrameTime = 0.0f;

	private:
		/** 全局 Application 实例（单例模式） */
		static Application* s_Instance;
	};

	// To be defined in Client
	Application* CreateApplication(ApplicationCommandLineArgs args);
}

