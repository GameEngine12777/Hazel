#pragma once

#include "entt.hpp"

#include "Hazel/Renderer/EditorCamera.h"
#include "Hazel/Core/Timestep.h"
#include "Hazel/Core/UUID.h"

class b2World;

namespace Hazel {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		/** 运行时启动 */
		void OnRuntimeStart();
		/** 运行时结束 */
		void OnRuntimeStop();

		void OnSimulationStart();
		void OnSimulationStop();

		/** 运行时更新 */
		void OnUpdateRuntime(Timestep ts);
		/** 模拟更新 */
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
		/** 编辑器下更新 */
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity entity);

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void OnPhysics2DStart();
		void OnPhysics2DStop();

		void RenderScene(EditorCamera& camera);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}
