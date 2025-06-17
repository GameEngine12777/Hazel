#pragma once

#include "Hazel/Renderer/Camera.h"

namespace Hazel {

	/**
	 * @brief SceneCamera 是 Hazel 引擎中用于实体摄像机组件的相机类。
	 *        它支持两种投影模式（透视和正交），可灵活用于 2D/3D 场景。
	 *        派生自 Camera 类，提供了设置投影参数和自动更新投影矩阵的能力。
	 */
	class SceneCamera : public Camera
	{
	public:
		/** 投影类型枚举：Perspective 透视投影，Orthographic 正交投影 */
		enum class ProjectionType
		{
			Perspective = 0,
			Orthographic = 1
		};

	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		/**
		 * @brief 设置为透视投影模式，并指定其参数
		 * @param verticalFOV 垂直视场角（以弧度为单位）
		 * @param nearClip 近裁剪面
		 * @param farClip 远裁剪面
		 */
		void SetPerspective(float verticalFOV, float nearClip, float farClip);

		/**
		 * @brief 设置为正交投影模式，并指定其参数
		 * @param size 投影高度（上下可视范围）
		 * @param nearClip 近裁剪面
		 * @param farClip 远裁剪面
		 */
		void SetOrthographic(float size, float nearClip, float farClip);

		/**
		 * @brief 设置视口大小（自动用于计算宽高比 AspectRatio）
		 * @param width 视口宽度（像素）
		 * @param height 视口高度（像素）
		 */
		void SetViewportSize(uint32_t width, uint32_t height);

		/** @name 透视参数的 getter/setter */
		///@{
		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); }

		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }

		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }
		///@}

		/** @name 正交参数的 getter/setter */
		///@{
		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }

		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }

		float GetOrthographicFarClip() const { return m_OrthographicFar; }
		void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }
		///@}

		/** @name 投影类型 getter/setter */
		///@{
		ProjectionType GetProjectionType() const { return m_ProjectionType; }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }
		///@}

	private:
		/**
		 * @brief 根据当前的投影类型和参数重新计算投影矩阵。
		 *        调用者为 SetXXX 系列函数。
		 */
		void RecalculateProjection();

	private:
		/** 当前使用的投影模式，默认为正交 */
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		/** 透视投影参数 */
		float m_PerspectiveFOV = glm::radians(45.0f);   ///< 垂直视角（弧度）
		float m_PerspectiveNear = 0.01f;                ///< 近裁剪面
		float m_PerspectiveFar = 1000.0f;               ///< 远裁剪面

		/** 正交投影参数 */
		float m_OrthographicSize = 10.0f;               ///< 视口缩放
		float m_OrthographicNear = -1.0f;               ///< 近裁剪面
		float m_OrthographicFar = 1.0f;                 ///< 远裁剪面

		/** 当前相机视口的宽高比（由 SetViewportSize 设置） */
		float m_AspectRatio = 0.0f;
	};

}
