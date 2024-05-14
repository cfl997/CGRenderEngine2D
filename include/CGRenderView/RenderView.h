#ifndef _RENDERVIEW_H_
#define _RENDERVIEW_H_

#include "CGRenderView_Export.h"
#include "CGWindow.h"
#include <memory>

namespace CGRender
{


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
#endif // !_RENDERVIEW_H_
