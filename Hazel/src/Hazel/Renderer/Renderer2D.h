#pragma once

#include "Hazel/Renderer/OrthographicCamera.h"

#include "Hazel/Renderer/Texture.h"
#include "Hazel/Renderer/Camera.h"
#include "Hazel/Renderer/EditorCamera.h"
#include "Hazel/Scene/Components.h"

namespace Hazel {

	/**
	 * @brief Renderer2D 是 Hazel 引擎中负责 2D 渲染的静态类。
	 *        提供简单的接口来绘制 2D 图元（Quad、Circle、Line 等）并进行批量渲染优化。
	 *        支持颜色和纹理绘制，自动处理批次管理，提供渲染统计信息。
	 */
	class Renderer2D
	{
	public:
		/** 初始化 2D 渲染器资源（如 Shader、缓冲区等） */
		static void Init();

		/** 清理释放所有资源 */
		static void Shutdown();

		/**
		 * @brief 开始一次新的渲染场景（普通摄像机版本）
		 * @param camera 提供视图-投影矩阵的相机
		 * @param transform 相机的世界变换矩阵
		 */
		static void BeginScene(const Camera& camera, const glm::mat4& transform);

		/** @brief 编辑器摄像机版本 */
		static void BeginScene(const EditorCamera& camera);

		/** @brief 兼容旧接口的正交摄像机版本（待移除） */
		static void BeginScene(const OrthographicCamera& camera); // TODO: Remove

		/** 结束当前场景的绘制并刷新所有批次 */
		static void EndScene();

		/** 手动刷新当前批次数据（通常由 EndScene 自动调用） */
		static void Flush();

		// ----------------------------------------------
		// 基本图元绘制接口
		// ----------------------------------------------

		/** @name 绘制普通 Quad（方形） */
		///@{
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);
		///@}

		/** @name 绘制带旋转的 Quad（以中心为轴旋转） */
		///@{
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		///@}

		/**
		 * @brief 绘制圆形（可用于 UI 圆圈或光圈效果）
		 * @param transform 世界变换矩阵
		 * @param color 填充颜色
		 * @param thickness 圆环厚度（0~1）
		 * @param fade 边缘渐变程度
		 * @param entityID 实体 ID（用于选择/调试）
		 */
		static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

		/**
		 * @brief 绘制一条线段
		 * @param p0 起点
		 * @param p1 终点
		 * @param color 线段颜色
		 * @param entityID 实体 ID（用于调试/选中）
		 */
		static void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID = -1);

		/** @brief 绘制一个矩形边框（线框） */
		static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		/** @brief 根据 SpriteRendererComponent 绘制一个精灵（纹理 + 颜色 + 旋转等） */
		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);

		/** 获取/设置绘制线条的宽度（OpenGL 支持取决于平台） */
		static float GetLineWidth();
		static void SetLineWidth(float width);

		// ----------------------------------------------
		// 统计信息（用于性能调试和 UI 显示）
		// ----------------------------------------------
		struct Statistics
		{
			uint32_t DrawCalls = 0;    ///< 发起的渲染调用次数（越少越好）
			uint32_t QuadCount = 0;    ///< 总共绘制的 Quad 数量

			/** 统计顶点数（4 个/Quad） */
			uint32_t GetTotalVertexCount() const { return QuadCount * 4; }

			/** 统计索引数（6 个/Quad） */
			uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		};

		/** 重置统计数据（通常每帧调用一次） */
		static void ResetStats();

		/** 获取当前的统计信息（可用于 ImGui 显示等） */
		static Statistics GetStats();

	private:
		/** 开始一个新的批次（缓冲满时调用） */
		static void StartBatch();

		/** 切换到下一个批次并清空上一个数据 */
		static void NextBatch();
	};

}
