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

	class CGRENDERVIEW_API CGRenderView
	{
	public:
		CGRenderView(uint32_t width, uint32_t height, void* parent);
		~CGRenderView();
	private:
		CGRenderView() = delete;
	public:
		/*
		* window
		*/
		bool createWindow(const CGRender::WindowProps& windowsProps);

		bool deleteWindow(const std::wstring& title);

		//std::shared_ptr<CGRender::Window>getWindowByType(CGRender::WindowType type);
		std::shared_ptr<CGRender::Window>getWindowByTitle(const std::wstring& title);

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
#endif // !_RENDERVIEW_H_
