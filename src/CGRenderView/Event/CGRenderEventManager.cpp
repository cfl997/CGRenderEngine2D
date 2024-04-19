#include "CGRenderEvent.h"
#include "CGRenderEventManager.h"

#include "CGRender.h"

using namespace CGRender;

CGRenderEventManager::CGRenderEventManager()
{
}

CGRenderEventManager::~CGRenderEventManager()
{
	for (auto it = m_CGrenderEvents.begin(); it != m_CGrenderEvents.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	m_CGrenderEvents.clear();
}

void CGRender::CGRenderEventManager::OnEvent(Event& e)
{
	unsigned int size = m_CGrenderEvents.size();
	//if (size == 1)
	{
		m_CGrenderEvents[CGRenderEventType::RenderEvent_Default]->OnEvent(e);
		//return;
	}

	for (auto& data : m_CGrenderEvents)
	{
		CGRenderEventType eventType = data.second->Type();
		if (eventType != CGRenderEventType::RenderEvent_Default)
			data.second->OnEvent(e);
	}
}

bool CGRender::CGRenderEventManager::addCGRenderEvent(CGRenderEvent* renderEvent)
{
	removeCGRenderEvent(renderEvent->Type());
	m_CGrenderEvents[renderEvent->Type()] = renderEvent;
	return true;
}

bool CGRender::CGRenderEventManager::removeCGRenderEvent(CGRenderEventType renderEventType)
{
	auto srcrenderEvent = m_CGrenderEvents[renderEventType];
	if (srcrenderEvent)
	{
		delete srcrenderEvent;
		srcrenderEvent = nullptr;
		m_CGrenderEvents.erase(renderEventType);
		return true;
	}
	return false;
}


