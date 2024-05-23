#ifndef _RENDERVIEW_H_
#define _RENDERVIEW_H_

#include "CGRenderView_Export.h"
#include <memory>
#include <string>

#ifdef CGRENDERVIEW_EXPORTS
#define CGRENDERVIEW_API __declspec(dllexport)
#else
#define CGRENDERVIEW_API __declspec(dllimport)
#endif // CGRENDERVIEW_EXPORTS



namespace CGRender
{

	class Window;
	struct WindowProps;
	enum RenderViewCallBack;
	enum WindowType;

	class CGRENDERVIEW_API CGRenderView
	{
	public:
		CGRenderView(unsigned int width, unsigned int height, void* parent);
		~CGRenderView();
	private:
		CGRenderView() = delete;
	public:
		/*
		* window
		*/
		bool createWindow(const CGRender::WindowProps& windowsProps);
		bool createWindow(void* parentWindow,const std::string& Title, unsigned int  windowWidth, unsigned int  windowHeight, void* share_Window, WindowType type);

		bool deleteWindow(const std::wstring& title);
		bool deleteWindow(const std::string& title);
		bool deleteWindow(const wchar_t* title);

		std::shared_ptr<CGRender::Window>getWindowByTitle(const std::wstring& title);
		std::shared_ptr<CGRender::Window>getWindowByTitle(const std::string& title);

		bool closeWindow(const std::wstring& title);
	public:
		/*
		* event
		*/
	public:
		/*
		* render
		*/
		void Render();
	public:
		/*
		* Texture
		*/
	private:
		struct PrivateRenderView;
		PrivateRenderView* m_priv;
	private:
		void WindowCallBack(RenderViewCallBack Enum, std::wstring& title);
	};

}

extern "C" int testswig_add(int a, int b);
extern "C" int testswig_char(char a[], int b);
extern "C" int testswig_string(std::string a);
#endif // !_RENDERVIEW_H_
