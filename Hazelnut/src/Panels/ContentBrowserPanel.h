#pragma once

#include <filesystem>

#include "Hazel/Renderer/Texture.h"

namespace Hazel {

	/**
	* 内容浏览器
	*/
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		/** ImGui 渲染函数：用于绘制内容浏览器的 UI 面板 */
		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;

		/** 资源浏览器图标纹理 */
		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};

}
