#pragma once

#include "Hazel/Core/UUID.h"
#include "Scene.h"
#include "Components.h"

#include "entt.hpp"

namespace Hazel {

	/**
	 * @brief 表示一个在场景中的实体（Entity），是对 entt::entity 的封装
	 * 提供组件的添加、获取、检查和移除操作，简化 ECS 使用方式
	 */
	class Entity
	{
	public:
		/** 默认构造函数，创建一个无效的 Entity（即 m_EntityHandle 为 entt::null） */
		Entity() = default;

		/**
		 * @brief 构造一个有效实体
		 * @param handle 该实体在 entt 中的句柄
		 * @param scene 该实体所属的场景指针（提供组件注册器访问）
		 */
		Entity(entt::entity handle, Scene* scene);

		/** 默认拷贝构造 */
		Entity(const Entity& other) = default;

		/**
		 * @brief 向实体添加一个组件类型 T
		 * @note 若组件已存在，则断言失败（避免重复添加）
		 * @param args 构造该组件所需的参数
		 * @return 返回添加后的组件引用
		 */
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			HZ_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		/**
		 * @brief 添加或替换组件（若已存在则替换）
		 * @param args 构造该组件所需的参数
		 * @return 返回添加或替换后的组件引用
		 */
		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		/**
		 * @brief 获取指定类型 T 的组件
		 * @note 若组件不存在将断言失败
		 * @return 返回组件的引用
		 */
		template<typename T>
		T& GetComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		/**
		 * @brief 判断实体是否拥有指定组件类型 T
		 * @return 若存在该组件返回 true，否则 false
		 */
		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		/**
		 * @brief 移除实体上的指定组件类型 T
		 * @note 若组件不存在将断言失败
		 */
		template<typename T>
		void RemoveComponent()
		{
			HZ_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		/** 判断当前 Entity 是否有效（即不为 entt::null） */
		operator bool() const { return m_EntityHandle != entt::null; }
		/** 隐式转换为 entt::entity 类型，便于与底层 entt API 交互 */
		operator entt::entity() const { return m_EntityHandle; }
		/** 转换为整数 ID（通常用于调试或序列化） */
		operator uint32_t() const { return static_cast<uint32_t>(m_EntityHandle); }

		/** 获取该实体的 UUID（必须具备 IDComponent 组件） */
		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		/** 获取该实体的名字（必须具备 TagComponent 组件） */
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

		/** 比较两个实体是否相等（实体句柄和所属场景都相等） */
		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		/** 比较两个实体是否不相等 */
		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

	private:
		/** entt 提供的实体句柄，唯一标识一个实体 */
		entt::entity m_EntityHandle{ entt::null };

		/** 所属场景指针，提供访问 Registry 和组件管理的上下文 */
		Scene* m_Scene = nullptr;
	};

}
