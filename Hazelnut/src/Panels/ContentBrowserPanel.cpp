#include "hzpch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

namespace Hazel {

	// Once we have projects, change this
	static const std::filesystem::path s_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(s_AssetPath)
	{
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(s_AssetPath))
		{
			// 回退到上一个文件夹
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetPath);
			std::string filenameString = relativePath.filename().string();
			if (directoryEntry.is_directory())
			{
				// 打开点击的文件夹
				if (ImGui::Button(filenameString.c_str()))
				{
					m_CurrentDirectory /= path.filename();
				}
			}
			else
			{
				// 不是文件夹的情况
				if (ImGui::Button(filenameString.c_str()))
				{
				}
			}
		}

		ImGui::End();
	}

}
