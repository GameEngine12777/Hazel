#pragma once

#include <string>

namespace Hazel {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		/** 激活当前着色器程序 */
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};

}