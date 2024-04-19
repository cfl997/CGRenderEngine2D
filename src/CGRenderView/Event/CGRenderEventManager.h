#ifndef _CGRENDEREVENTMANAGER_H_
#define _CGRENDEREVENTMANAGER_H_

#include "CGRenderEvent.h"
#include <vector>
#include <map>

namespace CGRender
{
	class Event;
	class CGRenderEventManager
	{
	public:
		CGRenderEventManager();
		~CGRenderEventManager();

		void OnEvent(Event& e);
		bool addCGRenderEvent(CGRenderEvent* renderEvent);
		bool removeCGRenderEvent(CGRenderEventType renderEventType);
	private:
		std::map<CGRenderEventType, CGRenderEvent*>m_CGrenderEvents;
	};

}
#endif // !_CGRENDEREVENTMANAGER_H_
