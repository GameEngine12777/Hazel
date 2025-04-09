#include "hzpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Hazel {

	/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		/**
		* 创建 VBO(顶点缓冲对象，Vertex Buffer Object)
		* VBO（Vertex Buffer Object）是 用于存储顶点数据的 GPU 缓冲区，它的作用是将顶点数据存储在显存（VRAM）中，从而提高渲染性能。
		* 在 OpenGL 早期，顶点数据是存储在 CPU 内存中的，每次绘制时都需要从 CPU 传输到 GPU，这样会严重影响性能。而 VBO 将数据一次性存入 GPU，然后在渲染时直接使用，大大提升了渲染效率。
		* 存储在显存（GPU），提高渲染性能
		* 避免 CPU-GPU 之间的频繁数据传输
		* 支持动态更新数据（GL_DYNAMIC_DRAW）
		*/
		// 创建一个 VBO，并将 ID 存储在 VBO 变量中。
		glCreateBuffers(1, &m_RendererID);

		// 绑定 VBO 并设置顶点属性
		// GL_ARRAY_BUFFER：表示当前绑定的是顶点缓冲对象（VBO）。
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

		/**
		* glBufferData 用于将数据传输到 GPU：
		* https://chatgpt.com/c/67f5e9b8-01b4-8007-a6ac-a1dd5241578f
		* 第一个参数：GL_ARRAY_BUFFER，表示数据用于顶点数组。
		* 第二个参数：数据大小（sizeof(vertices)）。
		* 第三个参数：数据指针（vertices）。
		* 第四个参数：GL_STATIC_DRAW，表示数据是静态的，只会传输一次，之后多次使用（如果数据会频繁修改，使用 GL_DYNAMIC_DRAW）。
		*/
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

		/**
		* 显式进行对某个 ID 进行数据绑定操作
		* 功能同上面两步
		* 第一步：glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		* 第二步：glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		*/
		// glNamedBufferData(m_RendererID, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/////////////////////////////////////////////////////////////////////////////
	// IndexBuffer //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		/**
		* 创建 EBO 并绑定索引数据(索引缓冲对象，Element Buffer Object)
		* 生成并绑定索引缓冲区。
		*
		* 使用 glDrawElements() 结合 EBO 可以用索引方式绘制图形，而不是简单的 glDrawArrays() 按顺序绘制。
		*/
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);

		// 提供索引数据，定义三角形如何连接顶点。
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}