#include "CGRenderEventRectangle.h"
#include "CGData.h"
#include "CGWindowsWindos.h"

using namespace CGRender;
struct CGRenderEventRectangle::PrivateData
{
	bool isPressLeft = false;
	glm::vec2 pressPos = glm::vec2{ 1 };
	glm::vec2 releasePos = glm::vec2{ 1 };

	//事件中对象所有权交由 layer释放
	CGData::CGItemRectangle* itemRect = nullptr;

	WindowsWindow* win = nullptr;
};

CGRender::CGRenderEventRectangle::CGRenderEventRectangle(Window* window) :super(CGRenderEventType::RenderEvent_Rectangle, window),
m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.isPressLeft = false;

	Window* curwindow = getCurWindow();

	d.itemRect = new CGData::CGItemRectangle(curwindow->ContextID());
	d.win = dynamic_cast<WindowsWindow*>(curwindow);
	assert(d.win != nullptr);

	auto layer = d.win->getCurLayer();
	layer->addItem(d.itemRect);
}

CGRender::CGRenderEventRectangle::~CGRenderEventRectangle()
{
	SAFE_DELETE(m_priv);
}

bool CGRender::CGRenderEventRectangle::OnMouseScoll(MouseScrolledEvent& e)
{
	return false;
}

bool CGRender::CGRenderEventRectangle::OnMouseMove(MouseMovedEvent& e)
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

bool CGRender::CGRenderEventRectangle::OnMouseButtonPress(MouseButtonPressedEvent& e)
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

bool CGRender::CGRenderEventRectangle::OnMouseButtonRelease(MouseButtonReleasedEvent& e)
{
	auto& d = *m_priv;
	if (e.GetMouseButton() == CG_MOUSE_BUTTON_LEFT)
	{
		d.isPressLeft = false;

		float posx = e.GetX();
		float posy = e.GetY();
		d.releasePos = glm::vec2{ posx,posy };

	}
	if (!d.isPressLeft)
	{

	}



	return true;
}
