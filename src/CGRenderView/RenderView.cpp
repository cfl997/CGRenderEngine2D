#include "RenderView.h"
#include "CGCore.h"

#include <map>
#include <mutex>
#include <memory>

#include "CGRender.h"
#include "CGWindowsWindos.h"

using namespace CGRender;


struct CGRenderView::PrivateRenderView
{
	bool m_Running = false;
	std::map<std::wstring, std::shared_ptr<CGRender::Window>> cgWindows;

	std::shared_ptr<CGRender::Window>getMainWindos() { return cgWindows[g_windowMainStr]; }
};


CGRender::CGRenderView::CGRenderView(uint32_t width, uint32_t height, void* parent) :m_priv(new PrivateRenderView)
{
	// 设置C语言的字符集环境  

	setlocale(LC_ALL, "");

	CGPath_Init();

	auto& d = *m_priv;
	d.m_Running = true;
	//手动创建主窗口，并作为主资源
	std::shared_ptr<CGRender::Window>window = std::shared_ptr<CGRender::Window>(CGRender::Window::Create({ parent, g_windowMainStr, width, height,nullptr ,WindowType::Window_Main }));
	d.cgWindows[g_windowMainStr] = window;
}
CGRenderView::~CGRenderView()
{
	CGPath_Release();

	auto& d = *m_priv;
	d.m_Running = false;
	d.cgWindows.clear();

	SAFE_DELETE(m_priv);
}

bool CGRender::CGRenderView::createWindow(const CGRender::WindowProps& windowsProps)
{
	auto& d = *m_priv;
	if (windowsProps.type == WindowType::Window_Main || windowsProps.type == WindowType::Window_unknow)
		return false;
	std::shared_ptr<CGRender::Window>window = std::shared_ptr<CGRender::Window>(CGRender::Window::Create({ windowsProps.parentWindow, windowsProps.Title, windowsProps.windowWidth,windowsProps.windowHeight,windowsProps.share_Window }));
	d.cgWindows[windowsProps.Title] = window;
	return true;
}

bool CGRender::CGRenderView::deleteWindow(const std::wstring& title)
{
	auto& d = *m_priv;
	auto windowIterator = d.cgWindows.find(title);
	if (windowIterator == d.cgWindows.end())
		return false;
	d.cgWindows.erase(windowIterator);
	return true;
}

std::shared_ptr<CGRender::Window> CGRender::CGRenderView::getWindowByTitle(const std::wstring& title)
{
	auto& d = *m_priv;
	return d.cgWindows[title];
}

//std::shared_ptr<CGRender::Window> CGRender::CGRenderView::getWindowByType(const std::wstring& title)
//{
//	auto& d = *m_priv;
//	return d.cgWindows[title];
//}


void CGRender::CGRenderView::Render()
{
	auto& d = *m_priv;

	//while (d.m_Running)//这个交给qt，不然事件冲突
	{
		for (auto& win : d.cgWindows)
		{
			win.second->Render();
		}
	}
}


