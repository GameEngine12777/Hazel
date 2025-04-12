#pragma once

#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel {

	/**
	* 渲染指令
	* Renderer/Renderer2D 等都统一调用这个模块进行渲染指令下发
	* RenderCommand 中包含了我们自己封装的 RendererAPI, 而 RendererAPI 中则又封装了不同平台的渲染命令
	*/
	class RenderCommand
	{
	public:
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		/**
		* 绘制
		*/
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, count);
		}
	private:
		static Scope<RendererAPI> s_RendererAPI;
	};

}
