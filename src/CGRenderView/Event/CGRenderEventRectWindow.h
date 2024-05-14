#ifndef _CGRENDEREVENTRECTWINDOW_H_
#define _CGRENDEREVENTRECTWINDOW_H_

#include "CGglm.h"
#include "CGRenderEvent.h"

namespace CGRender
{
	class CGRenderEventRectWindow :public CGRenderEvent
	{
		typedef CGRenderEvent super;
	public:
		CGRenderEventRectWindow(Window*window);
		~CGRenderEventRectWindow();


	public:
		/*
		* 所有的事件
		*/
		virtual bool OnMouseScoll(MouseScrolledEvent& e);
		virtual bool OnMouseMove(MouseMovedEvent& e);
		virtual bool OnMouseButtonPress(MouseButtonPressedEvent& e);
		virtual bool OnMouseButtonRelease(MouseButtonReleasedEvent& e);
	private:
		struct PrivateData;
		PrivateData* m_priv;
	};


}
#endif // !_CGRENDEREVENTRECTWINDOW_H_
