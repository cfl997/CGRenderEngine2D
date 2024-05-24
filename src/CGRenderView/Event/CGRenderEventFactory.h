#pragma once

#include "CGRenderEventRectangle.h"
#include "CGRenderEventRectWindow.h"
#include "CGRenderEventMeasureDistance.h"
#include "CGRenderEventDefault.h"
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

		CGRenderEvent* createRenderEvent(CGRenderEventType type, Window* window)
		{
			switch (type)
			{
			case RenderEvent_Default:
				return new CGRenderEventDefault(window);
				break;
			case RenderEvent_Rectangle:
			{
				return new CGRenderEventRectangle(window);
			}
			case RenderEvent_RectWindow:
			{
				return new CGRenderEventRectWindow(window);
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