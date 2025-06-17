#include "hzpch.h"
#include "Hazel/Renderer/Renderer2D.h"

#include "Hazel/Renderer/VertexArray.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/RenderCommand.h"
#include "Hazel/Renderer/UniformBuffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// https://chatgpt.com/c/67fa38bd-a720-8007-8da7-d33db289e844
namespace Hazel {

	/**
	 * @brief 表示单个 Quad（矩形）顶点的数据结构，用于 VertexBuffer。
	 */
	struct QuadVertex
	{
		glm::vec3 Position;       // 顶点的世界空间位置
		glm::vec4 Color;          // 顶点颜色（包括 alpha）
		glm::vec2 TexCoord;       // 纹理坐标（UV）
		float TexIndex;           // 当前顶点使用的纹理在绑定槽中的索引
		float TilingFactor;       // 纹理平铺因子（控制重复次数）

		int EntityID;             // 对应实体的 ID（仅用于编辑器中高亮/选中等）
	};

	/**
	 * @brief 表示单个圆形图元的顶点结构，支持圆环厚度与渐变。
	 */
	struct CircleVertex
	{
		glm::vec3 WorldPosition;  ///< 世界空间位置（用于变换）
		glm::vec3 LocalPosition;  ///< 局部空间位置（用于精确控制圆形外观）
		glm::vec4 Color;          ///< 圆形颜色
		float Thickness;          ///< 圆环厚度（0~1）
		float Fade;               ///< 渐变强度（边缘软化）

		int EntityID;             ///< 对应实体 ID（仅编辑器使用）
	};

	/**
	 * @brief 表示线段渲染的顶点结构，用于绘制辅助线（如 Gizmo）。
	 */
	struct LineVertex
	{
		glm::vec3 Position;       ///< 顶点位置
		glm::vec4 Color;          ///< 线条颜色

		int EntityID;             ///< 实体 ID（用于调试或点击选择）
	};

	/**
	 * @brief Renderer2D 渲染核心数据结构，集中管理所有缓冲区、资源状态、统计信息等。
	 */
	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;				///< 最大可绘制 Quad 数（影响缓冲区大小）
		static const uint32_t MaxVertices = MaxQuads * 4;	///< 顶点总数（一个 Quad 由 4 个顶点构成）
		static const uint32_t MaxIndices = MaxQuads * 6;	///< 索引总数（两个三角形绘制一个 Quad）
		static const uint32_t MaxTextureSlots = 32;			///< 最大绑定纹理数量（GPU 限制，通常为 32）

		// Quad 绘制相关资源
		Ref<VertexArray> QuadVertexArray;					///< Quad 的 VAO
		Ref<VertexBuffer> QuadVertexBuffer;					///< Quad 的 VBO
		Ref<Shader> QuadShader;								///< Quad 使用的 Shader
		Ref<Texture2D> WhiteTexture;						///< 默认白色纹理（用于纯色绘制）

		// Circle 绘制资源
		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		// Line 绘制资源
		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		// Quad 绘制动态数据
		uint32_t QuadIndexCount = 0;						///< 当前批次内已使用的索引数
		QuadVertex* QuadVertexBufferBase = nullptr;			///< 顶点缓冲区起始地址
		QuadVertex* QuadVertexBufferPtr = nullptr;			///< 当前写入位置（指针）

		// Circle 绘制动态数据
		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		// Line 绘制动态数据
		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		// 渲染参数
		float LineWidth = 2.0f;								///< 当前线宽（OpenGL 控制）

		// 当前绑定的所有纹理列表，Slot 0 通常为默认白色纹理
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;						///< 当前使用的纹理槽索引（0为默认纹理）

		// 单个 Quad 的标准顶点顺序（用于索引组装）
		glm::vec4 QuadVertexPositions[4];

		// 渲染统计信息
		Renderer2D::Statistics Stats;

		/**
		 * @brief 当前场景的摄像机矩阵缓存
		 *        会被上传到 UniformBuffer，以便所有 Shader 共享。
		 */
		struct CameraData
		{
			glm::mat4 ViewProjection; ///< 相机的视图投影矩阵
		};
		CameraData CameraBuffer;

