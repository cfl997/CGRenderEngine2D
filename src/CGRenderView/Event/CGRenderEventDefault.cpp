#include "CGRenderEventDefault.h"

#include "Event.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "WindowsEvent.h"

#include "CGWindow.h"
#include "CGWindowsWindos.h"
#include "../CGCamera.h"
#include "CGData.h"

#include "../CGCore.h"

#include <functional>
#include <assert.h>
#include "CGRender.h"
#include <mutex>

using namespace CGRender;


struct RTTexture
{
	int hsrcTexture = -1;
	int hTargetTexture = -1;

	int renderNums = -1;
	std::recursive_mutex mutex;
	bool needUpdate = false;

	CGData::CGItemRectangle* itemRect = nullptr;
};


struct CGRenderEventDefault::PrivateData
{
	Window* window = nullptr;
	WindowsWindow* winWindow = nullptr;
	RTTexture rtTexture;
	void renderRTTexture();
	bool first = true;

	void needRenderRect();
};


CGRender::CGRenderEventDefault::CGRenderEventDefault(Window* window) :super(CGRenderEventType::RenderEvent_Default, window)
, m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.window = CurWindow();
	d.winWindow = CurWinWindow();

	d.rtTexture.itemRect = new CGData::CGItemRectangle(d.window->ContextID(), CGData::RectangleType::Normal);
	d.rtTexture.itemRect->Width(40);
	d.rtTexture.itemRect->Width(30);
}

CGRender::CGRenderEventDefault::~CGRenderEventDefault()
{
	auto& d = *m_priv;
	if (d.rtTexture.hsrcTexture != -1)
		CGRender_DeleteTexture(d.window->ContextID(), d.rtTexture.hsrcTexture);
	if (d.rtTexture.hTargetTexture != -1)
		CGRender_DeleteTexture(d.window->ContextID(), d.rtTexture.hTargetTexture);

	SAFE_DELETE(d.rtTexture.itemRect);
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
	d.needRenderRect();
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

	if (e.GetMouseButton() == CG_MOUSE_BUTTON_LEFT)
		winWindow->getCamera()->ProcessMousePress(x, y);
	if (e.GetMouseButton() == CG_MOUSE_BUTTON_LEFT)
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

	if (e.GetMouseButton() == CG_MOUSE_BUTTON_LEFT)
		winWindow->getCamera()->ProcessMouseRelease(x, y);
	if (e.GetMouseButton() == CG_MOUSE_BUTTON_LEFT)
		winWindow->getCurLayer()->ProcessMouseRelease(x, y);

	return false;
}

void CGRender::CGRenderEventDefault::RenderTexture()
{
	auto& d = *m_priv;
	d.renderRTTexture();
}

void CGRender::CGRenderEventDefault::setDefultTexture(bool needUpdate)
{
	auto& d = *m_priv;
	std::lock_guard lock(d.rtTexture.mutex);
	d.rtTexture.needUpdate = needUpdate;

}

void CGRenderEventDefault::PrivateData::renderRTTexture()
{
	//return;

	if (winWindow->RenderMode() == CGRenderMode::RenderMode_rolling)
		return;

	if (rtTexture.renderNums < 0)
		return;
	rtTexture.renderNums--;

	int contextid = window->ContextID();

	{
		std::lock_guard lock(rtTexture.mutex);
		if (rtTexture.needUpdate)
		{
			rtTexture.needUpdate = false;
			int contextid = window->ContextID();
			if (rtTexture.hsrcTexture != -1)
			{
				CGRender_DeleteTexture(contextid, rtTexture.hsrcTexture);
			}
			if (rtTexture.hTargetTexture != -1)
			{
				CGRender_DeleteTexture(contextid, rtTexture.hTargetTexture);
			}
			rtTexture.hsrcTexture = CGRender_CreateTextureFromData(contextid, 0, window->GetWidth(), window->GetHeight(), GLTextureType::GLTexture_Normal2DTex);
			rtTexture.hTargetTexture = CGRender_CreateTextureFromData(contextid, 0, window->GetWidth(), window->GetHeight(), GLTextureType::GLTexture_Normal2DTex);

			int oldTarget = CGRender_GetRenderTarget(contextid);
			CGRender_CopyTexture(contextid, rtTexture.hsrcTexture, 0, 0, window->GetWidth(), window->GetHeight(),
				oldTarget, 0, 0, window->GetWidth(), window->GetHeight());
		}
	}

	if (rtTexture.hsrcTexture == -1)
	{
		GLRender_ASSERT("GLRender_ASSERT", "renderRTTexture:rtTexture.hTexture == -1");
		return;
	}
	CGRender_CopyTexture(contextid, rtTexture.hTargetTexture, 0, 0, window->GetWidth(), window->GetHeight(),
		rtTexture.hsrcTexture, 0, 0, window->GetWidth(), window->GetHeight());
	if (1)
	{
		int oldTarget = CGRender_GetRenderTarget(contextid);
		CGRender_SetRenderTarget(contextid, rtTexture.hTargetTexture);

		glm::mat4 vpmatrix = winWindow->getVPMatrix();
		rtTexture.itemRect->Width(40);
		rtTexture.itemRect->Height(30);
		CGRender_SetLineWidth(contextid, 30);
		rtTexture.itemRect->Render(contextid, vpmatrix);//todo  只要执行这个，就会有问题
		CGRender_SetLineWidth(contextid, 1);
		if (0)
		{
			CGRender_SaveTextue(contextid, rtTexture.hTargetTexture, L"D:/testImg.png");
		}
		CGRender_SetRenderTarget(contextid, oldTarget);
	}

	int width = window->GetWidth();

	CGRECT rect{ width - 200,0,200,100 };
	CGRender_RenderTexture(contextid, rtTexture.hTargetTexture, &rect);

	if (0)
	{
		CGRender_SaveTextue(contextid, rtTexture.hTargetTexture, L"D:/testImg.png");
	}
}

void CGRenderEventDefault::PrivateData::needRenderRect()
{
	rtTexture.renderNums = 100;
}


