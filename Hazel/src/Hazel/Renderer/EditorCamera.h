#pragma once

#include "Camera.h"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/MouseEvent.h"
#include <Hazel/Events/KeyEvent.h>

#include <glm/glm.hpp>

namespace Hazel {

	class EditorCamera : public Camera
	{
	public:
		/** 默认构造函数，不做初始化 */
		EditorCamera() = default;
		/** 构造函数：使用视角、宽高比、近平面和远平面初始化相机 */
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		/** 每帧更新相机的状态（比如处理输入变化、计算矩阵等） */
		void OnUpdate(Timestep ts);
		/** 处理与相机有关的事件（如鼠标滚轮缩放、拖动等） */
		void OnEvent(Event& e);

		/** 获取当前相机与焦点的距离（即相机“臂长”） */
		inline float GetDistance() const { return m_Distance; }
		/** 设置当前相机与焦点的距离 */
		inline void SetDistance(float distance) { m_Distance = distance; }

		/** 设置视口的宽度和高度，并重新计算投影矩阵 */
		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		// https://www.yuque.com/u34333378/oevi5k/vn5mz9zhc5kytakr
		/** 获取当前的视图矩阵（即观察矩阵） */
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		/** 获取完整的视图-投影矩阵，用于渲染物体时的转换 */
		glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		/** 获取相机当前“正上方”的方向向量（世界空间中） */
		glm::vec3 GetUpDirection() const;
		/** 获取相机当前“正右方”的方向向量 */
		glm::vec3 GetRightDirection() const;
		/** 获取相机当前“正前方”的方向向量 */
		glm::vec3 GetForwardDirection() const;
		/** 获取相机的世界空间位置 */
		const glm::vec3& GetPosition() const { return m_Position; }
		/** 获取当前相机的朝向（以四元数形式） */
		glm::quat GetOrientation() const;

		/** 获取当前相机的俯仰角（绕 X 轴旋转） */
		float GetPitch() const { return m_Pitch; }
		/** 获取当前相机的偏航角（绕 Y 轴旋转） */
		float GetYaw() const { return m_Yaw; }

	private:
		/** 根据当前参数（FOV、宽高比、近平远平面）更新投影矩阵 */
		void UpdateProjection();

		/** 根据当前相机位置、焦点、旋转角度计算 视图矩阵 */
		void UpdateView();

		/** 处理按键事件 */
		bool OnKeyPressed(KeyPressedEvent& e);
		/** 处理鼠标按下事件 */
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		/** 处理鼠标滚轮事件：实现缩放相机（拉近或拉远） */
		bool OnMouseScroll(MouseScrolledEvent& e);

		/** 鼠标拖动实现平移相机（移动焦点位置） */
		void MousePan(const glm::vec2& delta);
		/** 鼠标拖动实现相机围绕焦点旋转 */
		void MouseRotate(const glm::vec2& delta);
		/** 鼠标滚轮缩放距离，改变相机与焦点的距离 */
		void MouseZoom(float delta);

		/** 根据当前的焦点、距离、角度计算出相机的实际位置 */
		glm::vec3 CalculatePosition() const;

		/** 根据当前视口大小动态调整平移速度（避免视口越大，拖动越慢） */
		std::pair<float, float> PanSpeed() const;
		/** 获取当前旋转速度（通常固定值，也可变） */
		float RotationSpeed() const;
		/** 缩放相机时的速度因子，决定鼠标滚轮缩放的敏感度 */
		float ZoomSpeed() const;

	private:
		/** 相机参数：视场角 */
		float m_FOV = 45.0f;
		/** 相机参数：宽高比 */
		float m_AspectRatio = 1.778f;
		/** 相机参数：近平面 */
		float m_NearClip = 0.1f;
		/** 相机参数：远平面 */
		float m_FarClip = 1000.0f;

		/** 相机观察矩阵（从世界坐标转换到视图坐标） */
		glm::mat4 m_ViewMatrix;
		/** 相机在世界空间的位置 */
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		/** 相机观察的目标点（焦点） */
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		/** 鼠标拖动时的初始位置，用于计算差值 */
		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		/** 相机与焦点之间的距离（即轨道摄像机的半径） */
		float m_Distance = 10.0f;
		/** 相机的旋转角度（俯仰与偏航） */
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		/** 当前视口的尺寸（用于计算宽高比等） */
		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};

}
