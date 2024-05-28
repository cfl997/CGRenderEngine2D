#include <string>
#include <fstream>
#include <sstream>


#include <thread>
#include <random>
#include <assert.h>

#include "Windows.h"

#include <iostream>

#include <codecvt>


#include "CGRender.h"
#include "CGRenderView.h"
#include "CGWindowsWindos.h"
#include "CGglm.h"

#include "CGData.h"


struct ImageData
{
	unsigned short width;
	unsigned short height;
	char* data;
	bool isLoad;
	int bytes;
	ImageData() :isLoad(false),
		width(0),
		height(0),
		data(0),
		bytes(0)
	{

	}
	~ImageData()
	{
		if (data)
		{
			delete data;
			data = nullptr;
		}
	}

	ImageData(
		unsigned short w,
		unsigned short h,
		char* da,
		bool is,
		int by)
	{
		width = w;
		height = h;
		{
			data = new char[by * w * h];
			std::copy(da, da + by * w * h, data);
		}
		isLoad = is;
		bytes = bytes;
	}

	ImageData& operator=(const ImageData& other)
	{
		width = other.width;
		height = other.height;
		{
			data = new char[other.bytes * other.width * other.height];
			std::copy(other.data, other.data + other.bytes * other.width * other.height, data);
		}
		isLoad = other.isLoad;
		bytes = other.bytes;
	}
};
bool getImageData(const std::wstring& path, ImageData& imageData)
{
	FILE* fp = 0;

	_wfopen_s(&fp, path.c_str(), L"rb");
	if (!fp)
	{
		return false;
	}
	struct {
		unsigned short nO, width, height, nl, bits, energy, order, inverse, n[248];
	}header;
	size_t size = fread(&header, sizeof(header), 1, fp);
	if (header.width > 0 && header.height > 0)
	{
		header.bits = 16;
		header.energy = 0;
		header.order = 0;
		long max = (1 << header.bits) - 1;
		long bytes = (header.bits + 7) / 8;
		long bufsize = header.width * header.height * bytes;
		char* szbuf = new char[bufsize];
		//char* szbuf = new char[bufsize];
		if (1 == fread(szbuf, bufsize, 1, fp))
		{
			fclose(fp);
			imageData.width = header.width;
			imageData.height = header.height;
			imageData.bytes = bytes;
			imageData.isLoad = true;
			imageData.data = new char[bytes * header.width * header.height];
			std::copy(szbuf, szbuf + bytes * header.width * header.height, imageData.data);
			return true;
		}
	}
	fclose(fp);

	return false;
}
#if 0


float vertices[] = {
	//     ---- 位置 ----  - 纹理坐标 -
		 1.f,  1.f, 0.0f,    1.0f, 1.0f,   // 右上
		 1.f, -1.f, 0.0f,    1.0f, 0.0f,   // 右下
		-1.f, -1.f, 0.0f,    0.0f, 0.0f,   // 左下
		-1.f,  1.f, 0.0f,    0.0f, 1.0f    // 左上
};
#else

float vertices[] = {
	//     ---- 位置 ----  - 纹理坐标 -
		 10.f,  10.f, 0.0f,    1.0f, 1.0f,   // 右上
		 10.f, -10.f, 0.0f,    1.0f, 0.0f,   // 右下
		-10.f, -10.f, 0.0f,    0.0f, 0.0f,   // 左下
		-10.f,  10.f, 0.0f,    0.0f, 1.0f    // 左上
};
#endif 

static unsigned int indexes[] =
{
	0, 1, 3,
	1, 2, 3
};

struct UniformBufferData
{
	//matrix4f matrix;
	//float A;
	glm::mat4 matrix;
};

static const int WindowWidth = 800;
static const int WindowHeight = 600;

