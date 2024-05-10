#ifndef _CGRENDEREVENTMEASUREDISTANCE_H_
#define _CGRENDEREVENTMEASUREDISTANCE_H_

#include "CGglm.h"
#include "CGRenderEvent.h"

namespace CGRender
{
	class CGRenderEventMeasureDistance :public CGRenderEvent
	{
		typedef CGRenderEvent super;
	public:
		CGRenderEventMeasureDistance(Window* window);
		~CGRenderEventMeasureDistance();


	public:
		virtual bool OnMouseMove(MouseMovedEvent& e);
		virtual bool OnMouseButtonPress(MouseButtonPressedEvent& e);
		virtual bool OnMouseButtonRelease(MouseButtonReleasedEvent& e);
	private:
		struct PrivateData;
		PrivateData* m_priv;
	};


}
#endif // !_CGRENDEREVENTMEASUREDISTANCE_H_
