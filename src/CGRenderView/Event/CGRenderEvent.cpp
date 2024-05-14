#include "CGRenderEvent.h"
#include "Event.h"
#include "WindowsEvent.h"
#include "MouseEvent.h"

#include "CGWindow.h"
#include "CGWindowsWindos.h"
#include "../CGCamera.h"
#include "CGData.h"

#include <functional>
#include <assert.h>


using namespace CGRender;


struct CGRenderEvent::PrivateData
{
	Window* window = nullptr;
};

CGRender::CGRenderEvent::CGRenderEvent(CGRenderEventType type, Window* window) :m_priv(new PrivateData), m_type(type)
{
	m_priv->window = window;
}

CGRenderEvent::~CGRenderEvent()
{
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

Window* CGRender::CGRenderEvent::getCurWindow()
{
	auto& d = *m_priv;
	return d.window;
}

//void CGRender::CGRenderEvent::setWindow(Window* window)
//{
//	auto& d = *m_priv;
//	assert(d.window == nullptr);
//	d.window = window;
//}

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
	auto& d = *m_priv;
	if (!d.window)
		return false;

	WindowsWindow* winWindow = dynamic_cast<WindowsWindow*>(d.window);
	if (!winWindow)
		return false;

	winWindow->getCamera()->ProcessMouseScroll(e.GetYOffset(), e.GetX(), e.GetY());
	return true;
}
bool CGRender::CGRenderEvent::OnMouseMove(MouseMovedEvent& e)
{
	auto& d = *m_priv;
	if (!d.window)
		return false;

	WindowsWindow* winWindow = dynamic_cast<WindowsWindow*>(d.window);
	if (!winWindow)
		return false;
	float x = e.GetX();
	float y = e.GetY();

	winWindow->getCamera()->ProcessMouseMoveXY(x, y);

	winWindow->getCurLayer()->ProcessMouseMoveXY(x, y);

	return false;
}

bool CGRender::CGRenderEvent::OnMouseButtonPress(MouseButtonPressedEvent& e)
{
	auto& d = *m_priv;
	if (!d.window)
		return false;

	WindowsWindow* winWindow = dynamic_cast<WindowsWindow*>(d.window);
	if (!winWindow)
		return false;

	float x = e.GetX();
	float y = e.GetY();

	if (e.GetMouseButton() == CG_MOUSE_BUTTON_RIGHT)
		winWindow->getCamera()->ProcessMousePress(x, y);
	if (e.GetMouseButton() == CG_MOUSE_BUTTON_RIGHT)
		winWindow->getCurLayer()->ProcessMousePress(x, y);

	return false;
}

bool CGRender::CGRenderEvent::OnMouseButtonRelease(MouseButtonReleasedEvent& e)
{

	auto& d = *m_priv;
	if (!d.window)
		return false;

	WindowsWindow* winWindow = dynamic_cast<WindowsWindow*>(d.window);
	if (!winWindow)
		return false;

	float x = e.GetX();
	float y = e.GetY();

	if (e.GetMouseButton() == CG_MOUSE_BUTTON_RIGHT)
		winWindow->getCamera()->ProcessMouseRelease(x, y);
	if (e.GetMouseButton() == CG_MOUSE_BUTTON_RIGHT)
		winWindow->getCurLayer()->ProcessMouseRelease(x, y);

	return false;
}

