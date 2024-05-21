#include "CGWindowsWindos.h"

//#include "GLCore/Events/ApplicationEvent.h"
//#include "GLCore/Events/MouseEvent.h"
//#include "GLCore/Events/KeyEvent.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_NATIVE_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Event/WindowsEvent.h"
#include "Event/MouseEvent.h"
#include "Event/KeyEvent.h"
#include "Event/CGRenderEvent.h"
#include "Event/CGRenderEventManager.h"
#include "Event/CGRenderEventFactory.h"


#include "CGRender.h"
#include "CGCamera.h"


#include <assert.h>
#include <iostream>

#include <codecvt>
#include <tchar.h>

#include <tuple>
#include <mutex>

#define use_opengl_4_6


#include "CGData.h"

namespace CGRender
{

	static bool s_GLFWInitialized = false;


	struct WindowsWindow::WindowData
	{
		//UI_window
		HWND hWnd;
		//window
		void* parent;
		std::wstring Title;
		uint32_t windowWidth, windowHeight;
		bool VSync;
		GLFWwindow* shareWindow;
		WindowType type;
		//event
		EventCallbackFn EventCallback = nullptr;
		RenderViewCallBackFn RenderViewCallBack = nullptr;
		bool needClose = false;
		std::unique_ptr<CGRenderEventManager>eventManager;
		//opengl
		int glContextID = -1;
		std::recursive_mutex renderMutex;
		//camera
		std::unique_ptr<CGCamera>camera = nullptr;
		glm::mat4 perspectiveMatrix = glm::mat4(1);

		//this
		WindowsWindow* super = nullptr;

		//layer
		std::unique_ptr<CGData::CGLayer>layer = nullptr;


		//imageData
		int imageWidth = -1;
		int imageHeight = -1;
#ifdef USE_ORTHO
#else
		int imageChange = false;
#endif // USE_ORTHO

		glm::mat4 getInverseScreenMatrix()
		{
			glm::mat4 persp = super->getPerspectiveMatrix();
			glm::mat4 view = camera->GetViewMatrix();
			return glm::inverse(persp * view);
		}

		std::tuple<float, float> getWorldPos(glm::vec2 pos)
		{
			auto worldPos = CGRender_GetWorldPos(pos, camera->GetViewMatrix(), super->getPerspectiveMatrix(), glm::vec4(0, 0, windowWidth, windowHeight));
			float scale = camera->ScaleCoefficient();
			worldPos /= scale;
			return std::make_tuple(worldPos.x, worldPos.y);//yes
		}
	};

