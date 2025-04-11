#include "hzpch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

#include <stb_image.h>

#include <glad/glad.h>

namespace Hazel {

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		int width, height, channels;

		// 将图片垂直翻转，适应 OpenGL 的纹理坐标系（左下角为原点）。
		stbi_set_flip_vertically_on_load(1);

		// 加载图像数据并填充 width、height、channels。
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		HZ_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		// 创建纹理对象并分配显存
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		/**
		* 设置纹理的采样参数（缩小时线性插值，放大时最近邻采样）。
		* 
		* GL_TEXTURE_MIN_FILTER: 当纹理被缩小时（贴图变小，用更少的像素来表现更多的纹理）使用的过滤方式。
		* GL_TEXTURE_MAG_FILTER: 当纹理被放大时（贴图变大，用更多的像素来表现更少的纹理）使用的过滤方式。
		* 
		* GL_LINEAR: 线性插值，会平滑混合周围像素，产生模糊但平滑的效果。
		* GL_NEAREST: 最近点采样，直接选择最靠近的一个像素，结果清晰但可能有像素颗粒感（马赛克效果）。
		*/
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		/**
		* 设置纹理对象在 S（横向） 和 T（纵向） 方向上的环绕（Wrap）模式。
		* 
		* GL_REPEAT:			环绕方式之一，表示当纹理坐标超出 [0,1] 范围时，纹理图像会重复。
		* GL_MIRRORED_REPEAT:	镜像重复纹理
		* GL_CLAMP_TO_EDGE:		超出部分贴边缘的颜色
		* GL_CLAMP_TO_BORDER:	超出部分贴指定的边框颜色
		*/
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// 将图像数据上传到显存中
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		// 释放 CPU 中的图像数据内存
		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}