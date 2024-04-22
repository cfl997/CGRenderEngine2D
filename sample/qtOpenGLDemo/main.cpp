#include <QApplication>
#include "QtDisplayWidget.h"
#include "qwindow.h"

#include <string>
#include <fstream>
#include <sstream>

//#include "GLContext.h"


#include <thread>
#include <random>
#include "Windows.h"

#include <iostream>

#include <codecvt>


#include "CGRender.h"
#include "CGRenderView.h"
#include "CGglm.h"

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

//float vertices[] = {
//	//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
//		 1.f,  1.f, 0.0f,   1.0f, 0.0f, 0.0f,1.0f,   1.0f, 1.0f,   // 右上
//		 1.f, -1.f, 0.0f,   0.0f, 1.0f, 0.0f,1.0f,   1.0f, 0.0f,   // 右下
//		-1.f, -1.f, 0.0f,   0.0f, 0.0f, 1.0f,1.0f,   0.0f, 0.0f,   // 左下
//		-1.f,  1.f, 0.0f,   1.0f, 1.0f, 0.0f,1.0f,   0.0f, 1.0f    // 左上
//};

float vertices[] = {
	//     ---- 位置 ----  - 纹理坐标 -
		 1.f,  1.f, 0.0f,    1.0f, 1.0f,   // 右上
		 1.f, -1.f, 0.0f,    1.0f, 0.0f,   // 右下
		-1.f, -1.f, 0.0f,    0.0f, 0.0f,   // 左下
		-1.f,  1.f, 0.0f,    0.0f, 1.0f    // 左上
};

static unsigned int indexes[] =
{
	0, 1, 3,
	1, 2, 3
};
//#define useMutilView


static const int WindowWidth = 970;
static const int WindowHeight = 720;
struct UniformBufferData
{
	//matrix4f matrix;
	//float A;
	glm::mat4 matrix;
};

int main(int argc, char* argv[])
{

#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)) && defined(_WIN32)
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
	QCoreApplication::addLibraryPath(".");
	//QCoreApplication::setEventDispatcher(nullptr);

	QApplication* app = new QApplication(argc, argv);
	QTDisplayWidget w;
	w.resize(WindowWidth, WindowHeight);
	w.show();
	HWND parentHwnd = reinterpret_cast<HWND>(w.winId()); // 获取窗口句柄
	w.setDisabled(true);

	void* ptr = reinterpret_cast<void*>(parentHwnd);

	CGRender::CGRenderView* renderView = new CGRender::CGRenderView(WindowWidth, WindowHeight, ptr);
	std::shared_ptr<CGRender::Window>mainwindow = renderView->getWindowByType(CGRender::WindowType::Window_Main);
	//HWND hwnd = (HWND)mainwindow->GetWindowHwnd();

	//QWidget* glfw_ContainerWidget = nullptr;
	//QWindow* externWindow = QWindow::fromWinId((WId)hwnd);
	//glfw_ContainerWidget = QWidget::createWindowContainer(externWindow, &w);
	//glfw_ContainerWidget->resize(WindowWidth, WindowHeight);
	//auto newWidget = QWidget::find((WId)hwnd); //需要用（WID）
	int i = 0;

	int contextID = mainwindow->getGLContextID();

	int renderTarget = 0;

	{
		ImageData imageData;
		getImageData(L"D:/document/2024/March/20240327/010_L01S.img", imageData);

		//int texture = CGRender_CreateTextureFromData(contextID, imageData.data, imageData.width, imageData.height, GLTextureType::GLTexture_Raw16);

		int textureid = 0;
		int textureid1 = 0;

		int indexIndex;
		int vboIndex;

		int uniformBufferid = -1;

		{
			textureid1 = CGRender_CreateTextureFromFile(contextID, L"D:/document/2024/March/20240327/20240401095541.png", 0, 0, GLTexture_Normal2DTex);
			textureid = CGRender_CreateTextureFromData(contextID, imageData.data, imageData.width, imageData.height, GLTextureType::GLTexture_Raw16);
			renderTarget = CGRender_CreateTextureFromData(contextID, 0, WindowWidth, WindowHeight, GLTexture_Normal2DTex);
			//renderTarget = CGRender_CreateTextureFromData(contextID, 0, imageData.width, imageData.height, GLTexture_Normal2DTex);
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
					transform = glm::translate(transform, glm::vec3(0.5f, 0.5f, 0.0f));
					transform = glm::scale(transform, glm::vec3(.5f, .5f, 1.f));
					//transform = glm::rotate(transform, (float)1.0, glm::vec3(0.0f, 0.0f, 1.0f));
					unidata.matrix = transform;

					uniformBufferid = CGRender_CreateBuffer(contextID, sizeof(UniformBufferData), 1, &unidata, GLBufferType::uniformBuffer);
					//int vsptmvp = CGRender_CreateShader(contextID, ShaderCodeName::VS_POS_TEX_MVPMatrix);
					//int vsptmvp = CGRender_CreateShader(contextID, ShaderCodeName::VS_POS_COLOR_TEX);//test
					int psshader = CGRender_CreateShader(contextID, ShaderCodeName::FSTestUniform);
					CGRender_SetShader(contextID, psshader, ShaderType::FRAGMENT);
					CGRender_SetUniformBuffer(contextID, uniformBufferid, 0, ShaderType::VERTEX);
					//CGRender_SetUniformBuffer(contextID, uniformBufferid, 0, ShaderType::FRAGMENT);
				}
			}
		}

		CGRender_SetShaderTexture(contextID, textureid1, 1, ShaderType::FRAGMENT);
		CGRender_SetShaderTexture(contextID, textureid, 0, ShaderType::FRAGMENT);

		CGRender_SetRenderTarget(contextID, renderTarget);
		//CGRender_SetViewport(contextID, 0, 0, 1920, 1080);
		CGRender_SetViewport(contextID, 0, 0, WindowWidth, WindowHeight);

		//glfw_ContainerWidget->resize(WindowWidth, WindowHeight);


	}
	std::thread renderThread{ [&] {
	while (true)
	{
		renderView->Render();
	}
	} };

	renderThread.detach();


	return app->exec();
