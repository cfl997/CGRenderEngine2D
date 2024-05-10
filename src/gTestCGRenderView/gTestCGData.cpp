

#include "gtestInclude.h"
#include "gTestEnviroment.h"

using namespace CGData;
extern testing::Environment* const g_env;

class gTestCGData :public testing::Test
{
public:
	gTestCGData()
	{
		env = dynamic_cast<gTestEnviroment*>(g_env);

		int renderTarget = CGRender_CreateTextureFromData(env->contextID, 0, WindowWidth, WindowHeight, GLTexture_Normal2DTex);
		CGRender_SetRenderTarget(env->contextID, renderTarget);

	}
	~gTestCGData()
	{
		int target = CGRender_GetRenderTarget(env->contextID);
		CGRender_DeleteTexture(env->contextID, target);
	}

	virtual void SetUp()override
	{

	}

	virtual void TearDown()override
	{

	}


protected:
	gTestEnviroment* env = nullptr;
};

TEST_F(gTestCGData, Bounding)
{

	CGItemRectangle rectangle(env->contextID);
	rectangle.Width(30);
	rectangle.Height(40);

	rectangle.build(env->contextID);
	rectangle.Render(env->contextID, glm::mat4{ 1 });

	int redertarget = CGRender_GetRenderTarget(env->contextID);
	CGRender_SaveTextue(env->contextID, redertarget, L"d:/gtestCGData_Bounding.png");

	CGBoundingShape bShape(&rectangle);
	BoundingShape2D bshape2D;
	bShape.Bounding2D(&bshape2D);

	EXPECT_LE(bshape2D.radius, 25.);
	EXPECT_EQ(bshape2D.center.x, 15.);
	EXPECT_EQ(bshape2D.center.y, -20.);
}


TEST_F(gTestCGData, cgEffector)
{
	CGItemImage* image = new CGItemImage(env->contextID, L"D:/document/2024/picture/img.png");
	int srcTexture = image->TextureID();

	CGEffector effctor(env->contextID);
	effctor.addEffector(ShaderCodeName::FS_Tex_Rotate90);
	effctor.addEffector(ShaderCodeName::FS_Tex_Invert);
	effctor.addEffector(ShaderCodeName::FS_Tex_XReversal);
	int desTexture = -1;
	effctor.Render(env->contextID, srcTexture, &desTexture);

	int i = 0;
}


