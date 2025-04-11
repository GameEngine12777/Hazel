#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Core/Timestep.h"

namespace Hazel {

	class HAZEL_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		/** 将图层依附到 APP 中 */
		virtual void OnAttach() {}

		/** 将图层从 App 中分离 */
		virtual void OnDetach() {}

		/** 用于处理渲染的loop */
		virtual void OnUpdate(Timestep ts) {}

		virtual void OnImGuiRender() {}

		/** 用于处理事件 */
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}