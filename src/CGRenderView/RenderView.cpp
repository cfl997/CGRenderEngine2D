#include "RenderView.h"
#include "CGCore.h"

#include <map>
#include <mutex>
#include <memory>

#include "CGWindow.h"
#include "CGWindowsWindos.h"
#include "CGRender.h"

using namespace CGRender;


struct CGRenderView::PrivateRenderView
{
	bool m_Running = false;
	std::recursive_mutex windowMutex;
	std::map<std::wstring, std::shared_ptr<CGRender::Window>> cgWindows;

	std::shared_ptr<CGRender::Window>getMainWindos() { return cgWindows[g_windowMainStr]; }

	void needCloseWindow();
	void WindowCallBack(RenderViewCallBack Enum, std::wstring& title);
	bool closeWindow(const std::wstring& title);
	bool deleteWindow(const std::wstring& title);

};


CGRender::CGRenderView::CGRenderView(unsigned int width, unsigned int height, void* parent) :m_priv(new PrivateRenderView)
{
	// 设置C语言的字符集环境  

	setlocale(LC_ALL, "");

	CGPath_Init();

	auto& d = *m_priv;
	d.m_Running = true;
	//手动创建主窗口，并作为主资源
	WindowProps windowProps{ parent,const_cast<wchar_t*>(g_windowMainStr.c_str()), width, height,nullptr ,WindowType::Window_Main };
	std::shared_ptr<CGRender::Window>window = std::shared_ptr<CGRender::Window>(CGRender::Window::Create(windowProps));
	window->SetRenderViewCallback(std::bind(&CGRenderView::PrivateRenderView::WindowCallBack, m_priv, std::placeholders::_1, std::placeholders::_2));
	d.cgWindows[g_windowMainStr] = window;
	window->addCGRenderEvent(CGRenderEventType::RenderEvent_Default);//先构建对象再创建事件

#ifdef DEBUG
	std::cout << "cfl :CGRenderView init successful!" << std::endl;
#endif // DEBUG
}
CGRenderView::~CGRenderView()
{
	CGPath_Release();

	auto& d = *m_priv;
	d.m_Running = false;
	d.cgWindows.clear();

	SAFE_DELETE(m_priv);
}

bool CGRender::CGRenderView::createWindow(void* parentWindow, const char* Title, unsigned int windowWidth, unsigned int windowHeight, void* share_Window, WindowType type)
{
	CGRender::WindowProps windowsProps{ parentWindow,utf82wstr(Title),windowWidth,windowHeight,share_Window,type };

	auto& d = *m_priv;
	if (windowsProps.type == WindowType::Window_Main || windowsProps.type == WindowType::Window_unknow)
		return false;
	WindowProps windowProps{ windowsProps.parentWindow, windowsProps.Title, windowsProps.windowWidth,windowsProps.windowHeight,windowsProps.share_Window };
	std::shared_ptr<CGRender::Window>window = std::shared_ptr<CGRender::Window>(CGRender::Window::Create(windowProps));
	window->SetRenderViewCallback(std::bind(&CGRenderView::PrivateRenderView::WindowCallBack, m_priv, std::placeholders::_1, std::placeholders::_2));
	d.cgWindows[windowsProps.Title] = window;
	window->addCGRenderEvent(CGRenderEventType::RenderEvent_Default);//先构建对象再创建事件
	return true;
}

bool CGRender::CGRenderView::deleteWindow(const char* title)
{
	std::wstring wtitle = utf82wstr(title);
	auto& d = *m_priv;
	d.deleteWindow(wtitle);
	return true;
}

void* CGRender::CGRenderView::getWindowByTitle(const char* title)
{
	std::wstring wtitle=utf82wstr(title);
	return getWindowByTitle(wtitle.c_str());
}

void* CGRender::CGRenderView::getWindowByTitle(const wchar_t* title)
{
	auto& d = *m_priv;
	auto window = d.cgWindows[title];
	if (window)
	{
		return window.get();
	}
	return nullptr;
}

