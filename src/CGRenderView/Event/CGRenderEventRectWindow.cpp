#include "CGRenderEventRectWindow.h"
#include "CGData.h"
#include "CGWindowsWindos.h"

using namespace CGRender;
struct CGRenderEventRectWindow::PrivateData
{
	bool isPressLeft = false;
	glm::vec2 pressPos = glm::vec2{ 1 };
	glm::vec2 releasePos = glm::vec2{ 1 };

	//事件中对象所有权交由 layer释放
	CGData::CGItemRectangle* itemRect = nullptr;

	WindowsWindow* win = nullptr;
};

CGRender::CGRenderEventRectWindow::CGRenderEventRectWindow(Window* window) :super(CGRenderEventType::RenderEvent_RectWindow, window),
m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.isPressLeft = false;

	Window* curwindow = getCurWindow();

	d.itemRect = new CGData::CGItemRectangle(curwindow->ContextID(), CGData::RectangleType::Window);
	d.itemRect->Color({ 0, 0, 1, 1 });
	d.win = dynamic_cast<WindowsWindow*>(curwindow);
	assert(d.win != nullptr);

	auto layer = d.win->getCurLayer();
	layer->addItem(d.itemRect);
}

CGRender::CGRenderEventRectWindow::~CGRenderEventRectWindow()
{
	SAFE_DELETE(m_priv);
}

bool CGRender::CGRenderEventRectWindow::OnMouseScoll(MouseScrolledEvent& e)
{
	return false;
}

bool CGRender::CGRenderEventRectWindow::OnMouseMove(MouseMovedEvent& e)
{
	auto& d = *m_priv;
	if (!d.isPressLeft)
		return false;
	float posx = e.GetX();
	float posy = e.GetY();

	d.releasePos = glm::vec2{ posx,posy };

	float width = d.releasePos.x - d.pressPos.x;
	float height = d.releasePos.y - d.pressPos.y;

	d.itemRect->Width(width);
	d.itemRect->Height(-height);

	return false;
}

bool CGRender::CGRenderEventRectWindow::OnMouseButtonPress(MouseButtonPressedEvent& e)
{
	auto& d = *m_priv;
	if (e.GetMouseButton() == CG_MOUSE_BUTTON_LEFT)
	{

		d.isPressLeft = true;
		d.pressPos = glm::vec2{ e.GetX(),e.GetY() };
		d.itemRect->setPos(glm::vec3(d.pressPos, CGData::g_itemZDistance));
	}
	return false;
}

bool CGRender::CGRenderEventRectWindow::OnMouseButtonRelease(MouseButtonReleasedEvent& e)
{
	auto& d = *m_priv;
	if (e.GetMouseButton() == CG_MOUSE_BUTTON_LEFT)
	{
		d.isPressLeft = false;

		float posx = e.GetX();
		float posy = e.GetY();
		d.releasePos = glm::vec2{ posx,posy };
		d.itemRect->NeedRender(false);
		d.win->AfterEvent();

	}
	if (!d.isPressLeft)
	{

	}



	return true;
}
