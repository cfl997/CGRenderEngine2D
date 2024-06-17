#include "CGRenderEvent.h"
#include "CGRenderEventManager.h"

#include "CGRender.h"
#include "../CGCore.h"

using namespace CGRender;

CGRenderEventManager::CGRenderEventManager()
{
	m_currentType = RenderEvent_Default;
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
	m_CGrenderEvents[m_currentType]->OnEvent(e);
}

bool CGRender::CGRenderEventManager::addCGRenderEvent(CGRenderEvent* renderEvent)
{
	removeCGRenderEvent(renderEvent->Type());
	m_CGrenderEvents[renderEvent->Type()] = renderEvent;
	m_currentType = renderEvent->Type();
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

CGRenderEvent* CGRender::CGRenderEventManager::getCGRenderEvent(CGRenderEventType type)
{
	return m_CGrenderEvents[type];
}

void CGRender::CGRenderEventManager::setCurrentEvent(CGRenderEventType type)
{
	m_currentType = type;
}