//bool CGRender::CGRenderView::deleteWindow(const std::wstring& title)
//{
//	auto& d = *m_priv;
//	std::lock_guard Lock(d.windowMutex);
//	auto windowIterator = d.cgWindows.find(title);
//	if (windowIterator == d.cgWindows.end())
//		return false;
//	d.cgWindows.erase(windowIterator);
//	return true;
//}
//
//bool CGRender::CGRenderView::deleteWindow(const std::string& title)
//{
//	return deleteWindow(utf82wstr(title));
//}
//
//bool CGRender::CGRenderView::deleteWindow(const wchar_t* title)
//{
//	std::wstring wtitle{ title };
//	return deleteWindow(wtitle);
//}

//CGRender::Window* CGRender::CGRenderView::getWindowByTitle(const std::wstring& title)
//{
//	auto& d = *m_priv;
//	return d.cgWindows[title].get();
//}
//
//CGRender::Window* CGRender::CGRenderView::getWindowByTitle(const std::string& title)
//{
//	return getWindowByTitle(utf82wstr(title));
//}

bool CGRender::CGRenderView::PrivateRenderView::closeWindow(const std::wstring& title)
{
	std::lock_guard Lock(windowMutex);
	deleteWindow(title);
	return false;
}

bool CGRender::CGRenderView::addCGRenderEvent(CGRenderEventType EventType)
{
	auto& d = *m_priv;
	auto window = d.cgWindows[g_windowMainStr];
	if (window)
	{
		return window->addCGRenderEvent(EventType);
	}
	return false;
}

bool CGRender::CGRenderView::addImage(const char* windowTitle, const char* imagePath)
{
	auto& d = *m_priv;
	std::wstring wtitle = utf82wstr(windowTitle);
	auto window = d.cgWindows[wtitle];
	if (window)
	{
		window->addImage(imagePath);
		return true;
	}
	return false;
}

bool CGRender::CGRenderView::resizeWindow(const char* windowTitle, unsigned int width, unsigned int height)
{
	auto& d = *m_priv;
	std::wstring wtitle = utf82wstr(windowTitle);
	auto window = d.cgWindows[wtitle];
	WindowsWindow* windowsWindow = dynamic_cast<WindowsWindow*>(window.get());
	if (window)
	{
		windowsWindow->ResizeWindow(width, height);
		return true;
	}
	return false;
}

// 创建时钟
std::chrono::steady_clock::time_point start, end;


void CGRender::CGRenderView::Render()
{
	auto& d = *m_priv;

#ifdef DEBUG
	// 开始计时
	start = std::chrono::steady_clock::now();
#endif // DEBUG

	//while (d.m_Running)//这个交给qt，不然事件冲突
	d.needCloseWindow();

	std::lock_guard lock(d.windowMutex);

	{
		for (auto& win : d.cgWindows)
		{
			win.second->Render();
		}
	}
#ifdef DEBUG
	// 停止计时
	end = std::chrono::steady_clock::now();
	// 计算时间差并转换为毫秒
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto durationnano = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	// 输出结果
	//GLRender_LOG("CGRenderVeiw - Render time :", duration.count());
	std::cout << "CGRenderView - Render time ; " << duration.count() << "ms" << std::endl;
	std::cout << "CGRenderView - Render time ; " << durationnano.count() << "nanos" << std::endl;
	std::cout << "CGRenderView - Render time ; " << 1000000000 / durationnano.count() << "fps" << std::endl;
#endif // DEBUG

}

void CGRender::CGRenderView::PrivateRenderView::WindowCallBack(RenderViewCallBack Enum, std::wstring& title)
{
	GLRender_LOG("CGRender::CGRenderView::WindowCallBack", Enum);
	switch (Enum)
	{
	case RenderViewCallBack::closeWindow:
		closeWindow(title);

		break;
	case RenderViewCallBack::RenderViewCallBack_unKnow:
		break;
	default:
		break;
	}
}

bool CGRenderView::PrivateRenderView::deleteWindow(const std::wstring& title)
{
	std::lock_guard Lock(windowMutex);
	auto windowIterator = cgWindows.find(title);
	if (windowIterator == cgWindows.end())
		return false;
	cgWindows.erase(windowIterator);
	return true;
}



void CGRenderView::PrivateRenderView::needCloseWindow()
{
	windowMutex.lock();
	for (auto iter = cgWindows.begin(); iter != cgWindows.end();)
	{
		if (iter->second->needClose())
		{
			iter = cgWindows.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	windowMutex.unlock();
}

int testswig_add(int a, int b)
{
	return a + b;
}

int testswig_char(char a[], int b)
{
	return 0;
}

