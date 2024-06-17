#include "Render2D.h"
#include "CGRenderView.h"
#include "CGRender.h"
#include "CGData.h"

#include <mutex>

using namespace CGRender;

#define NULL_Return(x) if(!x)return;

#define SAFE_DELETE(x) if(x)\
{\
delete x;\
x=nullptr;\
}

struct PrivateData
{
	WindowsWindow* window = nullptr;
	bool Release = false;
	std::recursive_mutex mutex;
};
Render2D::Render2D()
{
	PrivateData* m_priv = new PrivateData;
	impl = m_priv;

}

Render2D::~Render2D()
{

	auto& d = *(PrivateData*)impl;
	SAFE_DELETE(d.window);
	SAFE_DELETE(impl);
}

void Render2D::Create(void* parent, int width, int height)
{
	CGPath_Init();
	auto& d = *(PrivateData*)impl;
	WindowProps windowProps{ parent,const_cast<wchar_t*>(g_windowMainStr.c_str()), (uint32_t)width, (uint32_t)height,nullptr ,WindowType::Window_Main };
	d.window = dynamic_cast<CGRender::WindowsWindow*>(CGRender::Window::Create(windowProps));
	d.window->addCGRenderEvent(CGRenderEventType::RenderEvent_Default);
	//d.window->addCGRenderEvent(CGRenderEventType::RenderEvent_Rectangle);

	SetMode(0);
}

void Render2D::Release()
{
	auto& d = *(PrivateData*)impl;
	if (d.Release)
		return;
	std::lock_guard lock(d.mutex);
	d.Release = true;

	SAFE_DELETE(d.window);
}

void Render2D::Resize(int width, int height)
{
	auto& d = *(PrivateData*)impl;
	NULL_Return(d.window);
	d.window->ResizeWindow(width, height);
}

void Render2D::SetMode(int mode)
{
	auto& d = *(PrivateData*)impl;
	NULL_Return(d.window);

	d.window->RenderMode((CGRenderMode)mode);
}

void Render2D::SetHeight(int height)
{
	auto& d = *(PrivateData*)impl;
	NULL_Return(d.window);
	if (d.window->RenderMode() != CGRenderMode::RenderMode_rolling)
		return;
	d.window->setRollingHeight(height);
}

void Render2D::SetDirection(int direction)
{
	auto& d = *(PrivateData*)impl;
	NULL_Return(d.window);
	if (d.window->RenderMode() != CGRenderMode::RenderMode_rolling)
		return;
	if (direction != 0 && direction != 1)
		return;
	d.window->setRollingDirection(direction);
}

void Render2D::SetImage(void* data, int width, int height, bool dual)
{
	auto& d = *(PrivateData*)impl;
	NULL_Return(d.window);
	auto layer = d.window->getCurLayer();
	NULL_Return(layer);
	auto items = layer->getItem(CGData::CGItemType::CGItemImage);
	if (items.empty())
	{
		d.window->addImage(data, width, height, dual, GLTextureType::GLTexture_Raw16);
		return;
	}
	if (d.window->RenderMode() == CGRenderMode::RenderMode_rolling)
	{
		std::swap(width, height);
	}
	CGData::CGItemImage* image = dynamic_cast<CGData::CGItemImage*>(items.at(0));
	NULL_Return(image);
	image->updateData(data, width, height);
}

void Render2D::SetParameter(const Parameter& parameter)
{
	auto& d = *(PrivateData*)impl;
	NULL_Return(d.window);
	{
		//lbutton_mode
	}
	{
		//rbutton_mode
		CGRenderEventType type = (CGRenderEventType)parameter.rbutton_mode;
		d.window->addCGRenderEvent(type);
	}
	{

		WindowType type = WindowType(parameter.windowType);
		d.window->setWindowType(type);
	}
}

void Render2D::SetProperty(const Property& property)
{
	auto& d = *(PrivateData*)impl;
	NULL_Return(d.window);

	auto fn = [&](bool property,ShaderCodeName name){
		if (property)
		{
			d.window->getCurLayer()->addImageShader(name);
		}
		else
		{
			d.window->getCurLayer()->removeImageShader(name);
		}
		};

	fn(property.anticolor, ShaderCodeName::FS_Tex_Red_Invert);
	fn(property.windowWidthLevel, ShaderCodeName::CUDA_WindowWidthLevel);

	//fn(property.equalization)//todo

}

void Render2D::SetLabels(Label* labels, int count)
{
	auto& d = *(PrivateData*)impl;
	NULL_Return(d.window);
	auto layer = d.window->getCurLayer();
	for (int index = 0; index < count; index++)
	{
		CGData::CGItemRectangle* rect = new CGData::CGItemRectangle(d.window->ContextID());
		Label* data = &labels[index];
		rect->setPos({ data->x,data->y });
		rect->Label(data->code);
		rect->Width(data->w);
		rect->Height(data->h);
		layer->addItem(rect);
	}
}

void Render2D::GetLabels(Label* labels, int capacity)
{
	auto& d = *(PrivateData*)impl;
	NULL_Return(d.window);
	auto layer = d.window->getCurLayer();
	auto rectItems = layer->getItem(CGData::CGItemType::CGItemRectangle);
	int flag = 0;
	for (int i = 0; i < rectItems.size(); i++)
	{
		CGData::CGItemRectangle* rect = static_cast<CGData::CGItemRectangle*>(rectItems.at(i));
		if (rect->rectType() == CGData::RectangleType::Window)
			continue;
		flag++;
		labels[i].type = -1;
		labels[i].code = rect->Label();
		labels[i].x = rect->getPos().x;
		labels[i].y = rect->getPos().y;
		labels[i].w = rect->Width();
		labels[i].h = rect->Height();
		if (flag == capacity)
			break;
	}
}

void Render2D::Update()
{
	auto& d = *(PrivateData*)impl;
	NULL_Return(d.window);
	std::lock_guard lock(d.mutex);
	if (!d.Release)
		d.window->Render();
}



void Render2D::SetWindowWidthLevel(int windowWidth, int windowLevel)
{
	auto& d = *(PrivateData*)impl;
	d.window->setWindowWidthLevel(windowWidth, windowLevel);
}
