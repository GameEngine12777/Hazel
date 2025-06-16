#pragma once

#include "Hazel/Core/Base.h"
#include "Hazel/Scene/Scene.h"
#include "Hazel/Scene/Entity.h"

namespace Hazel {

	/**
	* 场景信息面板
	* 场景大纲 ？
	*/
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		/** 在“Scene Hierarchy”中绘制单个实体的节点 */
		void DrawEntityNode(Entity entity);
		/** 为选中的实体绘制其所有组件的属性 UI */
		void DrawComponents(Entity entity);

	private:
		/** 场景上下文 */
		Ref<Scene> m_Context;
		/** 当前选中的实体 */
		Entity m_SelectionContext;
	};

}
