#pragma once

#include "../CGCore.h"
#include <string>

#define CG_MOUSE_BUTTON_1         0
#define CG_MOUSE_BUTTON_2         1
#define CG_MOUSE_BUTTON_3         2
#define CG_MOUSE_BUTTON_4         3
#define CG_MOUSE_BUTTON_5         4
#define CG_MOUSE_BUTTON_6         5
#define CG_MOUSE_BUTTON_7         6
#define CG_MOUSE_BUTTON_8         7
#define CG_MOUSE_BUTTON_LAST      CG_MOUSE_BUTTON_8
#define CG_MOUSE_BUTTON_LEFT      CG_MOUSE_BUTTON_1
#define CG_MOUSE_BUTTON_RIGHT     CG_MOUSE_BUTTON_2
#define CG_MOUSE_BUTTON_MIDDLE    CG_MOUSE_BUTTON_3

namespace CGRender
{

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication    = BIT(0),
		EventCategoryInput          = BIT(1),
		EventCategoryKeyboard       = BIT(2),
		EventCategoryMouse          = BIT(3),
		EventCategoryMouseButton    = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }


#define BIND_EVENT_FN(x,y) std::bind(&x::y, this, std::placeholders::_1)

	class Event
	{
	public:
		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}
		
		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled = func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

}

