#pragma once

#include <string>

namespace Hazel {

	/** 定义了在着色器（Shader）中可能使用的数据类型 */
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	/** 返回对应数据类型的的字节大小 */
	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:		return 4;
		case ShaderDataType::Float2:	return 4 * 2;
		case ShaderDataType::Float3:	return 4 * 3;
		case ShaderDataType::Float4:	return 4 * 4;
		case ShaderDataType::Mat3:		return 4 * 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4 * 4;
		case ShaderDataType::Int:		return 4;
		case ShaderDataType::Int2:		return 4 * 2;
		case ShaderDataType::Int3:		return 4 * 3;
		case ShaderDataType::Int4:		return 4 * 4;
		case ShaderDataType::Bool:		return 1;
		}

		// 空类型断言
		HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	/**
	* 每一个顶点属性（如 position、normal、uv）对应一个 BufferElement，这个结构体就是在描述一个属性的元数据。
	*/
	struct BufferElement
	{
		/** 属性名（例如 "a_Position"） */
		std::string Name;

		/** 属性类型 */
		ShaderDataType Type;

		/** 所占内存大小（由 ShaderDataTypeSize 推导） */
		uint32_t Size;

		/** 偏移量（layout中的位置） */
		size_t Offset;

		/** 是否需要归一化（比如颜色可能存为0-255，归一化为0.0-1.0） */
		bool Normalized;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		/** 组件数量（比如 Float3 就是3） */
		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Float:   return 1;
			case ShaderDataType::Float2:  return 2;
			case ShaderDataType::Float3:  return 3;
			case ShaderDataType::Float4:  return 4;
			case ShaderDataType::Mat3:    return 3; // 3* float3
			case ShaderDataType::Mat4:    return 4; // 4* float4
			case ShaderDataType::Int:     return 1;
			case ShaderDataType::Int2:    return 2;
			case ShaderDataType::Int3:    return 3;
			case ShaderDataType::Int4:    return 4;
			case ShaderDataType::Bool:    return 1;
			}

			HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	/**
	* 这个类保存了一组 BufferElement，也就是一个顶点数据结构的完整布局：
	* 图形API（如OpenGL）要求你明确告诉它：你传过去的一堆 float 数组里，每个顶点是怎么排列的。这个 BufferLayout 类就是做这个事情的封装。
	*/
	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		/** 获取顶点数据步长 */
		inline uint32_t GetStride() const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.cbegin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.cend(); }
	private:
		/** 遍历元素，自动计算偏移和总大小。 */
		void CalculateOffsetsAndStride()
		{
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;

		/** 整个顶点占用的字节数（步长） */
		uint32_t m_Stride = 0;
	};

	/**
	* VBO (顶点缓冲对象，Vertex Buffer Object)
	*/
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		/** 设置顶点数据布局信息 */
		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;


		static Ref<VertexBuffer> Create(uint32_t size);

		/**
		* @param vertices 顶点数组
		* @param size 数据长度
		*/
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	/**
	* EBO (索引缓冲对象，Element Buffer Object)
	* 1.避免重复存储顶点数据，减少内存占用
	* 2.优化绘制性能，避免重复处理相同顶点
	* 3.允许绘制复杂的几何形状（如立方体、网格等）
	*/
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		/** 返回索引数量 */
		virtual uint32_t GetCount() const = 0;

		/**
		* @param indices 索引数据
		* @param size 数据长度
		*/
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t size);
	};

}