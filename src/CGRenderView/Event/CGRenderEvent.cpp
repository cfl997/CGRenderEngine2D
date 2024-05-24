#include "CGRenderEvent.h"

#include "Event.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "WindowsEvent.h"

#include "CGWindow.h"
#include "CGWindowsWindos.h"
#include "../CGCamera.h"
#include "CGData.h"

#include <functional>
#include <assert.h>

#include "CGRender.h"

using namespace CGRender;


struct CGRenderEvent::PrivateData
{
	Window* window = nullptr;
	WindowsWindow* winWindow = nullptr;

};

CGRender::CGRenderEvent::CGRenderEvent(CGRenderEventType type, Window* window) :m_priv(new PrivateData), m_type(type)
{
	auto& d = *m_priv;
	d.window = window;
	d.winWindow = dynamic_cast<WindowsWindow*>(d.window);
}

CGRenderEvent::~CGRenderEvent()
{
	SAFE_DELETE(m_priv);
}

void CGRender::CGRenderEvent::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);

	dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(CGRenderEvent, OnMouseMove));
	dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(CGRenderEvent, OnMouseButtonPress));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(CGRenderEvent, OnMouseButtonRelease));
	dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(CGRenderEvent, OnMouseScoll));

	GLRender_LOG("CGRenderEvent default type:", e.GetName());
	GLRender_LOG("CGRenderEvent default tostring:", e.ToString());
	switch (m_type)
	{
	case RenderEvent_Default:
	{

		//std::function binda = std::bind(&CGRenderEvent::OnMouseScoll, this, std::placeholders::_1);
		GLRender_LOG("CGRenderEvent default type:", e.GetName());
		GLRender_LOG("CGRenderEvent default tostring:", e.ToString());

		{
			EventDispatcher dispatcher(e);
			dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(CGRenderEvent, OnWindowClose));
		}


	}
	case CGRenderEventType_unknow:
	{

	}
	break;
	default:
		break;
	}

}

Window* CGRender::CGRenderEvent::CurWindow()
{
	auto& d = *m_priv;
	return d.window;
}

WindowsWindow* CGRender::CGRenderEvent::CurWinWindow()
{
	auto& d = *m_priv;
	return d.winWindow;
}

bool CGRender::CGRenderEvent::OnWindowClose(WindowCloseEvent& e)
{
	auto& d = *m_priv;
	if (!d.window)
		return false;

	d.window->OnWindowClose();
	return true;

	//d.window->Close();
	//SAFE_DELETE(d.window);
	return true;
}

bool CGRender::CGRenderEvent::OnMouseScoll(MouseScrolledEvent& e)
{
	return true;
}
bool CGRender::CGRenderEvent::OnMouseMove(MouseMovedEvent& e)
{
	return true;
}

bool CGRender::CGRenderEvent::OnMouseButtonPress(MouseButtonPressedEvent& e)
{
	return true;
}

bool CGRender::CGRenderEvent::OnMouseButtonRelease(MouseButtonReleasedEvent& e)
{
	return true;
}
