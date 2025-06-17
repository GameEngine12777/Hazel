#pragma once

#include "SceneCamera.h"
#include "Hazel/Core/UUID.h"
#include "Hazel/Renderer/Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Hazel {

	// Forward declaration
	class ScriptableEntity;

	/** 
	 * 每个实体的唯一标识符组件
	 */
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	/** 
	 * 实体的标签（名称）组件 
	 */
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) 
		{}
	};

	/** 
	 * 实体在世界中的空间变换组件
	 */
	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) 
		{}

		/** 获取4x4变换矩阵 */
		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	/** 
	 * 精灵渲染组件（2D贴图或颜色）
	 */
	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) 
		{}
	};

	/** 
	 * 圆形渲染组件（用于 UI 或调试）
	 */
	struct CircleRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	/** 
	 * 摄像机组件（包含场景相机、主摄像机标记等）
	 */
	struct CameraComponent
	{
		SceneCamera Camera;

		/** 是否为主摄像机 */
		bool Primary = true; // TODO: think about moving to Scene

		/** 是否固定宽高比 */
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct ScriptComponent
	{
		std::string ClassName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	/** 
	 * 原生脚本绑定组件（运行时绑定C++类）
	 */
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		/** 绑定脚本类型 */
		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};


	// Physics
	/**
	 * 2D 刚体组件，用于物理模拟
	 */
	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		/** 运行时 Box2D 的物理体指针 */
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	/**
	 * 2D 矩形碰撞体组件
	 */
	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		// TODO(Yan): move into physics material in the future maybe
		/** 密度：物体的质量。质量 = 密度 × 体积。这个碰撞体是附加到一个 Dynamic 类型的刚体上，它的密度会影响物体在力作用下的加速度、动量、碰撞反应等。 */
		float Density = 1.0f;
		/** 摩擦系数：控制两个物体接触时的滑动阻力。碰撞时两个物体之间的实际摩擦力由两者摩擦系数的某种函数（Box2D 使用的是乘积）计算得出。 */
		float Friction = 0.5f;
		/** 反弹系数：定义碰撞后“弹性”程度，也叫“弹性系数”。 */
		float Restitution = 0.0f;
		/** 反弹阈值：当碰撞速度低于这个值时，不使用 Restitution（认为是“慢速压靠”）。避免微小接触也出现“跳跃”反应，提高物理表现稳定性。 */
		float RestitutionThreshold = 0.5f;

		/** 运行时 Box2D 夹具 */
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	/**
	 * 2D 圆形碰撞体组件
	 */
	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		// TODO(Yan): move into physics material in the future maybe
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		/** Storage for runtime 运行时 Box2D 夹具 */
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	/** 泛型组件组模板，用于 ECS 系统批量注册 */
	template<typename... Component>
	struct ComponentGroup
	{
	};

	/** 所有用于场景序列化/复制的组件集合 */
	using AllComponents = ComponentGroup<
		TransformComponent,
		SpriteRendererComponent,
		CircleRendererComponent,
		CameraComponent,
		ScriptComponent,
		NativeScriptComponent,
		Rigidbody2DComponent,
		BoxCollider2DComponent,
		CircleCollider2DComponent
	>;

}
