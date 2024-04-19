#pragma once

#include "CGRenderEventRectangle.h"
#include "CGRenderType.h"

namespace CGRender
{
	class CGRenderEvnetFactory
	{
	private:
		CGRenderEvnetFactory() {}
		~CGRenderEvnetFactory() {};
	private:
		static CGRenderEvnetFactory* m_instance;
	public:
		static CGRenderEvnetFactory* instance()
		{
			static CGRenderEvnetFactory* m_instance = new CGRenderEvnetFactory;
			return m_instance;
		}

		CGRenderEvent* createRenderEvent(CGRenderEventType type,Window*window)
		{
			switch (type)
			{
			case RenderEvent_Default:
				return new CGRenderEvent(type, window);
				break;
			case RenderEvent_Rectangle:
			{
				return new CGRenderEventRectangle(window);
			}
			break;
			case CGRenderEventType_unknow:
				break;
			default:
				break;
			}
			return nullptr;
		}


	};
}