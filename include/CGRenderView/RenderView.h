#ifndef _RENDERVIEW_H_
#define _RENDERVIEW_H_

#include "CGRenderView_Export.h"
#include "CGRenderType.h"

namespace CGRender
{
	class CGRENDERVIEW_API CGRenderView
	{
	public:
		CGRenderView(unsigned int width, unsigned int height, void* parent);
		~CGRenderView();
	private:
		//CGRenderView() = delete;
	public:
		/*
		* window
		*/
		bool createWindow(void* parentWindow, const char* Title, unsigned int  windowWidth, unsigned int  windowHeight, void* share_Window, WindowType type);
		bool deleteWindow(const char* title);
	public:
		void* getWindowByTitle(const char* title);
		void* getWindowByTitle(const wchar_t* title);
	public:
		/*
		* c#
		* event
		*/
		bool addCGRenderEvent(CGRenderEventType EventType);
		bool addImage(const char* windowTitle, const char* imagePath);
		bool resizeWindow(const char* windowTitle, unsigned int width, unsigned int height);
	public:
		/*
		* render
		*/
		void Render();
	private:
		struct PrivateRenderView;
		PrivateRenderView* m_priv;
	};
}


extern "C" int testswig_add(int a, int b);
extern "C" int testswig_char(char a[], int b);
#endif // !_RENDERVIEW_H_
