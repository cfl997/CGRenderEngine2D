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



	auto windowWidth = winWindow->GetWidth();
	auto windowHeight = winWindow->GetHeight();

	{
		//RedRect
		int oldTarget = CGRender_GetRenderTarget(contextid);
		CGRender_SetRenderTarget(contextid, rtTexture.hTargetTexture);

		auto scaleMatrix = glm::mat4{ 1 };
		float RedrectScale = 1.0;
		{
			float ScaleCoefficient = winWindow->getCamera()->ScaleCoefficient();
			float minScale = winWindow->getCamera()->ImageMinScale();
			RedrectScale = 1 / (ScaleCoefficient - minScale);
			if (RedrectScale / 2.5 < 1)
				RedrectScale *= 2.5;
			scaleMatrix = glm::scale(scaleMatrix, { RedrectScale,RedrectScale,1 });
		}

		glm::mat4 vpmatrix = glm::mat4{ 1 };
		glm::vec2 RedPos = glm::vec2{ 1 };
		auto translate = glm::mat4{ 1 };
		{
			auto camera = winWindow->getCamera();

			{
				//translate
				auto cameraMoveDirection = camera->Position - camera->OriginPos();
				cameraMoveDirection *= RedrectScale;
				translate = glm::translate(translate, cameraMoveDirection);
			}

			auto cameraOriginPos = camera->OriginPos();
			RedPos.x = 0 + cameraOriginPos.x;
			RedPos.y = 0 - cameraOriginPos.y;

			auto viewMatrix = glm::lookAt(cameraOriginPos, cameraOriginPos + camera->Front, camera->Up);
			auto perspectiveMatrix = winWindow->getPerspectiveMatrix();
			vpmatrix = perspectiveMatrix * viewMatrix * translate * scaleMatrix;
		}



		rtTexture.itemRect->Width(windowWidth);
		rtTexture.itemRect->Height(windowHeight);
		rtTexture.itemRect->setPos({ RedPos.x,RedPos.y });
		CGRender_SetLineWidth(contextid, 10);
		rtTexture.itemRect->Render(contextid, vpmatrix);//todo  只要执行这个，就会有问题
		CGRender_SetLineWidth(contextid, 1);
		if (0)
		{
			CGRender_SaveTextue(contextid, rtTexture.hTargetTexture, L"D:/testImg.png");
		}
		CGRender_SetRenderTarget(contextid, oldTarget);
	}

	uint32_t rtRectWidth = windowWidth * 0.1;
	uint32_t rtRectHeight = windowHeight * 0.1;

	CGRECT rect{ windowWidth - rtRectWidth,0,rtRectWidth,rtRectHeight };
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


