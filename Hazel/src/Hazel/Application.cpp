#include "hzpch.h"
#include "Application.h"
#include "Hazel/Log.h"

#include "Hazel/ImGui/ImGuiLayer.h"

#include "Input.h"

#include "Hazel/Renderer/Renderer.h"

#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/VertexArray.h"
#include "KeyCodes.h"

namespace Hazel
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
		: m_Camera(-1.f, 1.f, -1.f, 1.f)
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

		m_VertexArray.reset(VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.f, 0.f, 0.f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.f, 1.f, 0.f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.f, 0.f, 1.f, 1.0f
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(VertexArray::Create());

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};

		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			// 声明 uniform 变量，后期可以通过 glGetUniformLocation() 函数进行 GPU 寻址并设置值
			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);	
			}
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_BlueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	void Application::Run()
	{
		while (m_Running)
		{
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			m_Camera.SetPosition({ 0.9f, 0.9f, .0f });
			m_Camera.SetRotation(.0f);

			Renderer::BeginScene(m_Camera);

			Renderer::Submit(m_BlueShader, m_SquareVA);
			Renderer::Submit(m_Shader, m_VertexArray);

			Renderer::EndScene();

			/**
			* 更新图层
			* 处理渲染时，应该先画最远的 Layer，再画最近的 Layer
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
}