	static void GLFWErrorCallback(int error, const char* description)
	{

		//LOG_ERROR("GLFW Error ({0}): {1}", error, description);
		std::cout << "GLFW ERROR (" << error << "): {" << description << "}" << std::endl;
		//assert(0);
	}


	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props) :m_priv(new WindowData)
	{
		m_priv->super = this;
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		if (m_priv)
		{
			if (m_priv->eventManager)
			{
				m_priv->eventManager.reset();
				m_priv->eventManager = nullptr;

			}
			m_priv->super = nullptr;

			delete m_priv;
			m_priv = nullptr;
		}
		Shutdown();
	}

	bool WindowsWindow::addCGRenderEvent(CGRenderEvent* renderEvent)
	{
		auto& d = *m_priv;
		if (!renderEvent)
			return false;
		d.eventManager->addCGRenderEvent(renderEvent);
		return true;
	}

	bool WindowsWindow::addCGRenderEvent(CGRenderEventType EventType)
	{
		CGRenderEvent* renderEvent = CGRenderEvnetFactory::instance()->createRenderEvent(EventType, this);
		return addCGRenderEvent(renderEvent);
	}

	bool WindowsWindow::removeCGRenderEvent(CGRenderEventType renderEventType)
	{
		auto& d = *m_priv;
		return d.eventManager->removeCGRenderEvent(renderEventType);
	}

	void WindowsWindow::AfterEvent()
	{
		auto& d = *m_priv;
		d.eventManager->setCurrentEvent(CGRenderEventType::RenderEvent_Default);
	}

	int WindowsWindow::ContextID()
	{
		auto& d = *m_priv;
		return d.glContextID;
	}

	CGCamera* WindowsWindow::getCamera()noexcept
	{
		auto& d = *m_priv;
		return d.camera.get();
	}

	CGData::CGLayer* WindowsWindow::getCurLayer()noexcept
	{
		auto& d = *m_priv;
		return d.layer.get();
	}


	const glm::mat4 WindowsWindow::getViewMatrix() const
	{
		auto& d = *m_priv;
		return d.camera->GetViewMatrix();
	}

	const glm::mat4& WindowsWindow::getPerspectiveMatrix()noexcept
	{
		auto& d = *m_priv;
		if (d.windowHeight <= 0)
		{
			__debugbreak();
			return std::move(glm::mat4{ 1 });
		}
#ifdef USE_ORTHO
		d.perspectiveMatrix = glm::ortho(0., static_cast<double>(d.windowWidth), 0., static_cast<double>(d.windowHeight), 0.1, 1000.);
#else
		double defaultZoom = d.camera->Zoom;
		if (d.imageWidth > 0 && d.imageHeight > 0 && d.imageChange)
		{
			d.imageChange = false;
			float imageProportion = (float)d.imageWidth / (float)d.imageHeight;
			float windowProportion = (float)d.windowWidth / (float)d.windowHeight;

			double realHeight = d.imageHeight;
			if (imageProportion > 1)
			{
				realHeight = d.imageWidth / windowProportion;
			}

			double opposite = realHeight;
			//double adjacent = 2460;//this is correct
			//double adjacent = 10000;// 
			double adjacent = 1000 - 0.1;// 

			defaultZoom = atan(opposite / adjacent) * 180 / glm::pi<double>();
			//d.camera->Zoom = defaultZoom * 2.;
			d.camera->Zoom = defaultZoom;
		}
		//右手坐标系，z越大，越近
		d.perspectiveMatrix = glm::perspective(glm::radians(defaultZoom), static_cast<double>(d.windowWidth) / static_cast<double>(d.windowHeight), 0.1, 1000.);
#endif // USE_ORTHO

		return d.perspectiveMatrix;
	}

	void WindowsWindow::addImage(const std::wstring& path)
	{
		auto& d = *m_priv;

		d.layer->removeItem(CGData::CGItemType::CGItemImage);

		CGData::CGItemImage* image = new CGData::CGItemImage(ContextID(), path);
		d.layer->addItem(image);

#ifdef USE_ORTHO
		float widthScale = static_cast<float> (d.windowWidth) / static_cast<float> (image->worldWidth());
		float heightScale = static_cast<float> (d.windowHeight) / static_cast<float> (image->worldHeight());
		float scale = widthScale < heightScale ? widthScale : heightScale;
		if (scale > g_Camera_SCALE[CAMERA_SCALE_NUMS - 1])
			scale = 1.0;
		d.camera->ScaleCoefficient(scale);
		d.camera->ImageMinSclae(scale);
		d.camera->resetPosition(d.windowWidth, d.windowHeight);
#else
		d.imageWidth = image->worldWidth();
		d.imageHeight = image->worldHeight();
		d.imageChange = true;
#endif // USE_ORTHO

	}

	void WindowsWindow::syncWindowByParent(WindowsWindow* parent)
	{
		auto& d = *m_priv;
		std::lock_guard lock(d.renderMutex);
		//test
		CGRECT imageSelectRect{ 0,0,500,300 };

		const auto pCamera = parent->getCamera();
		const float pSclae = pCamera->ScaleCoefficient();
		getCamera()->ScaleCoefficient(pSclae);
		getCamera()->ImageMinSclae(pSclae);


		const auto pLayer = parent->getCurLayer();
		const auto rects = pLayer->getItem(CGData::CGItemType::CGItemRectangle);
		if (rects.empty())
			return;
		const auto prect = rects.at(0);
		CGData::CGBoundingShape shape(prect);
		CGData::BoundingShape2D pShape2d;
		shape.Bounding2D(&pShape2d);

		auto Layer = getCurLayer();
		auto Image = Layer->getItem(CGData::CGItemType::CGItemImage).at(0);
		auto modelMatrix = Image->getModelMatrix();
		auto tanslate = glm::translate(modelMatrix, glm::vec3(-pShape2d.center, 0));
		Image->setModelMatrix(tanslate);

		//getCamera()->Position = glm::vec3{ pShape2d.center,getCamera()->Position.z };
	}

	void WindowsWindow::renderTest()
	{
		auto& d = *m_priv;

		int renderTarget = CGRender_GetRenderTarget(d.glContextID);
		if (1)
		{
			CGRender_SetViewport(d.glContextID, 0, 0, d.windowWidth, d.windowHeight);
			CGRender_SetScissor(d.glContextID, 0, 0, d.windowWidth, d.windowHeight);

			CGRender_ClearTexture(d.glContextID, renderTarget, 0x00ffff00);
			CGRender_ClearTexture(d.glContextID, renderTarget, 0x00000000);

			glm::mat4 transform = glm::mat4(1.0f);
			glm::mat4 view = getViewMatrix();
			glm::mat4 Perspective = getPerspectiveMatrix();


			glm::mat4 scale = d.camera->GetSalceMatrix();
			transform = transform * scale;

			glm::mat4 aa = Perspective * view * transform;//test

			d.layer->Render(d.glContextID, aa);

#ifdef DEBUG
			if (0)
			{
				CGRender_SaveTextue(d.glContextID, renderTarget, L"d:/rendertestTarget.png");
			}
#endif // DEBUG

			return;
		}
	}


	void WindowsWindow::Init(const WindowProps& props)
	{
		auto& d = *m_priv;
		d.parent = props.parentWindow;
		d.Title = props.Title;
		d.windowWidth = props.windowWidth;
		d.windowHeight = props.windowHeight;
		d.shareWindow = (GLFWwindow*)props.share_Window;
		d.type = props.type;

		if (!s_GLFWInitialized)
		{

			int success = glfwInit();
			assert(success);
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;

			TCHAR* szfile = CGPath_GetPath(CGPathType::CG_PATH_PLUGIN);
			{
#ifdef DEBUG
				std::cout << "CGRender_Init: " << szfile << std::endl;
#endif // DEBUG

				bool isInit = CGRender_Init(szfile);//是否包含glew32.dll
				assert(isInit);
			}
			//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#ifndef use_Window_Title
			// 设置窗口属性，隐藏标题栏
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);//
#endif // use_Window_Title

#ifdef use_opengl_4_6
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // use_opengl_4_6

		}//s_GLFWInitialized

		{
			//event
			d.eventManager = std::make_unique<CGRenderEventManager>();
			SetEventCallback(std::bind(&CGRenderEventManager::OnEvent, d.eventManager.get(), std::placeholders::_1));
			auto defaultRenderEvent = CGRenderEvnetFactory::instance()->createRenderEvent(RenderEvent_Default, this);
			addCGRenderEvent(defaultRenderEvent);
		}

		std::string title = wstr2utf8(d.Title);
		//这个会导致  发生偏移 c++ 
		int windowheigt = props.windowHeight;
		int windowWidth = props.windowWidth;

