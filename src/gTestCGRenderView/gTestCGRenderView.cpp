
#include "CGRender.h"
#include "CGData.h"
#include "CGRenderView.h"

#include "gtest/gtest.h"

static const int WindowWidth = 970;
static const int WindowHeight = 720;

class MyTest :public testing::Test
{
public:
	MyTest()
	{
		m_renderView = new CGRender::CGRenderView(WindowWidth, WindowHeight, nullptr);
		window = m_renderView->getWindowByType(CGRender::WindowType::Window_Main).get();
	}
	~MyTest()
	{

	}

	// Sets up the test fixture.
	virtual void SetUp()override
	{
		contextID = window->getGLContextID();

		m_windowswin = dynamic_cast<CGRender::WindowsWindow*>(window);
		EXPECT_NE(m_windowswin, nullptr);

		int renderTarget = CGRender_CreateTextureFromData(contextID, 0, WindowWidth, WindowHeight, GLTexture_Normal2DTex);
		CGRender_SetRenderTarget(contextID, renderTarget);
	}

	// Tears down the test fixture.
	virtual void TearDown()override
	{

		delete m_renderView;
		m_renderView = nullptr;
	}

	CGRender::CGRenderView* m_renderView = nullptr;
	int contextID = -1;
	CGRender::Window* window = nullptr;
	CGRender::WindowsWindow* m_windowswin = nullptr;
private:
};

TEST_F(MyTest, aa)
{


	//m_renderView->Render();//这个是主循环

	window->Render();
	int i = 0;

}

TEST_F(MyTest, worldPos)
{
	auto worldPos = CGRender_GetWorldPos({ WindowWidth / 2,WindowHeight / 2 }, m_windowswin->getViewMatrix(), m_windowswin->getPerspectiveMatrix(), { 0,0,WindowWidth,WindowHeight });
	float distance = glm::distance(glm::vec2{ worldPos.x,worldPos.y }, { 0,0 });
	EXPECT_LE(distance, 0.1);

	if(0)
	{
		auto worldPos = CGRender_GetWorldPos({ WindowWidth,WindowHeight }, m_windowswin->getViewMatrix(), m_windowswin->getPerspectiveMatrix(), { 0,0,WindowWidth,WindowHeight });
		float distance = glm::distance(glm::vec2{ worldPos.x,worldPos.y }, { float(WindowWidth / 2) ,float(WindowHeight / 2) });
		std::cout << "distance :" << distance << std::endl;
		EXPECT_LE(distance, 0.1);
	}
}


TEST_F(MyTest, RenderEvent)
{
	m_windowswin->addCGRenderEvent(CGRenderEventType::RenderEvent_Rectangle);

	//m_renderView->Render();

	m_windowswin->removeCGRenderEvent(CGRenderEventType::RenderEvent_Rectangle);


}