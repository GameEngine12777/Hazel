#pragma once

#include <string>

#include <glm/glm.hpp>

namespace Hazel {

	class Shader
	{
	public:
		/**
		* @param vertexSrc 顶点着色器代码段
		* @param fragmentSrc 片段着色器代码段
		*/
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		/**
		* 更新 Shader 中 name 对应得变量值
		* @param name 在 GLSL 中声明得 uniform 变量
		* @param matrix 即将更新得值
		*/
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
	private:
		uint32_t m_RendererID;
	};

}