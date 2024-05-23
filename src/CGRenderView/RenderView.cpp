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
	window->SetRenderViewCallback(std::bind(&CGRenderView::WindowCallBack, this, std::placeholders::_1, std::placeholders::_2));
	d.cgWindows[g_windowMainStr] = window;

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

bool CGRender::CGRenderView::createWindow(const CGRender::WindowProps& windowsProps)
{
	auto& d = *m_priv;
	if (windowsProps.type == WindowType::Window_Main || windowsProps.type == WindowType::Window_unknow)
		return false;
	WindowProps windowProps{ windowsProps.parentWindow, windowsProps.Title, windowsProps.windowWidth,windowsProps.windowHeight,windowsProps.share_Window };
	std::shared_ptr<CGRender::Window>window = std::shared_ptr<CGRender::Window>(CGRender::Window::Create(windowProps));
	window->SetRenderViewCallback(std::bind(&CGRenderView::WindowCallBack, this, std::placeholders::_1, std::placeholders::_2));
	d.cgWindows[windowsProps.Title] = window;
	return true;
}

bool CGRender::CGRenderView::createWindow(void* parentWindow, const std::string& Title, unsigned int windowWidth, unsigned int windowHeight, void* share_Window, WindowType type)
{
	return createWindow({ parentWindow,utf82wstr(Title),windowWidth,windowHeight,share_Window,type });
}


bool CGRender::CGRenderView::deleteWindow(const std::wstring& title)
{
	auto& d = *m_priv;
	std::lock_guard Lock(d.windowMutex);
	auto windowIterator = d.cgWindows.find(title);
	if (windowIterator == d.cgWindows.end())
		return false;
	d.cgWindows.erase(windowIterator);
	return true;
}

bool CGRender::CGRenderView::deleteWindow(const std::string& title)
{
	return deleteWindow(utf82wstr(title));
}

bool CGRender::CGRenderView::deleteWindow(const wchar_t* title)
{
	std::wstring wtitle{ title };
	return deleteWindow(wtitle);
}

std::shared_ptr<CGRender::Window> CGRender::CGRenderView::getWindowByTitle(const std::wstring& title)
{
	auto& d = *m_priv;
	return d.cgWindows[title];
}

std::shared_ptr<CGRender::Window> CGRender::CGRenderView::getWindowByTitle(const std::string& title)
{
	return getWindowByTitle(utf82wstr(title));
}

bool CGRender::CGRenderView::closeWindow(const std::wstring& title)
{
	auto& d = *m_priv;
	std::lock_guard Lock(d.windowMutex);
	deleteWindow(title);
	return false;
}

// 创建时钟
std::chrono::steady_clock::time_point start, end;


void CGRender::CGRenderView::Render()
{
	auto& d = *m_priv;


	// 开始计时
	start = std::chrono::steady_clock::now();




	//while (d.m_Running)//这个交给qt，不然事件冲突
	d.needCloseWindow();

	std::lock_guard lock(d.windowMutex);

	{
		for (auto& win : d.cgWindows)
		{
			win.second->Render();
		}
	}

	// 停止计时
	end = std::chrono::steady_clock::now();

	// 计算时间差并转换为毫秒
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	// 输出结果
	GLRender_LOG("CGRenderVeiw - Render tiem :", duration.count());
}

void CGRender::CGRenderView::WindowCallBack(RenderViewCallBack Enum, std::wstring& title)
{
	GLRender_LOG("CGRender::CGRenderView::WindowCallBack", Enum);
	switch (Enum)
	{
	case CGRender::closeWindow:
		closeWindow(title);

		break;
	case CGRender::RenderViewCallBack_unKnow:
		break;
	default:
		break;
	}
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

int testswig_string(std::string a)
{
	return 0;
}
