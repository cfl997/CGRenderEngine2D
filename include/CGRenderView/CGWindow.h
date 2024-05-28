#ifndef _CGWINDOW_H_
#define _CGWINDOW_H_

#include "CGRenderMacros.h"
#include <functional>
#include "CGRenderType.h"
#include "CGRenderView_Export.h"


namespace CGRender
{
	class Event;
	class CGRenderEvent;


	//WindowsWindow
	struct CGRENDERVIEW_API WindowProps
	{
		void* parentWindow;
		std::wstring Title;
		uint32_t windowWidth;
		uint32_t windowHeight;
		//GLFWwindow* share_Window;
		void* share_Window;
		WindowType type;

		WindowProps(void* parent = nullptr, std::wstring title = L"OpenGL",
			uint32_t width = 1280,
			uint32_t height = 720,
			void* shareWindow = nullptr, WindowType windowType = WindowType::Window_Main)
			:parentWindow(parent), Title(title), windowWidth(width), windowHeight(height), share_Window(shareWindow), type(windowType)
		{
		}
	};



	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		using RenderViewCallBackFn = std::function<void(RenderViewCallBack, std::wstring&)>;

		virtual ~Window() = default;

		virtual void Render() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetRenderViewCallback(const RenderViewCallBackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual void* GetWindowHwnd()const = 0;

		virtual bool addCGRenderEvent(CGRenderEventType EventType) = 0;
		virtual bool removeCGRenderEvent(CGRenderEventType renderEventType) = 0;

		virtual void addImage(const std::string& imagePath) = 0;

		virtual int ContextID() = 0;
		virtual bool needClose() = 0;
		virtual bool OnWindowClose() = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};
}


#endif // !_CGWINDOW_H_
