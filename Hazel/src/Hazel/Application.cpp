#include "hzpch.h"
#include "Application.h"
#include "Hazel/Log.h"

#include "Hazel/ImGui/ImGuiLayer.h"

#include "Input.h"

#include <glad/glad.h>

#include "Hazel/Renderer/Shader.h"

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

		/**
		* https://chatgpt.com/c/67ee026c-e4cc-8007-97b2-68af3048c6a5
		* 创建 VAO(顶点数组对象，Vertex Array Object)
		* VAO 的主要作用是 存储顶点属性的配置
		* 顶点数据的格式（比如顶点坐标、颜色、法线等）
		* VBO（顶点缓冲对象）的绑定状态
		* EBO（索引缓冲对象）的绑定状态
		* VAO 记录了“如何解释 VBO 里的数据”，这样就不需要每次绘制都重新配置数据格式。
		*/
		glGenVertexArrays(1, &m_VertexArray);
		// 绑定 VAO
		glBindVertexArray(m_VertexArray);

		// 
		/**
		* 创建 VBO(顶点缓冲对象，Vertex Buffer Object)
		* VBO（Vertex Buffer Object）是 用于存储顶点数据的 GPU 缓冲区，它的作用是将顶点数据存储在显存（VRAM）中，从而提高渲染性能。
		* 在 OpenGL 早期，顶点数据是存储在 CPU 内存中的，每次绘制时都需要从 CPU 传输到 GPU，这样会严重影响性能。而 VBO 将数据一次性存入 GPU，然后在渲染时直接使用，大大提升了渲染效率。
		* 存储在显存（GPU），提高渲染性能
		* 避免 CPU-GPU 之间的频繁数据传输
		* 支持动态更新数据（GL_DYNAMIC_DRAW）
		*/
		// 创建一个 VBO，并将 ID 存储在 VBO 变量中。
		glGenBuffers(1, &m_VertexBuffer);
		// 绑定 VBO 并设置顶点属性
		// GL_ARRAY_BUFFER：表示当前绑定的是顶点缓冲对象（VBO）。
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		// 三角形的 3D 坐标数据
		//float vertices[3 * 3] = {
		//	-0.5f, -0.5f, 0.0f,
		//	 0.5f, -0.5f, 0.0f,
		//	 0.0f,  0.5f, 0.0f
		//};

		// 四边形 坐标
		float vertices[] = {
			-0.5f, -0.5f,  // 左下角
			 0.5f, -0.5f,  // 右下角
			 0.5f,  0.5f,  // 右上角
			-0.5f,  0.5f   // 左上角
		};
		/**
		* glBufferData 用于将数据传输到 GPU：
		* 第一个参数：GL_ARRAY_BUFFER，表示数据用于顶点数组。
		* 第二个参数：数据大小（sizeof(vertices)）。
		* 第三个参数：数据指针（vertices）。
		* 第四个参数：GL_STATIC_DRAW，表示数据是静态的，只会传输一次，之后多次使用（如果数据会频繁修改，使用 GL_DYNAMIC_DRAW）。
		*/
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		/**
		* 创建 EBO 并绑定索引数据(索引缓冲对象，Element Buffer Object)
		* 生成并绑定索引缓冲区。
		* 1.避免重复存储顶点数据，减少内存占用
		* 2.优化绘制性能，避免重复处理相同顶点
		* 3.允许绘制复杂的几何形状（如立方体、网格等）
		* 
		* 使用 glDrawElements() 结合 EBO 可以用索引方式绘制图形，而不是简单的 glDrawArrays() 按顺序绘制。
		*/
		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		// 提供索引数据，定义三角形如何连接顶点。
		// unsigned int indices[3] = { 0, 1, 2 };
		unsigned int indices[] = {
			0, 1, 2,  // 第一个三角形
			2, 3, 0   // 第二个三角形
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		/**
		* VBO 里的数据格式：
		* 每个顶点包含 3 个 GL_FLOAT 值，不需要归一化，步长 3 * sizeof(float)。
		* 0：属性索引，代表这个是“位置”属性。
		* 3：表示每个顶点由 3 个 float 组成（x, y, z）。
		* GL_FLOAT：数据类型。
		* GL_FALSE：不进行归一化。
		* 3 * sizeof(float)：步长（每个顶点占 3 个 float）。
		* (void*)0：偏移量（数据从 0 开始）。
		*/
		// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		// 设置顶点属性指针(顶点属性 0（索引 0）)
		glEnableVertexAttribArray(0);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(v_Position * 10000 + 0.5, 1.0);
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
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

			m_Shader->Bind();

			glBindVertexArray(m_VertexArray);
			/**
			* 参数解释：
			* 1.绘制图元类型（决定如何连接顶点）
			* 2.要绘制的索引数量
			* 3.索引数据类型
			* 4.索引数组的起始位置（如果已绑定 EBO，传 0 即可）
			*/
			// glDrawElements(GL_TRIANGLE_STRIP, 3, GL_UNSIGNED_INT, nullptr);
			glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, nullptr);

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