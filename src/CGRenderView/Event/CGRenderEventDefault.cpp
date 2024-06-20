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

	bool isleftPress = false;
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

	glm::vec2 pressPos = glm::vec2(0);

	//ImageEdgePos
	void ImageEdgePosMove(float x, float y);
	bool xChange = true;
	bool yChange = true;
	void ImageEdgePosScoll();
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
	d.ImageEdgePosScoll();

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

	d.ImageEdgePosMove(x, y);

	d.pressPos = glm::vec2(x, y);

	winWindow->getCamera()->ProcessMouseMoveXY(x, y, d.xChange, d.yChange);

	winWindow->getCurLayer()->ProcessMouseMoveXY(x, y, d.xChange, d.yChange);

	if (d.rtTexture.isleftPress)
	{
		d.needRenderRect();
	}
	return true;
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

	d.rtTexture.isleftPress = true;
	d.pressPos = glm::vec2(x, y);
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

	d.rtTexture.isleftPress = false;
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

	//if (winWindow->RenderMode() == CGRenderMode::RenderMode_rolling)
	//	return;

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

void CGRenderEventDefault::PrivateData::ImageEdgePosMove(float x, float y)
{
	xChange = true;
	yChange = true;
	CGData::CGLayer* layer = winWindow->getCurLayer();
	auto items = layer->getItem(CGData::CGItemType::CGItemImage);
	if (!items.empty())
	{
		CGData::CGItemImage* image = dynamic_cast<CGData::CGItemImage*>(items.at(0));
		int imageWidth = image->worldWidth();
		int imageHeight = image->worldHeight();

		std::swap(imageWidth, imageHeight);

		int windowWidth = winWindow->GetWidth();
		int windowHeight = winWindow->GetHeight();

		auto camera = winWindow->getCamera();
		float cameraScale = camera->ScaleCoefficient();

		imageWidth *= cameraScale;
		imageHeight *= cameraScale;

		glm::vec3 originPos = camera->OriginPos();
		glm::vec3 cameraCurPos = camera->Position;

		float moveWidth = std::abs(imageWidth - windowWidth) / 2.;
		float moveHeight = std::abs(imageHeight - windowHeight) / 2.;

		int i = 0;

		glm::vec2 offset = glm::vec2{ x,y } - pressPos;
#ifdef USE_ORTHO
		offset *= cameraScale;
#endif // USE_ORTHO
		//glm::vec3 cameraCanMovePos = originPos;
		cameraCurPos.x -= offset.x;
		cameraCurPos.y -= offset.y;

		if (std::abs(cameraCurPos.x - originPos.x) > moveWidth)
			xChange = false;
		if (std::abs(cameraCurPos.y - originPos.y) > moveHeight)
			yChange = false;

	}
}

void CGRenderEventDefault::PrivateData::ImageEdgePosScoll()
{
	CGData::CGLayer* layer = winWindow->getCurLayer();
	auto items = layer->getItem(CGData::CGItemType::CGItemImage);
	if (items.empty())
		return;
	CGData::CGItemImage* image = dynamic_cast<CGData::CGItemImage*>(items.at(0));
	int imageWidth = image->worldWidth();
	int imageHeight = image->worldHeight();

	std::swap(imageWidth, imageHeight);

	int windowWidth = winWindow->GetWidth();
	int windowHeight = winWindow->GetHeight();

	auto camera = winWindow->getCamera();
	float cameraScale = camera->ScaleCoefficient();

	imageWidth *= cameraScale;
	imageHeight *= cameraScale;

	glm::vec3 originPos = camera->OriginPos();
	glm::vec3 cameraCurPos = camera->Position;

	float moveWidth = std::abs(imageWidth - windowWidth) / 2.;
	float moveHeight = std::abs(imageHeight - windowHeight) / 2.;

	{
		float xCameraOffset = cameraCurPos.x - originPos.x;
		if (std::abs(xCameraOffset) > moveWidth)
			xCameraOffset < 0 ? (camera->Position.x += (std::abs(xCameraOffset) - moveWidth)) : camera->Position.x -= (std::abs(xCameraOffset) - moveWidth);
	}
	{
		float yCameraOffset = cameraCurPos.y - originPos.y;
		if (std::abs(yCameraOffset) > moveHeight)
			yCameraOffset < 0 ? (camera->Position.y += (std::abs(yCameraOffset) - moveHeight)) : camera->Position.y -= (std::abs(yCameraOffset) - moveHeight);
	}
}


