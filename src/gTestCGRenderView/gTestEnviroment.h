#include "gtestInclude.h"


static const int WindowWidth = 970;
static const int WindowHeight = 720;

class gTestEnviroment : public testing::Environment
{
public:
	gTestEnviroment()
	{
		m_renderView = new CGRender::CGRenderView(WindowWidth, WindowHeight, nullptr);
		//window = m_renderView->getWindowByType(CGRender::WindowType::Window_Main).get();
		window = m_renderView->getWindowByTitle(CGRender::g_windowMainStr).get();
		contextID = window->ContextID();
		m_windowswin = dynamic_cast<CGRender::WindowsWindow*>(window);
		EXPECT_NE(m_windowswin, nullptr);
	}
	~gTestEnviroment()
	{
		delete m_renderView;
		m_renderView = nullptr;
	}

	virtual void SetUp()
	{

	}

	virtual void TearDown()
	{

	}
public:
	CGRender::CGRenderView* m_renderView = nullptr;
	int contextID = -1;
	CGRender::Window* window = nullptr;
	CGRender::WindowsWindow* m_windowswin = nullptr;
private:

};
testing::Environment* const g_env = testing::AddGlobalTestEnvironment(new gTestEnviroment);

