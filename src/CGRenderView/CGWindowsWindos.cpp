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

#include "CGCuda/CGCuda.h"

//正交模式
#define USE_ORTHO

//事件触发render - 集成使用
//#define USE_EVNET_RENDER

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
		bool resizeWindow = false;//resize状态
		WindowType type;
		CGRenderMode renderMode;
		int RollingDirection = (int)CGData::ImageRollingDrt::ImageRollingDrt_left2right;//方向
		//event
		EventCallbackFn EventCallback = nullptr;
		RenderViewCallBackFn RenderViewCallBack = nullptr;
		bool needClose = false;
		std::unique_ptr<CGRenderEventManager>eventManager;
		CGRenderEventDefault* eventDefault = nullptr;
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
			glm::mat4 persp = getPerspectiveMatrix();
			glm::mat4 view = camera->GetViewMatrix();
			return glm::inverse(persp * view);
		}

		std::tuple<float, float> getWorldPos(glm::vec2 pos)
		{
			auto worldPos = CGRender_GetWorldPos(pos, camera->GetViewMatrix(), getPerspectiveMatrix(), glm::vec4(0, 0, windowWidth, windowHeight));
			float scale = camera->ScaleCoefficient();
			worldPos /= scale;
			return std::make_tuple(worldPos.x, worldPos.y);//yes
		}

		/*
* matrix view and perspective
*/
		const glm::mat4 getViewMatrix()const;
		const glm::mat4& getPerspectiveMatrix()noexcept;

		void ResizeWindow(unsigned int width, unsigned int height);

		void ImageScale(float imageWidth, float imageHeight);
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
			CGRender_ReleaseContext(m_priv->glContextID);
			if (m_priv->eventManager)
			{
				m_priv->eventManager.reset();
				m_priv->eventManager = nullptr;

			}
			m_priv->super = nullptr;

			delete m_priv;
			m_priv = nullptr;
		}
		CUDA_ReleaseMemory();
		Shutdown();
	}

	bool WindowsWindow::addCGRenderEvent(CGRenderEventType EventType)
	{
		auto& d = *m_priv;
		//auto event = d.eventManager->getCGRenderEvent(EventType);
		//if (event)
		//	return false;
		CGRenderEvent* renderEvent = CGRenderEvnetFactory::instance()->createRenderEvent(EventType, this);
		if (!renderEvent)
			return false;
		d.eventManager->addCGRenderEvent(renderEvent);
		if (EventType == CGRenderEventType::RenderEvent_Default)
		{
			d.eventDefault = dynamic_cast<CGRenderEventDefault*>(renderEvent);
		}
		return true;
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

	void WindowsWindow::addImage(const std::wstring& path)
	{
		auto& d = *m_priv;

		d.layer->removeItem(CGData::CGItemType::CGItemImage);

		CGData::CGItemImage* image = new CGData::CGItemImage(ContextID(), path);
		d.layer->addItem(image);

#ifdef USE_ORTHO
		d.ImageScale(image->worldWidth(), image->worldHeight());
#else
		d.imageWidth = image->worldWidth();
		d.imageHeight = image->worldHeight();
		d.imageChange = true;
#endif // USE_ORTHO
		d.eventDefault->setDefultTexture(true);

#ifdef USE_EVNET_RENDER
		Render();
#endif // USE_EVNET_RENDER
	}

	void WindowsWindow::addImage(const std::string& path)
	{
		addImage(utf82wstr(path));
	}

	void WindowsWindow::addImage(void* data, int width, int height, bool dual, GLTextureType textureType)
	{
		auto& d = *m_priv;
		d.layer->removeItem(CGData::CGItemType::CGItemImage);

		CGData::CGItemImage* image = new CGData::CGItemImage(ContextID(), data, width, height, textureType);
		//image->updateData(data, width, height);
		d.layer->addItem(image);


#ifdef USE_ORTHO
		d.ImageScale(image->worldWidth(), image->worldHeight());
#else
		d.imageWidth = image->worldWidth();
		d.imageHeight = image->worldHeight();
		d.imageChange = true;
#endif // USE_ORTHO
		d.eventDefault->setDefultTexture(true);
#ifdef USE_EVNET_RENDER
		Render();
#endif // USE_EVNET_RENDER
	}

	void WindowsWindow::setRollingHeight(float height)
	{
		auto& d = *m_priv;

		float imageHeight = height;
		float scale = static_cast<float> (d.windowHeight) / static_cast<float> (imageHeight / g_globalProportion);

		if (scale > g_Camera_SCALE[CAMERA_SCALE_NUMS - 1])
			scale = 1.0;
		d.camera->ScaleCoefficient(scale);
		d.camera->ImageMinScale(scale);
		d.camera->resetPosition(d.windowWidth, d.windowHeight);

		float imageWidth = d.windowWidth / scale * g_globalProportion;

		std::swap(imageWidth, imageHeight);//动图模式宽高应该反过来

		auto items = d.layer->getItem(CGData::CGItemType::CGItemImage);
		CGData::CGItemImage* image = nullptr;
		if (items.empty())
		{
			image = new CGData::CGItemImage(ContextID(), 0, imageWidth, imageHeight, GLTextureType::GLTexture_Raw16);
			image->setRollingDirection((CGData::ImageRollingDrt)d.RollingDirection);
			d.layer->addItem(image);
			return;
		}
		image = dynamic_cast<CGData::CGItemImage*>(items.at(0));
		if (!image)return;
		image->Resize(imageWidth, imageHeight);
		image->setRollingDirection((CGData::ImageRollingDrt)d.RollingDirection);
#ifdef USE_EVNET_RENDER
		Render();
#endif // USE_EVNET_RENDER
	}

	void WindowsWindow::RollingDirection(int direction)
	{
		auto& d = *m_priv;
		d.RollingDirection = direction;
	}

	void WindowsWindow::RenderMode(CGRenderMode renderMode)
	{
		auto& d = *m_priv;
		d.renderMode = renderMode;
	}

	CGRenderMode WindowsWindow::RenderMode()
	{
		auto& d = *m_priv;
		return d.renderMode;
	}



	void WindowsWindow::setWindowWidthLevel(int WindowWidth, int WindowLevel)
	{
		auto& d = *m_priv;
		auto effector = getCurLayer()->Effector();
		if (!effector)
			return;
		effector->setWindowWidthLevel(WindowWidth, WindowLevel);
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
		getCamera()->ImageMinScale(pSclae);


		const auto pLayer = parent->getCurLayer();
		const auto rects = pLayer->getItem(CGData::CGItemType::CGItemRectangle);
		if (rects.empty())
			return;
		//const auto prect = rects.at(0);
		//CGData::CGBoundingShape shape(prect);
		//CGData::BoundingShape2D pShape2d;
		//shape.Bounding2D(&pShape2d);

		//auto Layer = getCurLayer();
		//auto Image = Layer->getItem(CGData::CGItemType::CGItemImage).at(0);
		//auto modelMatrix = Image->getModelMatrix();
		//auto tanslate = glm::translate(modelMatrix, glm::vec3(-pShape2d.center, 0));
		//Image->setModelMatrix(tanslate);

		//getCamera()->Position = glm::vec3{ pShape2d.center,getCamera()->Position.z };
	}

	int width = 800;
	int height = 600;
	int bufferSize = width * height * 4;
	char* buffer = new char[bufferSize];

	bool firstbuffer = true;

	void WindowsWindow::renderTest()
	{
		auto& d = *m_priv;

		int renderTarget = CGRender_GetRenderTarget(d.glContextID);
		if (1)
		{
			CGRender_SetViewport(d.glContextID, 0, 0, d.windowWidth, d.windowHeight);
			CGRender_SetScissor(d.glContextID, 0, 0, d.windowWidth, d.windowHeight);

			//CGRender_ClearTexture(d.glContextID, renderTarget, 0x00ffff00);
			CGRender_ClearTexture(d.glContextID, renderTarget, 0xFFFFFFFF);

			glm::mat4 transform = glm::mat4(1.0f);
			glm::mat4 view = d.getViewMatrix();
			glm::mat4 Perspective = d.getPerspectiveMatrix();


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
			if (d.eventDefault == nullptr)
				return;
			d.eventDefault->RenderTexture();
			if (1)
			{
				CGRender_SaveTextue(d.glContextID, renderTarget, L"D:/render2D.png");
			}

		}
		else
		{

			//CGRender_SaveTextue(contextid, hsrcTexture, L"D:/render2D.png");

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
			TCHAR* szfile = CGPath_GetPath(CGPathType::CG_PATH_PLUGIN);
			CGRender_LOG_INIT(szfile);

			CGRender_TIME_START;
			int success = glfwInit();
			assert(success);
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;

			CGRender_TIME_END("glfwInit");


			//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifndef use_Window_Title
			// 设置窗口属性，隐藏标题栏
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);//
#else
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
#endif // use_Window_Title

#ifdef use_opengl_4_6
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // use_opengl_4_6

		}//s_GLFWInitialized
		CGRender_TIME_START;
		{
			//event
			d.eventManager = std::make_unique<CGRenderEventManager>();
			SetEventCallback(std::bind(&CGRenderEventManager::OnEvent, d.eventManager.get(), std::placeholders::_1));
			//auto defaultRenderEvent = CGRenderEvnetFactory::instance()->createRenderEvent(RenderEvent_Default, this);
			//addCGRenderEvent(defaultRenderEvent);

			//d.eventDefault = dynamic_cast<CGRenderEventDefault*>(defaultRenderEvent);
			//assert(d.eventDefault);
		}
		CGRender_TIME_END("CGRenderEventManager");
		std::string title = wstr2utf8(d.Title);
		//这个会导致  发生偏移 c++ 
		int windowheigt = props.windowHeight;
		int windowWidth = props.windowWidth;

#ifndef use_Window_Title
		//m_Window = glfwCreateWindow(d.parent, (int)props.windowWidth, (int)props.windowHeight, title.c_str(), nullptr, d.shareWindow);
#else
		if (!d.parent)
		{
			windowheigt += 25;
		}
#endif // use_Window_Title
		CGRender_TIME_START;

		m_Window = glfwCreateWindow(d.parent, windowWidth, windowheigt, title.c_str(), nullptr, d.shareWindow);
		{
			////pianyi
			//int x, y = -1;
			//glfwGetWindowPos(m_Window, &x, &y);
			//glfwSetWindowPos(m_Window, x, y - 25);
		}
		glfwMakeContextCurrent(m_Window);
		CGRender_TIME_END("glfwCreateWindow");


		{
#ifdef DEBUG
			std::cout << "CGRender_Init: " << szfile << std::endl;
#endif // DEBUG

			TCHAR* szfile = CGPath_GetPath(CGPathType::CG_PATH_PLUGIN);
			bool isInit = CGRender_Init(szfile);//是否包含glew32.dll
			assert(isInit);
		}
		CGRender_TIME_START;
		{
			//d.glContext = new CGRenderGL::GLContext();
			// 获取窗口句柄
			d.hWnd = glfwGetWin32Window(m_Window);
			HGLRC hGLRC = glfwGetWGLContext(m_Window);
			//d.glContext->setWindowData(hwnd, hGLRC);
			d.glContextID = CGRender_CreateContext(d.hWnd, hGLRC, d.windowWidth, d.windowHeight);
		}
		CGRender_TIME_END("CGRender_CreateContext");
		CGRender_TIME_START;
		{
			//cfl-20240523  一个目标
			int renderTarget = CGRender_CreateTextureFromData(d.glContextID, 0, width, height, GLTexture_Normal2DTex);
			CGRender_SetRenderTarget(d.glContextID, renderTarget);
		}
		CGRender_TIME_END("CGRender_CreateTextureFromData");
		CGRender_SetViewport(d.glContextID, 0, 0, d.windowWidth, d.windowHeight);
		CGRender_SetScissor(d.glContextID, 0, 0, d.windowWidth, d.windowHeight);
		CGRender_TIME_START;
		{
			d.camera = std::make_unique<CGCamera>();
#ifdef USE_ORTHO
			d.camera->resetPosition(d.windowWidth, d.windowHeight);
#endif // USE_ORTHO
		}
		CGRender_TIME_END("CGCamera");

		{
			CGRender_TIME_START;
			d.layer = std::make_unique<CGData::CGLayer>(ContextID());
			CGRender_TIME_END("CGLayer");
		}
		//cfl-test
		CGRender_LOG_CLOSE;

		ResizeWindow(d.windowWidth, d.windowHeight);//20240618

		glfwSetWindowUserPointer(m_Window, m_priv);

		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				if (width <= 0 && height <= 0 && data.resizeWindow)
				{
					return;
				}
				data.ResizeWindow(width, height);
#ifdef USE_EVNET_RENDER
				data.super->Render();
#endif // USE_EVNET_RENDER
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
#ifdef USE_EVNET_RENDER
				data.super->Render();
#endif // USE_EVNET_RENDER
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
#ifdef USE_EVNET_RENDER
				data.super->Render();
#endif // USE_EVNET_RENDER
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
#ifdef USE_EVNET_RENDER
				data.super->Render();
#endif // USE_EVNET_RENDER
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				double x, y = 0;
				glfwGetCursorPos(window, &x, &y);

				auto [worldx, worldy] = data.getWorldPos({ x,y });

				MouseScrolledEvent event((float)xOffset, (float)yOffset, (float)worldx, (float)worldy);
				data.EventCallback(event);
#ifdef USE_EVNET_RENDER
				data.super->Render();
#endif // USE_EVNET_RENDER
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				auto [worldx, worldy] = data.getWorldPos({ x,y });

				//auto worldx = x; auto worldy = y;

				MouseMovedEvent event((float)worldx, (float)worldy);
				data.EventCallback(event);
#ifdef USE_EVNET_RENDER
				data.super->Render();
#endif // USE_EVNET_RENDER
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

	void WindowsWindow::ResizeWindow(unsigned int width, unsigned int height)
	{
		auto& d = *m_priv;
		d.ResizeWindow(width, height);
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

	const WindowType WindowsWindow::getWindowType()
	{
		return m_priv->type;
	}

	void WindowsWindow::setWindowType(WindowType type)
	{
		m_priv->type = type;
	}

	const glm::mat4& WindowsWindow::getVPMatrix()
	{
		auto& d = *m_priv;
		glm::mat4 view = d.getViewMatrix();
		glm::mat4 Perspective = d.getPerspectiveMatrix();

		return Perspective * view;
	}

	const glm::mat4& WindowsWindow::getPerspectiveMatrix()
	{
		// TODO: 在此处插入 return 语句
		auto& d = *m_priv;
		return d.getPerspectiveMatrix();
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
	const glm::mat4 WindowsWindow::WindowData::getViewMatrix() const
	{
		return camera->GetViewMatrix();
	}
	const glm::mat4& WindowsWindow::WindowData::getPerspectiveMatrix() noexcept
	{
		if (windowHeight <= 0)
		{
			__debugbreak();
			return std::move(glm::mat4{ 1 });
		}
#ifdef USE_ORTHO
		perspectiveMatrix = glm::ortho(0., static_cast<double>(windowWidth), 0., static_cast<double>(windowHeight), 0.1, 1000.);
#else
		double defaultZoom = camera->Zoom;
		if (imageWidth > 0 && imageHeight > 0 && imageChange)
		{
			imageChange = false;
			float imageProportion = (float)imageWidth / (float)imageHeight;
			float windowProportion = (float)windowWidth / (float)windowHeight;

			double realHeight = imageHeight;
			if (imageProportion > 1)
			{
				realHeight = imageWidth / windowProportion;
			}

			double opposite = realHeight;
			//double adjacent = 2460;//this is correct
			//double adjacent = 10000;// 
			double adjacent = 1000 - 0.1;// 

			defaultZoom = atan(opposite / adjacent) * 180 / glm::pi<double>();
			//d.camera->Zoom = defaultZoom * 2.;
			camera->Zoom = defaultZoom;
		}
		//右手坐标系，z越大，越近
		perspectiveMatrix = glm::perspective(glm::radians(defaultZoom), static_cast<double>(windowWidth) / static_cast<double>(windowHeight), 0.1, 1000.);
#endif // USE_ORTHO

		return perspectiveMatrix;
	}
	void WindowsWindow::WindowData::ResizeWindow(unsigned int width, unsigned int height)
	{
		windowWidth = width;
		windowHeight = height;

		{
			resizeWindow = true;
			glfwSetWindowSize(super->m_Window, width, height);
			resizeWindow = false;
		}


		camera->resetPosition(windowWidth, windowHeight);

		CGRender_SetViewport(glContextID, 0, 0, windowWidth, windowHeight);
		CGRender_SetScissor(glContextID, 0, 0, windowWidth, windowHeight);
		CGRender_ResizeWindow(glContextID, windowWidth, windowHeight);


		auto images = layer->getItem(CGData::CGItemType::CGItemImage);
		if (images.empty())
			return;

		auto image = dynamic_cast<CGData::CGItemImage*>(images.at(0));
		if (image == nullptr)
			return;

		ImageScale(image->worldWidth(), image->worldHeight());

	}
	void WindowsWindow::WindowData::ImageScale(float imageWidth, float imageHeight)
	{
		float widthScale = static_cast<float> (windowWidth) / static_cast<float> (imageWidth);
		float heightScale = static_cast<float> (windowHeight) / static_cast<float> (imageHeight);
		float scale = widthScale < heightScale ? widthScale : heightScale;
		if (scale > g_Camera_SCALE[CAMERA_SCALE_NUMS - 1])
			scale = 1.0;
		camera->ScaleCoefficient(scale);
		camera->ImageMinScale(scale);
		camera->resetPosition(windowWidth, windowHeight);
	}
}