		Ref<UniformBuffer> CameraUniformBuffer;				///< 相机数据对应的 Uniform Buffer（GL UBO）
	};

	// 实例化全局渲染状态
	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		HZ_PROFILE_FUNCTION(); // 用于性能分析的宏（Profile 工具记录函数耗时）

		// ------------------- Quad 渲染数据初始化 -------------------
		s_Data.QuadVertexArray = VertexArray::Create(); // 创建 VAO（管理顶点状态）

		// 创建 Quad 顶点缓冲区（预留足够内存用于批处理渲染）
		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));

		// 设置顶点布局，匹配 Shader 中的顶点属性
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Position" },       // 位置
			{ ShaderDataType::Float4,	"a_Color" },          // 颜色
			{ ShaderDataType::Float2,	"a_TexCoord" },       // 纹理坐标
			{ ShaderDataType::Float,	"a_TexIndex" },       // 纹理索引
			{ ShaderDataType::Float,	"a_TilingFactor" },   // 平铺系数
			{ ShaderDataType::Int,		"a_EntityID" }        // 实体 ID（编辑器使用）
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		// 分配 CPU 端缓存，用于动态构造顶点数据（后续通过 memcpy 上传到 GPU）
		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		// 构造 Quad 索引缓冲区（每个 Quad 由两个三角形组成，共6个索引）
		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4; // 每4个顶点表示一个 Quad
		}

		// 创建 Index Buffer，并绑定到 VAO 中
		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices; // 临时内存释放


		// ------------------- Circle 渲染数据初始化 -------------------
		s_Data.CircleVertexArray = VertexArray::Create();

		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color"         },
			{ ShaderDataType::Float,  "a_Thickness"     },
			{ ShaderDataType::Float,  "a_Fade"          },
			{ ShaderDataType::Int,    "a_EntityID"      }
			});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);

		// 与 Quad 共享相同的索引缓冲（都是 2D 图元）
		s_Data.CircleVertexArray->SetIndexBuffer(quadIB);

		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];


		// ------------------- Line 渲染数据初始化 -------------------
		s_Data.LineVertexArray = VertexArray::Create();

		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);

		s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];


		// ------------------- 创建默认白色纹理 -------------------
		s_Data.WhiteTexture = Texture2D::Create(1, 1); // 单像素纹理
		uint32_t whiteTextureData = 0xffffffff;        // RGBA = (255,255,255,255)
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		// 创建默认的纹理绑定槽（用于在着色器中采样多个纹理）
		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;


		// ------------------- 加载 Shader -------------------
		s_Data.QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");
		s_Data.CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");
		s_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");

		// 设置第0个纹理槽为白色纹理（默认用于纯色绘制）
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;


		// ------------------- 定义 Quad 顶点顺序 -------------------
		// 顶点顺序为逆时针，从左下角开始，以中心为锚点
		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f }; // 左下
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f }; // 右下
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f }; // 右上
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f }; // 左上


		// ------------------- 创建 Uniform Buffer（绑定槽0） -------------------
		// 用于上传相机 ViewProjection 矩阵，供所有 Shader 使用
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		HZ_PROFILE_FUNCTION();

		delete[] s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBufferBase = nullptr;
		s_Data.QuadVertexBufferPtr = nullptr;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		HZ_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		HZ_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			s_Data.QuadShader->Bind();
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

			s_Data.CircleShader->Bind();
			RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

			s_Data.LineShader->Bind();
			RenderCommand::SetLineWidth(s_Data.LineWidth);
			RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
			s_Data.Stats.DrawCalls++;
		}
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		HZ_PROFILE_FUNCTION(); // 性能分析（记录函数调用耗时）

		constexpr size_t quadVertexCount = 4; // 一个 Quad 有四个顶点
		const float textureIndex = 0.0f; // 使用白色纹理（表示无贴图，仅用颜色）
		
		// 标准纹理坐标
		constexpr glm::vec2 textureCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};
		const float tilingFactor = 1.0f; // 不重复贴图（仅在有纹理时生效）

		// 如果当前批次的 Index 数已达上限，则先提交（Flush），再开启新批次
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		// 将 Quad 的四个顶点写入缓冲区
		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i]; // 应用变换矩阵
			s_Data.QuadVertexBufferPtr->Color = color;                // 设置颜色
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];  // 设置纹理坐标
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;      // 使用白色纹理（索引0）
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;  // 贴图重复因子
			s_Data.QuadVertexBufferPtr->EntityID = entityID;          // 实体 ID（用于选中、调试）
			s_Data.QuadVertexBufferPtr++;                             // 移动指针写入下一个顶点
		}

		// 一个 Quad 需要绘制 6 个索引（三角形 * 2）
		s_Data.QuadIndexCount += 6;

		// 统计信息更新
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		HZ_PROFILE_FUNCTION(); // 性能分析

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		// 若当前批处理数量已满，则先渲染已有内容，再开启新一批
		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		// ----------- 查找当前贴图是否已存在于 TextureSlots 中 -----------
		float textureIndex = 0.0f; // 默认白色纹理（备用）
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i; // 找到纹理槽，获取对应索引
				break;
			}
		}

		// 如果没有找到当前纹理，就加入 TextureSlots 数组中
		if (textureIndex == 0.0f)
		{
			// 如果纹理槽满了，也需要提交当前批次，重置槽
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture; // 新增纹理引用
			s_Data.TextureSlotIndex++;
		}

		// 写入 4 个 Quad 顶点
		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tintColor;              // 给贴图加上颜色调制（可做遮罩）
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;        // 使用绑定槽中纹理
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;    // 贴图平铺控制
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		// 一个 Quad 需要绘制 6 个索引（三角形 * 2）
		s_Data.QuadIndexCount += 6;

		// 统计信息更新
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		HZ_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		HZ_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		HZ_PROFILE_FUNCTION(); // 性能分析宏，用于记录函数调用时间

		// TODO: implement for circles
		// 当前尚未做批次上限判断，如果未来出现大量圆形绘制，需要加上
		// if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		//     NextBatch();

		// 圆形的绘制实际上和 Quad 一样走四个顶点的路径，
		// 然后通过 Shader 判断 fragment 是否在圆形范围内，从而达到圆形效果
		for (size_t i = 0; i < 4; i++)
		{
			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];  // 顶点的世界坐标
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;       // 本地坐标，中心对称放大，供 Shader 做圆形计算使用
			s_Data.CircleVertexBufferPtr->Color = color;               // 圆的颜色
			s_Data.CircleVertexBufferPtr->Thickness = thickness;       // 圆环的厚度（线宽）
			s_Data.CircleVertexBufferPtr->Fade = fade;                 // 边缘的模糊程度
			s_Data.CircleVertexBufferPtr->EntityID = entityID;         // 实体 ID（用于编辑器选中）
			s_Data.CircleVertexBufferPtr++;                            // 写入下一个顶点
		}

		s_Data.CircleIndexCount += 6;   // 每个圆仍是一个矩形 quad（两个三角形）

		s_Data.Stats.QuadCount++;       // 统计中也按 Quad 来算一个圆形
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		// 第一个端点
		s_Data.LineVertexBufferPtr->Position = p0;       // 起点坐标
		s_Data.LineVertexBufferPtr->Color = color;       // 线颜色
		s_Data.LineVertexBufferPtr->EntityID = entityID; // 实体 ID
		s_Data.LineVertexBufferPtr++;                    // 移动到下一个顶点

		// 第二个端点
		s_Data.LineVertexBufferPtr->Position = p1;       // 终点坐标
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexCount += 2; // 每条线段两个点
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color, entityID);
		DrawLine(p1, p2, color, entityID);
		DrawLine(p2, p3, color, entityID);
		DrawLine(p3, p0, color, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_Data.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color, entityID);
		DrawLine(lineVertices[1], lineVertices[2], color, entityID);
		DrawLine(lineVertices[2], lineVertices[3], color, entityID);
		DrawLine(lineVertices[3], lineVertices[0], color, entityID);
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		else
			DrawQuad(transform, src.Color, entityID);
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_Data.LineWidth = width;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Hazel::Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer2D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

}