#ifndef use_Window_Title
		//m_Window = glfwCreateWindow(d.parent, (int)props.windowWidth, (int)props.windowHeight, title.c_str(), nullptr, d.shareWindow);
#else
		if (!d.parent)
		{
			//windowWidth += 25;
			windowheigt += 25;
		}
#endif // use_Window_Title
		m_Window = glfwCreateWindow(d.parent, windowWidth, windowheigt, title.c_str(), nullptr, d.shareWindow);
		glfwMakeContextCurrent(m_Window);

		{
			//d.glContext = new CGRenderGL::GLContext();
			// 获取窗口句柄
			d.hWnd = glfwGetWin32Window(m_Window);
			HGLRC hGLRC = glfwGetWGLContext(m_Window);
			//d.glContext->setWindowData(hwnd, hGLRC);
			d.glContextID = CGRender_CreateContext(d.hWnd, hGLRC, d.windowWidth, d.windowHeight);
		}

		CGRender_SetViewport(d.glContextID, 0, 0, d.windowWidth, d.windowHeight);
		CGRender_SetScissor(d.glContextID, 0, 0, d.windowWidth, d.windowHeight);
		{
			d.camera = std::make_unique<CGCamera>();
#ifdef USE_ORTHO
			d.camera->resetPosition(d.windowWidth, d.windowHeight);
#endif // USE_ORTHO
		}
		{
			d.layer = std::make_unique<CGData::CGLayer>(ContextID());
		}
		//cfl-test
		if (1)
		{
			//auto defaultRenderEvent = CGRenderEvnetFactory::instance()->createRenderEvent(RenderEvent_Rectangle, this);
			//addCGRenderEvent(defaultRenderEvent);
		}
		glfwSetWindowUserPointer(m_Window, m_priv);

		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				if (width <= 0 && height <= 0)
				{
					int i = 0;
					return;
				}
				data.windowWidth = width;
				data.windowHeight = height;

				//WindowResizeEvent event(width, height);
				//data.EventCallback(event);

