#pragma once

#include <glm/glm.hpp>

namespace Hazel {

	/**
	* 正交投影摄像机
	* 存储视口 VP 矩阵
	*/
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();
	private:
		/** 投影矩阵: 将「相机空间」中的坐标转换到「裁剪空间（Clip Space）」 */
		glm::mat4 m_ProjectionMatrix;
		/** 观察矩阵: 把「世界空间」中的物体转换到「相机空间」中（也叫观察空间） */
		glm::mat4 m_ViewMatrix;
		/** VP矩阵 */
		glm::mat4 m_ViewProjectionMatrix;

		/** 当前相机的位置 */
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		/** 相机的旋转，仅围绕Z轴(在计算时体现) */
		float m_Rotation = 0.0f;
	};

}
