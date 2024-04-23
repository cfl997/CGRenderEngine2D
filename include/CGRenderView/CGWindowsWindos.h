#pragma once

#include "CGWindow.h"

//#include <GLFW/glfw3.h>
#include "CGglm.h"

//#include "CGData.h"
struct GLFWwindow;
namespace CGData
{
	class CGLayer;
}
namespace CGRender
{
	class CGRenderEvent;
	class CGCamera;
	class CGRENDERVIEW_API WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void Render() override;

		inline uint32_t GetWidth() const override;
		inline uint32_t GetHeight() const override;

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override;
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;


		const WindowType getType();

	public:
		inline virtual void* GetNativeWindow() const { return m_Window; }
		virtual void* GetWindowHwnd()const;

		void OnWindowClose();
	public:
		/*
		* 增加事件
		*/
		virtual bool addCGRenderEvent(CGRenderEvent* renderEvent)override;
		virtual bool addCGRenderEvent(CGRenderEventType EventType) override;
		virtual bool removeCGRenderEvent(CGRenderEventType renderEventType)override;
	public:
		int ContextID()override;
	public:
		/*
		* camera
		*/
		CGCamera* getCamera()noexcept;
	public:
		/*
		* layer
		*/
		CGData::CGLayer* getCurLayer()noexcept;
	public:
		/*
		* matrix view and perspective
		*/
		const glm::mat4 getViewMatrix()const;
		const glm::mat4& getPerspectiveMatrix()noexcept;

	private:
		void renderTest();

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;

		struct WindowData;
		WindowData* m_priv = nullptr;
	private:
		WindowsWindow() = delete;
	};

}