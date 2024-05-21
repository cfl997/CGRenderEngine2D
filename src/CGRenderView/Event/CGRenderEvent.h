#ifndef _CGRENDEREVENT_H_
#define _CGRENDEREVENT_H_

#include "CGRenderView_Export.h"
#include "CGRenderType.h"


namespace CGRender
{

	class Event;
	class Window;
	class WindowsWindow;
	class WindowCloseEvent;
	class MouseScrolledEvent;
	class MouseMovedEvent;
	class MouseButtonPressedEvent;
	class MouseButtonReleasedEvent;

	class CGRenderEvent
	{
	public:
		CGRenderEvent(CGRenderEventType type, Window* window);
		virtual ~CGRenderEvent();
		virtual void OnEvent(Event& e);
		const CGRenderEventType Type() { return m_type; }

		//只有Default才需要在事件中处理窗口
		Window* CurWindow();
		WindowsWindow* CurWinWindow();
	public:
		/*
		* 所有的事件
		*/
		virtual bool OnMouseScoll(MouseScrolledEvent& e);
		virtual bool OnMouseMove(MouseMovedEvent& e);
		virtual bool OnMouseButtonPress(MouseButtonPressedEvent& e);
		virtual bool OnMouseButtonRelease(MouseButtonReleasedEvent& e);

	private:
		CGRenderEvent() = delete;
		CGRenderEvent(const CGRenderEvent&) = delete;
		CGRenderEvent& operator =(const CGRenderEvent&) = delete;
	private:
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		struct PrivateData;
		PrivateData* m_priv;
		CGRenderEventType m_type;
	};


}
#endif // !_CGRENDEREVENT_H_
