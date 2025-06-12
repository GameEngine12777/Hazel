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

		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};

}