//#ifndef use_Window_Title
//				glfwSetWindowSize((GLFWwindow*)data.super->GetNativeWindow(), width, height);
//#else
//				glfwSetWindowSize((GLFWwindow*)data.super->GetNativeWindow(), width, height + 25);
//#endif // use_Window_Title

#ifdef USE_ORTHO
				data.camera->resetPosition(data.windowWidth, data.windowHeight);
#endif // USE_ORTHO


				CGRender_SetViewport(data.glContextID, 0, 0, data.windowWidth, data.windowHeight);
				CGRender_SetScissor(data.glContextID, 0, 0, data.windowWidth, data.windowHeight);
				CGRender_ResizeWindow(data.glContextID, data.windowWidth, data.windowHeight);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				double x, y = 0;
				glfwGetCursorPos(window, &x, &y);
				auto [worldx, worldy] = data.getWorldPos({ x, y });
				//auto worldx = x;
				//auto worldy = y;

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button, worldx, worldy);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button, worldx, worldy);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				double x, y = 0;
				glfwGetCursorPos(window, &x, &y);

				auto [worldx, worldy] = data.getWorldPos({ x,y });

				MouseScrolledEvent event((float)xOffset, (float)yOffset, (float)worldx, (float)worldy);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				auto [worldx, worldy] = data.getWorldPos({ x,y });

				//auto worldx = x; auto worldy = y;

				MouseMovedEvent event((float)worldx, (float)worldy);
				data.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;
	}

	bool WindowsWindow::needClose()
	{
		return m_priv->needClose;
	}

	void WindowsWindow::Render()
	{
		auto& d = *m_priv;
		if (m_Window == nullptr)
			return;

		std::lock_guard lock(d.renderMutex);

#ifndef use_opengl_4_6
		//d.glContext->renderTest(1);
		double time = glfwGetTime();
		time = fmodl(time, 1);
		CGRender_RenderTest(d.glContextID, time);
#else
		renderTest();
		//CGRender_Render(1, 1, 2, GLPrimitiveTypes::TRIANGLELIST, 0, 0, 0);

#endif // !use_opengl_4_6

		glfwPollEvents();

#ifndef use_opengl_4_6
		glfwSwapBuffers(m_Window);//用present
#else
		CGRender_Present(d.glContextID);
#endif
	}

	inline uint32_t WindowsWindow::GetWidth() const
	{
		return m_priv->windowWidth;
	}

	inline uint32_t WindowsWindow::GetHeight() const
	{
		return m_priv->windowHeight;
	}

	inline void WindowsWindow::SetEventCallback(const EventCallbackFn& callback)
	{
		m_priv->EventCallback = callback;
	}

	void WindowsWindow::SetRenderViewCallback(const RenderViewCallBackFn& callback)
	{
		m_priv->RenderViewCallBack = callback;
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		auto& d = *m_priv;
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		d.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		auto& d = *m_priv;
		return d.VSync;
	}

	const WindowType WindowsWindow::getType()
	{
		return m_priv->type;
	}

	void* WindowsWindow::GetWindowHwnd() const
	{
		return m_priv->hWnd;
	}

	bool WindowsWindow::OnWindowClose()
	{
		auto& d = *m_priv;
		//d.RenderViewCallBack(RenderViewCallBack::closeWindow, d.Title);
		d.needClose = true;
		return true;
	}
}

