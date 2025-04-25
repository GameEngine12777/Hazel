#pragma once

#include <glm/glm.hpp>

namespace Hazel {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection)
			: m_Projection(projection) {}

		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
	protected:
		/** 投影矩阵: 将「相机空间」中的坐标转换到「裁剪空间（Clip Space）」 */
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};

}
