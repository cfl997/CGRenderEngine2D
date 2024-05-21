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

	private:

	};

}
#endif // !_CGRENDEREVENTDEFAULT_H_
