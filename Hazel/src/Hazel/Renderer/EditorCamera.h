#pragma once

#include "Camera.h"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace Hazel {

	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
	private:
		void UpdateProjection();

		/** 更新视口 */
		void UpdateView();

		/** 编辑器下，鼠标滚轮滚动时缩放场景 */
		bool OnMouseScroll(MouseScrolledEvent& e);

		/** 平移相机 */
		void MousePan(const glm::vec2& delta);
		/** 旋转相机 */
		void MouseRotate(const glm::vec2& delta);
		/** 伸缩相机 */
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;
	private:
		float m_FOV = 45.0f;
		float m_AspectRatio = 1.778f;
		float m_NearClip = 0.1f;
		float m_FarClip = 1000.0f;

		/** 观察矩阵: 把「世界空间」中的物体转换到「相机空间」中（也叫观察空间） */
		glm::mat4 m_ViewMatrix;
		/** 相机的位置 */
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		/** 聚焦点 */
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		/** 相机臂长 */
		float m_Distance = 10.0f;
		/** 视口的旋转, 这里不涉及 Roll 的旋转 */
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		/** 视口的宽高 */
		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};

}
