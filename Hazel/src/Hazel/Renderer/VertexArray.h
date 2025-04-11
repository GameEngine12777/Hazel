#pragma once

#include <memory>
#include "Hazel/Renderer/Buffer.h"

namespace Hazel {

	/**
	* VAO(顶点数组对象，Vertex Array Object)
	* VAO 的主要作用是 存储顶点属性的配置
	* 顶点数据的格式（比如顶点坐标、颜色、法线等）
	* VBO（顶点缓冲对象）的绑定状态
	* EBO（索引缓冲对象）的绑定状态
	* VAO 记录了“如何解释 VBO 里的数据”，这样就不需要每次绘制都重新配置数据格式。
	* https://chatgpt.com/c/67ee026c-e4cc-8007-97b2-68af3048c6a5
	*/
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		/** 添加顶点缓存数据 */
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		/** 添加顶点索引数据 */
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

		static Ref<VertexArray> Create();
	};

}
