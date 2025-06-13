#pragma once

#include "Hazel/Events/Event.h"
#include "Hazel/Core/KeyCodes.h"

namespace Hazel {

	class HAZEL_API KeyEvent : public Event
	{
	public:
		inline KeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryKeyboard | EventCategory::EventCategoryInput);
	protected:
		KeyEvent(const KeyCode keycode)
			: m_KeyCode(keycode) {}

		KeyCode m_KeyCode;
	};

	class HAZEL_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode, bool isRepeat = false)
			: KeyEvent(keycode), m_IsRepeat(isRepeat) {}

		inline bool IsRepeat() const { return m_IsRepeat; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed);
	private:
		bool m_IsRepeat;
	};

	class HAZEL_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased);
	};

	class HAZEL_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}
