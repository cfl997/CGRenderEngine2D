#include "CGRenderEventDefault.h"

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

const static int s_rtTextureWidth = 200;
const static int s_rtTextureHeight = 100;

struct RTTexture
{
	int hTexture = -1;
	int width = s_rtTextureWidth;
	int height = s_rtTextureHeight;
	int renderNums = -1;
};


struct CGRenderEventDefault::PrivateData
{
	Window* window = nullptr;
	WindowsWindow* winWindow = nullptr;
	RTTexture rtTexture;
	void renderRTTexture();
	bool first = true;
};


CGRender::CGRenderEventDefault::CGRenderEventDefault(Window* window) :super(CGRenderEventType::RenderEvent_Default, window)
,m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.window = CurWindow();
	d.winWindow = CurWinWindow();
}

CGRender::CGRenderEventDefault::~CGRenderEventDefault()
{
	auto& d = *m_priv;
	if (d.rtTexture.hTexture != -1)
		CGRender_DeleteTexture(d.window->ContextID(), d.rtTexture.hTexture);
	SAFE_DELETE(m_priv);
}
bool CGRender::CGRenderEventDefault::OnMouseScoll(MouseScrolledEvent& e)
{
	auto& d = *m_priv;
	if (!d.window)
		return false;

	WindowsWindow* winWindow = dynamic_cast<WindowsWindow*>(d.window);
	if (!winWindow)
		return false;

	winWindow->getCamera()->ProcessMouseScroll(e.GetYOffset(), e.GetX(), e.GetY());
	//d.renderRTTexture();
	d.rtTexture.renderNums = 10;
	return true;
}
bool CGRender::CGRenderEventDefault::OnMouseMove(MouseMovedEvent& e)
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

bool CGRender::CGRenderEventDefault::OnMouseButtonPress(MouseButtonPressedEvent& e)
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

bool CGRender::CGRenderEventDefault::OnMouseButtonRelease(MouseButtonReleasedEvent& e)
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

void CGRender::CGRenderEventDefault::RenderTexture()
{
	auto& d = *m_priv;
	d.renderRTTexture();
}

bool firstbuffer = true;
int bufferSize = s_rtTextureWidth * s_rtTextureHeight * 4;
char* buffer = new char[bufferSize];

void CGRenderEventDefault::PrivateData::renderRTTexture()
{
	if (rtTexture.renderNums < 0)
		return;
	rtTexture.renderNums--;

	if (rtTexture.hTexture == -1)
		rtTexture.hTexture = CGRender_CreateTextureFromData(window->ContextID(), 0, rtTexture.width, rtTexture.height, GLTexture_Normal2DTex);


	int width = window->GetWidth();
	if (firstbuffer)
	{
		for (int i = 0; i < bufferSize; i += 4) {
			buffer[i] = 0x00; // Red
			buffer[i + 1] = 0x00; // Green
			buffer[i + 2] = 0xff; // Blue
			buffer[i + 3] = 0xFF; // Alpha
		}
		firstbuffer = false;
	}
	CGRender_UploadTexture(window->ContextID(), rtTexture.hTexture, 0, 0, rtTexture.width, rtTexture.height, buffer);

	CGRECT rect{ width - 200,0,200,100 };
	CGRender_RenderTexture(window->ContextID(), rtTexture.hTexture, &rect);
}
