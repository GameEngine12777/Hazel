#include "hzpch.h"
#include "Hazel/Renderer/OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
	{
		HZ_PROFILE_FUNCTION();

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		HZ_PROFILE_FUNCTION();
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		HZ_PROFILE_FUNCTION();

		// 将相机的位置和旋转转换到世界空间中(正交相机，仅旋转了Z轴)，把相机“放置”到 m_Position 处，并绕 Z 轴旋转 m_Rotation 度
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		// 将上面的 transform 取逆，得到从“世界空间”到“相机空间”的变换。
		// 在图形学中我们不是真让相机动，而是把整个世界反向移动到相机前面，这就是 View 矩阵的本质。
		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}
