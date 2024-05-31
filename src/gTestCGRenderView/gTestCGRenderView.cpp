

#include "gtestInclude.h"

#include "gTestEnviroment.h"

extern testing::Environment* const g_env;

class MyTest :public testing::Test
{
public:
	MyTest()
	{
		env = dynamic_cast<gTestEnviroment*>(g_env);

	}
	~MyTest()
	{

	}

	// Sets up the test fixture.
	virtual void SetUp()override
	{



		int renderTarget = CGRender_CreateTextureFromData(env->contextID, 0, WindowWidth, WindowHeight, GLTexture_Normal2DTex);
		CGRender_SetRenderTarget(env->contextID, renderTarget);
	}

	// Tears down the test fixture.
	virtual void TearDown()override
	{
		int renderTarget = CGRender_GetRenderTarget(env->contextID);
		CGRender_DeleteTexture(env->contextID, renderTarget);



	}

protected:
	gTestEnviroment* env = nullptr;
};



TEST_F(MyTest, worldPos)
{
#if 0


	auto worldPos = CGRender_GetWorldPos({ WindowWidth / 2,WindowHeight / 2 }, env->m_windowswin->getViewMatrix(), env->m_windowswin->getPerspectiveMatrix(), { 0,0,WindowWidth,WindowHeight });
	float distance = glm::distance(glm::vec2{ worldPos.x,worldPos.y }, { 0,0 });
	EXPECT_LE(distance, 0.1);

	if (0)
	{
		auto worldPos = CGRender_GetWorldPos({ WindowWidth,WindowHeight }, env->m_windowswin->getViewMatrix(), env->m_windowswin->getPerspectiveMatrix(), { 0,0,WindowWidth,WindowHeight });
		float distance = glm::distance(glm::vec2{ worldPos.x,worldPos.y }, { float(WindowWidth / 2) ,float(WindowHeight / 2) });
		std::cout << "distance :" << distance << std::endl;
		EXPECT_LE(distance, 0.1);
	}
#endif // 0
}


TEST_F(MyTest, RenderEvent)
{
	env->m_windowswin->addCGRenderEvent(CGRenderEventType::RenderEvent_Rectangle);

	//m_renderView->Render();

	env->m_windowswin->removeCGRenderEvent(CGRenderEventType::RenderEvent_Rectangle);


}
const static std::wstring tex16file = L"E:/A/work/Render2D/src/gTestCGRenderView/testFiles/010_L01S.img";
TEST_F(MyTest, CGItemTex16)
{
	auto layer = env->m_windowswin->getCurLayer();


	CGData::CGITtemTex16* itemTex = new CGData::CGITtemTex16(env->contextID, tex16file);
	layer->addItem(itemTex);

	//m_renderView->Render();
	env->m_windowswin->Render();


	layer->removeItem(itemTex->GUID());
	//	int target = CGRender_GetRenderTarget(contextID);
	//	//CGRender_SaveTextue(contextID, target, RESOURCE_FILE_PATH L"Mytest-saveImageTarget.png");
	//	CGRender_SaveTextue(contextID, target, PICTUREFILENAME);
}


TEST_F(MyTest, saveImage)
{

	//int target = CGRender_GetRenderTarget(contextID);
	//CGRender_SaveTextue(contextID, target, RESOURCE_FILE_PATH L"Mytest-saveImageTarget.png");

}