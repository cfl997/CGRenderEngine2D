#ifndef _CGRENDEREVENTBOX_H_
#define _CGRENDEREVENTBOX_H_

#include "CGglm.h"
#include "CGRenderEvent.h"

namespace CGRender
{
	class CGRenderEventRectangle :public CGRenderEvent
	{
		typedef CGRenderEvent super;
	public:
		CGRenderEventRectangle(Window*window);
		~CGRenderEventRectangle();


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
#endif // !_CGRENDEREVENTBOX_H_
