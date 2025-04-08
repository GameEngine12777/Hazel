#pragma once

#include "RenderCommand.h"

namespace Hazel {

	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();

		/** 提交 VAO */
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};


}
