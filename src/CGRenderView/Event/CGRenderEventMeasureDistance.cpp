#include "CGRenderEventMeasureDistance.h"
#include "CGData.h"
#include "CGWindowsWindos.h"

#include "MouseEvent.h"
#include "KeyEvent.h"
#include "WindowsEvent.h"

using namespace CGRender;

struct CGRender::CGRenderEventMeasureDistance::PrivateData
{

	WindowsWindow* win = nullptr;
	CGData::CGItemMeasureDistance* item = nullptr;
};

CGRender::CGRenderEventMeasureDistance::CGRenderEventMeasureDistance(Window* window) :super(CGRenderEventType::RenderEvent_MeasureDistance, window),
m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.item = new CGData::CGItemMeasureDistance();


}

CGRender::CGRenderEventMeasureDistance::~CGRenderEventMeasureDistance()
{

}

bool CGRender::CGRenderEventMeasureDistance::OnMouseMove(MouseMovedEvent& e)
{

	return true;
}

bool CGRender::CGRenderEventMeasureDistance::OnMouseButtonPress(MouseButtonPressedEvent& e)
{

	return true;
}

bool CGRender::CGRenderEventMeasureDistance::OnMouseButtonRelease(MouseButtonReleasedEvent& e)
{

	return true;
}
