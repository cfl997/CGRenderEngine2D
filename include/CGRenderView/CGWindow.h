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

	enum WindowType
	{
		Window_Main = 0,
		Window_unknow = 0xff
	};
	//WindowsWindow
	struct CGRENDERVIEW_API WindowProps
	{
		void* parentWindow;
		std::wstring Title;
		uint32_t Width;
		uint32_t Height;
		//GLFWwindow* share_Window;
		void* share_Window;
		WindowType type;

		WindowProps(void* parent = nullptr, const std::wstring& title = L"OpenGL",
			uint32_t width = 1280,
			uint32_t height = 720,
			void* shareWindow = nullptr, WindowType windowType = WindowType::Window_Main)
			:parentWindow(parent), Title(title), Width(width), Height(height), share_Window(shareWindow), type(windowType)
		{
		}
	};

	class CGRENDERVIEW_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void Render() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual void* GetWindowHwnd()const = 0;

		virtual bool addCGRenderEvent(CGRenderEvent* renderEvent) = 0;
		virtual bool addCGRenderEvent(CGRenderEventType EventType) = 0;
		virtual bool removeCGRenderEvent(CGRenderEventType renderEventType) = 0;

		virtual int ContextID() = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};
}


#endif // !_CGWINDOW_H_
