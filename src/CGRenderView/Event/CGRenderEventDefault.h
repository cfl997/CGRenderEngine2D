#ifndef _CGRENDEREVENTDEFAULT_H_
#define _CGRENDEREVENTDEFAULT_H_

#include "CGRenderEvent.h"

namespace CGRender
{
	class CGRenderEvent;
	class CGRenderEventDefault : public CGRenderEvent
	{
		using super = CGRenderEvent;
	public:
		CGRenderEventDefault(Window* window);
		~CGRenderEventDefault();

		virtual bool OnMouseScoll(MouseScrolledEvent& e)override;
		virtual bool OnMouseMove(MouseMovedEvent& e)override;
		virtual bool OnMouseButtonPress(MouseButtonPressedEvent& e)override;
		virtual bool OnMouseButtonRelease(MouseButtonReleasedEvent& e)override;
		void RenderTexture();
	private:
		struct PrivateData;
		PrivateData* m_priv;
	};

}
#endif // !_CGRENDEREVENTDEFAULT_H_
