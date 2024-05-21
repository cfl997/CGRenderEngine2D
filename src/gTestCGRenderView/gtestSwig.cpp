#include "gtestInclude.h"



class gtestSwig :public testing::Test
{
public:
	gtestSwig()
	{

	}
	~gtestSwig()
	{

	}

	// Sets up the test fixture.
	virtual void SetUp()
	{

	}

	// Tears down the test fixture.
	virtual void TearDown()
	{

	}

private:

};

typedef int(*addaa)(int a, int b);
typedef void* (*newRenderView)(void* a, void* b, void* c);
typedef void (*deleteRenderView)(void* jarg1);


TEST_F(gtestSwig, gtestLoadLibrary)
{
	auto binstr = CGPath_GetPath(CGPathType::CG_PATH_BIN);

	std::wstring binwstr{ binstr };
	binwstr += L"/CGRenderViewD.dll";

	HANDLE dll = LoadLibrary(binwstr.c_str());
	EXPECT_NE(dll, nullptr);


	addaa add = (addaa)GetProcAddress((HMODULE)dll, "CSharp_testswig_add");
	EXPECT_NE(add, nullptr);

	newRenderView newrenderView = (newRenderView)GetProcAddress((HMODULE)dll, "CSharp_new_CGRenderView");
	EXPECT_NE(newrenderView, nullptr);

	uint32_t width1 = 800;
	uint32_t height1 = 600;

	// µ÷ÓÃº¯Êý
	auto renderview = newrenderView(&width1, &height1, nullptr);
	EXPECT_NE(renderview, nullptr);

	deleteRenderView deleterenderView = (deleteRenderView)GetProcAddress((HMODULE)dll, "CSharp_delete_CGRenderView");
	EXPECT_NE(newrenderView, nullptr);

	deleterenderView(renderview);


	int b = add(100, 100);

	EXPECT_EQ(b, 200);

}