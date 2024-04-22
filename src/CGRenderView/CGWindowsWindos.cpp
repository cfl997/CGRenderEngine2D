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

#define use_opengl_4_6
//#define use_Window_Title //标题栏会导致 页面发生偏差


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
		uint32_t Width, Height;
		bool VSync;
		GLFWwindow* shareWindow;
		WindowType type;
		//event
		EventCallbackFn EventCallback;
		std::unique_ptr<CGRenderEventManager>eventManager;
		//opengl
		int glContextID = -1;

		//camera
		std::unique_ptr<CGCamera>camera = nullptr;
		glm::mat4 perspectiveMatrix = glm::mat4(1);

		//this
		WindowsWindow* super = nullptr;

		//layer
		std::unique_ptr<CGData::CGLayer>layer = nullptr;




		glm::mat4 getInverseScreenMatrix()
		{
			glm::mat4 persp = super->getPerspectiveMatrix();
			glm::mat4 view = camera->GetViewMatrix();
			return glm::inverse(persp * view);
		}

		std::tuple<float, float> getWorldPos(glm::vec2 pos)
		{
			//glm::mat4 aa = glm::mat4{ 1 };
			//glm::vec3 worldPos = screenToWorld({ pos.x,Height - pos.y,1. }, super->getViewMatrix(), super->getPerspectiveMatrix(), {0,0,Width,Height});
			auto worldPos = CGRender_GetWorldPos(pos, camera->GetViewMatrix(), super->getPerspectiveMatrix(), glm::vec4(0, 0, Width, Height));
			return std::make_tuple(worldPos.x, worldPos.y);//yes
			//{

			//	float z = 1.0f;


			//	glm::vec3 win(pos.x, Height - pos.y, z);
			//	glm::vec4 viewport(0.0f, 0.0f, (float)Width, (float)Height);
			//	glm::vec3 world = glm::unProject(win, view, persp, viewport);
			//	return { world.x,world.y };
			//}

			glm::mat4 inverseSM = getInverseScreenMatrix();
			//glm::vec2 devicePos = ScreenToNormalizedDeviceCoords(pos.x, pos.y, Width, Height);
			//glm::vec2 devicePos = pos;
			glm::vec2 devicePos = glm::vec2{ pos.x  , pos.y };
			glm::vec4 worldpos = inverseSM * glm::vec4(devicePos, 1, 1);
#ifdef _DEBUG
			{
				glm::mat4 persp = super->getPerspectiveMatrix();
				glm::vec4 revertpos = persp * camera->GetViewMatrix() * worldpos;
				float distance = glm::distance(glm::vec3(devicePos, 0), glm::vec3{ revertpos.x,revertpos.y,revertpos.z });
				if (distance > 0.001)
				{
					//__debugbreak();
					//assert(0);
				}
			}
#endif // DEBUG
			if (std::isnan(worldpos.x) || std::isnan(worldpos.y))
			{
				__debugbreak();
			}
			return std::make_tuple(worldpos.x, worldpos.y);
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

	int WindowsWindow::getGLContextID()
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
		//return std::move(d.camera->GetViewMatrix());
		return d.camera->GetViewMatrix();
	}

	const glm::mat4& WindowsWindow::getPerspectiveMatrix()noexcept
	{
		auto& d = *m_priv;
		if (d.Height <= 0)
		{
			__debugbreak();
			return std::move(glm::mat4{ 1 });
		}
		//右手坐标系，z越大，越近
		d.perspectiveMatrix = glm::perspective(glm::radians(d.camera->Zoom), static_cast<float>(d.Width) / static_cast<float>(d.Height), 0.1f, 1000.f);
		return d.perspectiveMatrix;
	}

	struct UniformBufferData
	{
		//matrix4f matrix;
		//float A;
		glm::mat4 matrix;
	};

	//#define useRenderDoc//cfl
	void WindowsWindow::renderTest()
	{


		auto& d = *m_priv;
		int renderTarget = CGRender_GetRenderTarget(d.glContextID);
		if (1)
		{
			CGRender_SetViewport(d.glContextID, 0, 0, d.Width, d.Height);
			CGRender_SetScissor(d.glContextID, 0, 0, d.Width, d.Height);

			CGRender_ClearTexture(d.glContextID, renderTarget, 0x00ffff00);
			CGRender_ClearTexture(d.glContextID, renderTarget, 0x00000000);
			glm::mat4 transform = glm::mat4(1.0f);
			glm::mat4 view = getViewMatrix();
			glm::mat4 Perspective = getPerspectiveMatrix();

			glm::mat4 aa = Perspective * view * transform;//test
			//aa = glm::mat4{ 1 };

			//d.layer->Render(d.glContextID, aa);
			//return;
			//glClear(GL_DEPTH_BUFFER_BIT);
			//auto error=glGetError();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glEnable(GL_DEPTH_TEST);
			//glDepthFunc(GL_LESS);
			if (1)
			{
				const static std::wstring tex16file = L"E:/A/work/Render2D/src/gTestCGRenderView/testFiles/010_L01S.img";
				CGData::CGITtemTex16* tex16 = new CGData::CGITtemTex16(tex16file);
				d.layer->addItem(tex16);
				d.layer->Render(d.glContextID, aa);
				d.layer->removeItem(tex16->GUID());
				return;
			}

			{
				//CGRender_ClearTexture(d.glContextID, renderTarget, 0xFFFFFFFF);
				CGData::CGItemRectangle* itemRectangle = new CGData::CGItemRectangle();
				//itemRectangle.setGLContext(d.glContextID);
				//itemRectangle.Render(d.glContextID, aa);
				d.layer->addItem(itemRectangle);
				d.layer->Render(d.glContextID, aa);
				d.layer->removeItem(itemRectangle->GUID());
			}
			return;
			if (1)
			{
				CGData::CGItemRectangle* itemRectangle = new CGData::CGItemRectangle();
				itemRectangle->setWidth(30);
				itemRectangle->setHeight(50);
				itemRectangle->setColor(0x0ffffff00);
				glm::mat4 transform = glm::mat4{ 1 };
				transform = glm::translate(transform, glm::vec3{ 100,100,0 });
				glm::mat4 bb = aa * transform;
				d.layer->addItem(itemRectangle);
				//d.layer->Render(d.glContextID, bb);
				d.layer->removeItem(itemRectangle->GUID());
			}

			{
				CGData::CGItemRectangle* itemRectangle = new CGData::CGItemRectangle();
				itemRectangle->setColor(0x0ff0000ff);
				glm::mat4 transform = glm::mat4{ 1 };
				transform = glm::translate(transform, glm::vec3{ 100,-100,0 });
				glm::mat4 bb = aa * transform;
				d.layer->addItem(itemRectangle);
				//d.layer->Render(d.glContextID, bb);
				d.layer->removeItem(itemRectangle->GUID());
			}
			return;
		}
		int vsptmvp = CGRender_CreateShader(d.glContextID, ShaderCodeName::VS_POS_TEX_MVPMatrix);
		CGRender_SetShader(d.glContextID, vsptmvp, ShaderType::VERTEX);



		UniformBufferData unidata;
		glm::mat4 transform = glm::mat4(1.0f);
		//transform = glm::translate(transform, glm::vec3(-0.5f, 0.5f, 0.0f));
		//transform = glm::scale(transform, glm::vec3(.5f, .5f, 1.f));
		//transform = glm::rotate(transform, (float)1.0, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 view = getViewMatrix();
		glm::mat4 Perspective = getPerspectiveMatrix();
		unidata.matrix = view * transform;

		glm::mat4 aa = Perspective * view * transform;//test
		//glm::mat4 aa =  view * transform;
		unidata.matrix = aa;

		int uniformBufferid = CGRender_CreateBuffer(d.glContextID, sizeof(UniformBufferData), 1, &unidata, GLBufferType::uniformBuffer);

		//int psshader = CGRender_CreateShader(d.glContextID, ShaderCodeName::FSTestUniform);
		int psshader = CGRender_CreateShader(d.glContextID, ShaderCodeName::FS_Tex_Rotate90);
		CGRender_SetShader(d.glContextID, psshader, ShaderType::FRAGMENT);
		CGRender_SetUniformBuffer(d.glContextID, uniformBufferid, 0, ShaderType::VERTEX);

#ifndef useRenderDoc
		//结果作为纹理
		//CGRender_Render(1, 1, 2, GLPrimitiveTypes::TRIANGLELIST, 0, 0, 0);


		//CGRender_ClearTexture(d.glContextID, renderTarget, 0xFFFFFFFF);

		//int rotateTexture = CGRender_GetRenderTarget(d.glContextID);
		//CGRender_SetShaderTexture(d.glContextID, rotateTexture, 0);

		//int renderTarget= CGRender_CreateTextureFromData(d.glContextID, 0, 1280, 720,GLTexture_Normal2DTex);
		//CGRender_SetRenderTarget(d.glContextID, renderTarget);

		int psGray = CGRender_CreateShader(d.glContextID, ShaderCodeName::FS_Tex_Gray);
		CGRender_SetShader(d.glContextID, psGray, ShaderType::FRAGMENT);

		CGRender_Render(1, 1, 2, GLPrimitiveTypes::TRIANGLELIST, 0, 0, 0);
#else
		CGRender_ClearTexture(d.glContextID, renderTarget, 0xFFFFFFFF);
		CGRender_Render(1, 19, 20, GLPrimitiveTypes::TRIANGLELIST, 0, 0, 0);
#endif // !useRenderDoc
	}


	void WindowsWindow::Init(const WindowProps& props)
	{
		auto& d = *m_priv;
		d.parent = props.parentWindow;
		d.Title = props.Title;
		d.Width = props.Width;
		d.Height = props.Height;
		d.shareWindow = (GLFWwindow*)props.share_Window;
		d.type = props.type;

		if (!s_GLFWInitialized)
		{

			int success = glfwInit();
			assert(success);
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;

			{
				TCHAR szFilePath[MAX_PATH + 1] = { 0 };
				GetModuleFileName(NULL, szFilePath, MAX_PATH);
				(_tcsrchr(szFilePath, _T('\\')))[1] = 0; // 删除文件名，只获得路径字串
				_tcscat(szFilePath, L"plugin");

				bool isInit = CGRender_Init(szFilePath);//是否包含glew32.dll
				assert(isInit);

			}
			//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#ifdef use_Window_Title
			// 设置窗口属性，隐藏标题栏
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
#endif // use_Window_Title

#ifdef use_opengl_4_6
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // use_opengl_4_6

		}

		{
			//event
			d.eventManager = std::make_unique<CGRenderEventManager>();
			SetEventCallback(std::bind(&CGRenderEventManager::OnEvent, d.eventManager.get(), std::placeholders::_1));
			//CGRenderEvent* defaultRenderEvent = new CGRenderEvent(CGRenderEventType::RenderEvent_Default);
			auto defaultRenderEvent = CGRenderEvnetFactory::instance()->createRenderEvent(RenderEvent_Default, this);
			addCGRenderEvent(defaultRenderEvent);
			//defaultRenderEvent->setWindow(this);
		}

		std::wstring_convert<std::codecvt_utf8<wchar_t>> m_cv;
		//这个会导致  发生偏移 c++ 
#ifdef use_Window_Title
		m_Window = glfwCreateWindow(d.parent, (int)props.Width, (int)props.Height, m_cv.to_bytes(d.Title).c_str(), nullptr, d.shareWindow);
#else
		m_Window = glfwCreateWindow(d.parent, (int)props.Width, (int)props.Height + 25, m_cv.to_bytes(d.Title).c_str(), nullptr, d.shareWindow);
#endif // use_Window_Title


		glfwMakeContextCurrent(m_Window);

		{
			//d.glContext = new CGRenderGL::GLContext();
			// 获取窗口句柄
			d.hWnd = glfwGetWin32Window(m_Window);
			HGLRC hGLRC = glfwGetWGLContext(m_Window);
			//d.glContext->setWindowData(hwnd, hGLRC);
			d.glContextID = CGRender_CreateContext(d.hWnd, hGLRC, d.Width, d.Height);
		}

		{
			d.camera = std::make_unique<CGCamera>();
			getPerspectiveMatrix();
		}
		{
			d.layer = std::make_unique<CGData::CGLayer>();
		}
		//cfl-test
		if (1)
		{
			auto defaultRenderEvent = CGRenderEvnetFactory::instance()->createRenderEvent(RenderEvent_Rectangle, this);
			addCGRenderEvent(defaultRenderEvent);
		}


		//int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		//GLCORE_ASSERT(status, "Failed to initialize Glad!");

		//LOG_INFO("OpenGL Info:");
		//LOG_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		//LOG_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		//LOG_INFO("  Version: {0}", glGetString(GL_VERSION));

		//std::cout << "  Vendor: " << glGetString(GL_VENDOR) << std::endl;
		//std::cout << "  Renderer: " << glGetString(GL_RENDERER) << std::endl;
		//std::cout << "  Version: " << glGetString(GL_VERSION) << std::endl;

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
				data.Width = width;
				data.Height = height;

				//WindowResizeEvent event(width, height);
				//data.EventCallback(event);



				CGRender_SetViewport(data.glContextID, 0, 0, data.Width, data.Height);
				CGRender_ResizeWindow(data.glContextID, data.Width, data.Height);
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

	void WindowsWindow::Render()
	{
		auto& d = *m_priv;
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
		return m_priv->Width;
	}

	inline uint32_t WindowsWindow::GetHeight() const
	{
		return m_priv->Height;
	}

	inline void WindowsWindow::SetEventCallback(const EventCallbackFn& callback)
	{
		m_priv->EventCallback = callback;
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

	void WindowsWindow::OnWindowClose()
	{

	}

	}
