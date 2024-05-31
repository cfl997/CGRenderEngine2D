#pragma once

#include "CGWindow.h"

struct GLFWwindow;
namespace CGData
{
	class CGLayer;
}
namespace CGRender
{
	static const std::wstring g_windowMainStr = L"CGRenderViewMainWindow";
	class CGRenderEvent;
	class CGCamera;
	struct WindowProps;
	class Window;

	class CGRENDERVIEW_API WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

	public:
		inline uint32_t GetWidth() const override;
		inline uint32_t GetHeight() const override;

		void ResizeWindow(unsigned int width, unsigned int height);
	public:
		void Render() override;
		// Window attributes
		inline void SetEventCallback(const Window::EventCallbackFn& callback) override;
		inline void SetRenderViewCallback(const Window::RenderViewCallBackFn& callback)override;
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;


		const WindowType getWindowType();
		void setWindowType(WindowType type);
	public:
		const glm::mat4& getVPMatrix();

	public:
		inline virtual void* GetNativeWindow() const { return m_Window; }
		virtual void* GetWindowHwnd()const;

		bool OnWindowClose()override;
	public:
		/*
		* 增加事件
		*/
		virtual bool addCGRenderEvent(CGRenderEventType EventType) override;
		virtual bool removeCGRenderEvent(CGRenderEventType renderEventType)override;
		void AfterEvent();
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

	public:
		void addImage(const std::wstring& path);
		virtual void addImage(const std::string& path)override;
		void addImage(void* data, int width, int height, bool dual, GLTextureType textureType = GLTextureType::GLTexture_Normal2DTex);
		void setRollingHeight(float height);
		void setRollingDirection(int direction);
		void RenderMode(CGRenderMode renderMode);
		CGRenderMode RenderMode();
	public:
		/*
		* another window
		*/
		void syncWindowByParent(WindowsWindow* parent);

	private:
		void renderTest();

	private:
		void Init(const WindowProps& props);
		void Shutdown();
		virtual bool needClose()override;
	private:
		GLFWwindow* m_Window;

		struct WindowData;
		WindowData* m_priv = nullptr;
	private:
		WindowsWindow() = delete;
	};

}