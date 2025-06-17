#pragma once

#include "entt.hpp"

#include "Hazel/Renderer/EditorCamera.h"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Core/UUID.h"

class b2World;

namespace Hazel {

	class Entity;

	/**
	 * @brief Hazel 引擎中的核心场景类，负责实体创建与管理、组件注册、
	 *        运行时更新、物理模拟控制、渲染、序列化等功能。
	 */
	class Scene
	{
	public:
		Scene();
		~Scene();

		/**
		 * @brief 复制一个场景，包含所有实体和组件
		 * @param other 被复制的场景
		 * @return 返回一个新的场景智能指针（深拷贝）
		 */
		static Ref<Scene> Copy(Ref<Scene> other);

		/**
		 * @brief 创建一个新实体，并默认添加基础组件
		 * @param name 可选实体名称（用于 TagComponent）
		 * @return 新创建的实体对象
		 */
		Entity CreateEntity(const std::string& name = std::string());

		/**
		 * @brief 使用指定 UUID 创建实体（用于反序列化等）
		 * @param uuid 实体的唯一标识
		 * @param name 可选实体名称
		 * @return 返回创建的实体
		 */
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());

		/**
		 * @brief 销毁指定实体，同时移除所有附加组件
		 * @param entity 要销毁的实体
		 */
		void DestroyEntity(Entity entity);

		/** 启动运行时（Runtime）模式，初始化脚本、物理、相机等组件 */
		void OnRuntimeStart();

		/** 结束运行时（Runtime）模式，释放相关资源 */
		void OnRuntimeStop();

		/** 启动模拟模式（Simulation），不运行脚本，但启用物理 */
		void OnSimulationStart();

		/** 结束模拟模式 */
		void OnSimulationStop();

		/**
		 * @brief 在运行时每帧更新场景（包括脚本驱动、物理模拟、渲染）
		 * @param ts 当前帧与上一帧之间的时间间隔
		 */
		void OnUpdateRuntime(Timestep ts);

		/**
		 * @brief 在模拟模式下更新场景（无脚本，仅物理与渲染）
		 * @param ts 时间步长
		 * @param camera 当前编辑器相机
		 */
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);

		/**
		 * @brief 在编辑器视图下更新（不运行脚本与物理）
		 * @param ts 时间步长
		 * @param camera 编辑器摄像机
		 */
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);

		/**
		 * @brief 当编辑器窗口尺寸改变时通知场景（更新相机等）
		 * @param width 新的宽度
		 * @param height 新的高度
		 */
		void OnViewportResize(uint32_t width, uint32_t height);

		/**
		 * @brief 复制一个实体及其所有组件（用于右键复制操作）
		 * @param entity 要复制的实体
		 */
		void DuplicateEntity(Entity entity);

		/**
		 * @brief 获取当前场景中标记为“主相机”的实体
		 * @return 若存在，返回主相机实体；否则返回无效实体
		 */
		Entity GetPrimaryCameraEntity();

		/**
		 * @brief 获取拥有指定组件的所有实体视图
		 * @tparam Components 可变模板参数，指定要查询的组件类型
		 * @return 返回 entt::view，用于遍历所有符合条件的实体
		 */
		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		/**
		 * @brief 内部钩子函数：当组件添加到实体时触发（初始化某些状态）
		 * @tparam T 组件类型
		 * @param entity 目标实体
		 * @param component 被添加的组件
		 */
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		/** 初始化物理世界及所有刚体组件（运行时/模拟模式） */
		void OnPhysics2DStart();

		/** 停止物理世界模拟（释放 b2World 等资源） */
		void OnPhysics2DStop();

		/**
		 * @brief 渲染场景内容（可供编辑器/运行时调用）
		 * @param camera 当前摄像机
		 */
		void RenderScene(EditorCamera& camera);

	private:
		/** EnTT 注册器，存储所有实体与组件 */
		entt::registry m_Registry;

		/** 视口尺寸（用于相机、渲染适配） */
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		/** Box2D 物理世界对象指针（仅在运行/模拟时存在） */
		b2World* m_PhysicsWorld = nullptr;

		/** 声明友元类，允许直接访问私有成员 */
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}