//static const int WindowWidth = 1920;
//static const int WindowHeight = 1000;
int main()
{


	CGData::CGItem Item;
#if 0
	std::unique_ptr<CGRender::Window> Window = std::unique_ptr<CGRender::Window>(CGRender::Window::Create({ L"opengl", 1280,720,nullptr }));

	std::unique_ptr<CGRender::Window> Window1 = std::unique_ptr<CGRender::Window>(CGRender::Window::Create({ L"opengl1", 1280,720,Window->GetNativeWindow() }));


	//增加这个事件去执行
	CGRender::CGRenderEvent* renderEvent = new CGRender::CGRenderEvent(CGRender::CGRenderEventType::RenderEvent_Default);
	Window->addCGRenderEvent(renderEvent);

	while (true)
	{
		Window->OnUpdate();
		Window1->OnUpdate();
	}
#endif // 0

	std::unique_ptr<CGRender::CGRenderView> renderView = std::make_unique<CGRender::CGRenderView>(WindowWidth, WindowHeight, nullptr);
	//renderView->createWindow({ L"opengl", 1280,720,nullptr });
	//auto window = renderView->getWindowByType(CGRender::WindowType::Window_Main);
	auto window1 = renderView->getWindowByTitle(CGRender::g_windowMainStr.c_str());
	//window->addCGRenderEvent(CGRenderEventType::BoxRenderEvent);
	auto window = (CGRender::Window*)window1;
	int contextID = window->ContextID();

	ImageData imageData;
	getImageData(L"D:/document/2024/March/20240327/010_L01S.img", imageData);

	//int texture = CGRender_CreateTextureFromData(contextID, imageData.data, imageData.width, imageData.height, GLTextureType::GLTexture_Raw16);

	int textureid = 0;
	int textureid1 = 0;

	int indexIndex;
	int vboIndex;

	int uniformBufferid = -1;

	int renderTarget = 0;
	{
		textureid1 = CGRender_CreateTextureFromFile(contextID, L"D:/document/2024/March/20240327/20240401095541.png", GLTexture_Normal2DTex);
		textureid = CGRender_CreateTextureFromData(contextID, imageData.data, imageData.width, imageData.height, GLTextureType::GLTexture_Raw16);

		{
			renderTarget = CGRender_CreateTextureFromData(contextID, 0, WindowWidth, WindowHeight, GLTexture_Normal2DTex);
			//renderTarget = CGRender_CreateTextureFromData(contextID, 0, imageData.width, imageData.height, GLTexture_Normal2DTex);//这个会影响位置????????

			{

				//CGRender_SetRenderTarget(contextID, renderTarget);
				//CGRender_SetViewport(contextID, 0, 0, WindowWidth, WindowHeight);

				//CGData::CGItemRectangle itemRectangle;
				//itemRectangle.setGLContext(contextID);
				//itemRectangle.Render();

			}
		}
		std::cout << "renderTarget:  " << renderTarget << std::endl;
		assert(renderTarget > 0);

		int vsptmvp3 = CGRender_CreateShader(contextID, ShaderCodeName::VS_POS_TEX_MVP3Matrix);
		int vsptmvp = CGRender_CreateShader(contextID, ShaderCodeName::VS_POS_TEX_MVPMatrix);
		//int psshader = CGRender_CreateShader(contextID, ShaderCodeName::FSTwoTex);
		int psshader = CGRender_CreateShader(contextID, ShaderCodeName::FS_Tex_Gray);

		CGRender_SetShader(contextID, vsptmvp3, ShaderType::VERTEX);
		CGRender_SetShader(contextID, vsptmvp, ShaderType::VERTEX);
		//CGRender_SetShader(contextID, psshader, ShaderType::FRAGMENT);

		{
			vboIndex = CGRender_CreateBuffer(contextID, sizeof(vertices), sizeof(vertices) / sizeof(float), (void*)vertices, GLBufferType::VertexBuffer, GLPrimitiveTypes::TRIANGLELIST);
			std::cout << "vboindex: " << vboIndex << std::endl;
			indexIndex = CGRender_CreateBuffer(contextID, sizeof(indexes), sizeof(indexes) / sizeof(unsigned int), (void*)indexes, GLBufferType::IndexBuffer, GLPrimitiveTypes::TRIANGLELIST);

			std::cout << "indexIndex: " << indexIndex << std::endl;

			/*
			* testUniform
			*/
			{
				UniformBufferData unidata;
				glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
				//transform = glm::translate(transform, glm::vec3(0.5f, 0.5f, 0.0f));
				//transform = glm::scale(transform, glm::vec3(.5f, .5f, 1.f));
				//transform = glm::rotate(transform, (float)1.0, glm::vec3(0.0f, 0.0f, 1.0f));
				unidata.matrix = transform;

				//unidata.matrix._11 = 1.;
				//unidata.matrix._12 = 0;
				//unidata.matrix._13 = 0;
				//unidata.matrix._14 = 0;
				//unidata.matrix._21 = 0;
				//unidata.matrix._22 = 1.;
				//unidata.matrix._23 = 0;
				//unidata.matrix._24 = 0;
				//unidata.matrix._31 = 0;
				//unidata.matrix._32 = 0;
				//unidata.matrix._33 = 1.;
				//unidata.matrix._34 = 0;
				//unidata.matrix._41 = 0;
				//unidata.matrix._42 = 0;
				//unidata.matrix._43 = 0;
				//unidata.matrix._44 = 1.;

				//unidata.A = 0.5;

				uniformBufferid = CGRender_CreateBuffer(contextID, sizeof(UniformBufferData), 1, &unidata, GLBufferType::uniformBuffer);
				//int vsptmvp = CGRender_CreateShader(contextID, ShaderCodeName::VS_POS_TEX_MVPMatrix);
				//int vsptmvp = CGRender_CreateShader(contextID, ShaderCodeName::VS_POS_COLOR_TEX);//test
				//int psshader = CGRender_CreateShader(contextID, ShaderCodeName::FSTestUniform);
				int psshader = CGRender_CreateShader(contextID, ShaderCodeName::FS_Tex_Rotate90);
				CGRender_SetShader(contextID, psshader, ShaderType::FRAGMENT);
				CGRender_SetUniformBuffer(contextID, uniformBufferid, 0, ShaderType::VERTEX);
				//CGRender_SetUniformBuffer(contextID, uniformBufferid, 0, ShaderType::FRAGMENT);
			}
		}
	}

	//CGRender_SetShaderTexture(contextID, textureid1, 1, ShaderType::FRAGMENT);
	CGRender_SetShaderTexture(contextID, textureid, 0, ShaderType::FRAGMENT);


	CGRender_SetRenderTarget(contextID, renderTarget);
	//CGRender_SetViewport(contextID, 0, 0, 1920, 1080);
	CGRender_SetViewport(contextID, 0, 0, WindowWidth, WindowHeight);
	CGRender_SetScissor(contextID, 0, 0, WindowWidth, WindowHeight);



	//std::thread renderThread{ [&] {
	//int nums = 0;
	//while (true)
	//{

	//	//CGRender_Render(contextID, vboIndex, indexIndex, GLPrimitiveTypes::TRIANGLELIST, 0, 0, 0);

	//}
	//} };
	//renderThread.detach();

	renderView->Render();
	while ((true))
	{
		renderView->Render();
	}

	return 0;
}