#if 0
	//std::wstring szBinPath = qApp->applicationDirPath().toStdWString();
	//std::wstring  strBin(szBinPath);
	//std::wstring  strPlug = strBin + L".\\plug_in";
	//std::wstring  strPlug_qt = strPlug + L"\\qt";
	//QApplication::addLibraryPath(QString::fromStdWString(strPlug_qt));

	ImageData imageData;
	getImageData(L"D:/document/2024/March/20240327/010_L01S.img", imageData);



	HWND hwnd;
	{
		w.resize(960, 540);
		w.show();


		WId windowId = w.winId();

		QWindow* window = QWindow::fromWinId(windowId);
		hwnd = reinterpret_cast<HWND>(window->winId());
	}


	//initgl
	bool isInit = CGRenderGL::CGRenderGLInitGL();
	assert(isInit);

	CGRenderGL::GLContext* glcontext = new CGRenderGL::GLContext();
	int vsshader = 0;
	int psshader = 0;
	int textureid = 0;

	int indexIndex;
	int vboIndex;
	{
		glcontext->setHWND(hwnd);
		//textureid = glcontext->glCreateTextureFromPath(L"D:/document/2024/March/20240327/20240401095541.png");
		textureid = glcontext->glCreateTextureFromData(imageData.data, imageData.width, imageData.height);

		vsshader = glcontext->glCreateShaderFromName(ShaderCodeName::VS_POS_TEX_MVP);
		psshader = glcontext->glCreateShaderFromName(ShaderCodeName::FSGray);

		{
			indexIndex = glcontext->glCreateBuffer(sizeof(indexes), 1, (void*)indexes, GLBufferType::IndexBuffer, GLPrimitiveTypes::TRIANGLELIST);
			vboIndex = glcontext->glCreateBuffer(sizeof(vertices), 1, (void*)vertices, GLBufferType::VertexBuffer, GLPrimitiveTypes::TRIANGLELIST);
		}
	}

	QTDisplayWidget w1;
	HWND hwnd1;
	CGRenderGL::GLContext* glcontext1 = new CGRenderGL::GLContext();
	{
		w1.resize(800, 600);
		//w1.show();


		WId windowId = w1.winId();

		QWindow* window = QWindow::fromWinId(windowId);
		hwnd1 = reinterpret_cast<HWND>(window->winId());
#ifdef useMutilView
		glcontext1->setHWND(hwnd1, glcontext->getShareGLContext());
#endif // useMutilView

	}


	std::thread renderThread{ [&] {
		int nums = 0;
	while (true)
	{

		// 创建随机数生成器引擎
		std::random_device rd;
		std::mt19937 gen(rd()); // 使用 Mersenne Twister 引擎

		// 创建均匀分布
		std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		float randomNum = dis(gen);

		if (nums == 100)
		{
			nums = 0;
			randomNum = 0.0;

		}
		// 生成随机数

		//glcontext->renderTest(randomNum);
		glcontext->renderTest(textureid,vsshader,psshader, vboIndex,indexIndex);
		//glcontext1->renderTest(textureid,vsshader,psshader);
		//glcontext1->renderTest(0);
#ifdef useMutilView
		glcontext1->renderTest(nums * 2 == 100 || nums == 100 ? 0 : randomNum);
#endif // useMutilView
		nums++;
	}
	} };

	renderThread.detach();

	//return 0;
#endif // 0
}