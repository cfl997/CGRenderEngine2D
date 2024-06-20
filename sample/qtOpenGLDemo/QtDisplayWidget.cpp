#include "QtDisplayWidget.h"
#include "ui_QtDisplayWidget.h"
#include <QResizeEvent>


#include "CGRenderView.h"
#include "CGRender.h"
#include "CGData.h"

#include "CGWindow.h"
#include "CGWindowsWindos.h"

#include <memory>
#include <thread>
#include <qtimer.h>
//#include <qfile.h>
#include <qfiledialog.h>
#include <iostream>

#include "Render2D.h"
#ifdef _DEBUG
#pragma comment (lib,"Render2DD.lib")
#else
#pragma comment (lib,"Render2D.lib")
#endif // _DEBUG

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

static bool getImageData(const std::wstring& path, int width, int height, ImageData& imageData)
{
	FILE* fp = 0;

	_wfopen_s(&fp, path.c_str(), L"rb");
	if (!fp)
	{
		return false;
	}
	int bytes = 2;
	long bufsize = width * height * bytes;
	char* szbuf = new char[bufsize];
	if (1 == fread(szbuf, bufsize, 1, fp))
	{
		fclose(fp);
		imageData.width = width;
		imageData.height = height;
		imageData.isLoad = true;
		imageData.data = new char[bytes * width * height];
		std::copy(szbuf, szbuf + bytes * width * height, imageData.data);
		delete szbuf;
		szbuf = nullptr;
		return true;
	}

	fclose(fp);

	return false;
}
#include <fstream>

static bool getImageDataHeader(const char* path, ImageData& imageData)
{
	std::ifstream in(path, std::ios::in | std::ios::binary);
	if (in.is_open()) {
		struct {
			//unsigned short n0, width, height, n1, bits, energy, order, inverse, n[248];
			int width, height, n1, n2;
		} header;
		if (in.read((char*)&header, sizeof(header))) {
			int width = header.width;
			int height = header.height;
			int bytes = 2;
			int bufsize = width * height * bytes;
			char* szbuf = new char[bufsize];
			if (in.read(szbuf, bufsize)) {
				imageData.width = width;
				imageData.height = height;
				imageData.isLoad = true;
				imageData.data = new char[bytes * width * height];
				std::copy(szbuf, szbuf + bytes * width * height, imageData.data);
				delete szbuf;
				szbuf = nullptr;
				return true;
			}
		}
	}
	in.close();
	return false;

}


uint16_t ConvertColorToRGB565(uint32_t color) {
	// 提取RGB分量
	uint8_t r = (color >> 16) & 0xFF;
	uint8_t g = (color >> 8) & 0xFF;
	uint8_t b = color & 0xFF;

	// 转换为RGB565格式
	uint16_t r5 = (r * 31) / 255;
	uint16_t g6 = (g * 63) / 255;
	uint16_t b5 = (b * 31) / 255;

	// 打包成一个16位整数
	uint16_t rgb565 = (r5 << 11) | (g6 << 5) | b5;
	return rgb565;
}

const std::wstring g_oneWindowTitle = L"oneTitleWindow";

#include "Render2D.h"
#define use_Render2D
#include <mutex>

struct QTDisplayWidget::PrivateData
{
	Ui_MainWidget ui;
	bool use16unsignedint = false;
	std::recursive_mutex mutex;
#ifndef use_Render2D


	std::unique_ptr<CGRender::CGRenderView>renderview = nullptr;
	CGRender::WindowsWindow* glWindow = nullptr;


	CGRender::WindowsWindow* oneTitleWindow = nullptr;
	CGData::CGItemImage* image = nullptr;

#endif // !use_Render2D
#ifdef use_Render2D
	Render2D* render2D = nullptr;
#endif // use_Render2D

};
#ifdef use_Render2D
QTDisplayWidget::QTDisplayWidget(QWidget* parent)
	: QWidget(parent), m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.ui.setupUi(this);
	d.ui.leImagePath->setReadOnly(true);

	int width = d.ui.w_glwindow->geometry().width();
	int height = d.ui.w_glwindow->geometry().height();

	HWND parentHwnd = reinterpret_cast<HWND>(d.ui.w_glwindow->winId()); // 获取窗口句柄
	RECT rect;
	GetWindowRect(parentHwnd, &rect);

	d.render2D = new Render2D();
	d.render2D->Create(parentHwnd, width, height);

	int RollingHeight = 1000;
	{
		//d.render2D->SetMode(1);
		d.render2D->SetDirection(0);
		d.render2D->SetHeight(RollingHeight);
		d.use16unsignedint = true;
	}




	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &QTDisplayWidget::runLoop);
	timer->start(10);

	{
		//int contextId = renderWindow->ContextID();
		//int renderTarget = CGRender_CreateTextureFromData(contextId, 0, width, height, GLTexture_Normal2DTex);
		//CGRender_SetRenderTarget(contextId, renderTarget);

		//主循环
		//QTimer* timer = new QTimer(this);
		//connect(timer, &QTimer::timeout, this, &QTDisplayWidget::runLoop);
		//timer->start(10);
	}



	connect(d.ui.pbOpenImage, &QPushButton::clicked, this, [&]() {
		// 设置文件过滤器，仅显示 PNG 和 JPEG 格式的文件
		QStringList filters;
		filters << "All File(*.*)" << "PNG Files (*.png)" << "JPEG Files (*.jpeg *.jpg)";

		// 弹出文件选择对话框，设置过滤器
		QString selectedFile = QFileDialog::getOpenFileName(this, "Select Image", QDir::homePath(), filters.join(";;"));

		// 如果用户选择了文件，则输出文件路径
		if (selectedFile.isEmpty())
			return;
		{
			qDebug() << "Selected file:" << selectedFile;
		}

		// 创建 QFileInfo 对象
		QFileInfo fileInfo(selectedFile);

		// 获取文件后缀
		QString fileSuffix = fileInfo.suffix();
		if (fileSuffix == QString("raw"))
		{

			int width = d.ui.leW->text().toInt();
			int height = d.ui.leH->text().toInt();
			ImageData imageData;
			//getImageData(selectedFile.toStdWString(), width, height, imageData);
			//getImageData(selectedFile.toStdWString(), 496, 448, imageData);
			getImageDataHeader(selectedFile.toStdString().c_str(), imageData);
			d.render2D->SetImage(imageData.data, imageData.width, imageData.height, false);
		}
		else
		{
			assert(0);
		}

		d.ui.leImagePath->setText(selectedFile);

		});




	connect(d.ui.pbGrayColor, &QPushButton::clicked, this, [&]() {
		//auto layer = d.glWindow->getCurLayer();

		//layer->addImageShader(ShaderCodeName::FS_Tex_Red_equalizeHistogramSource);
		});

	connect(d.ui.pbInvertColor, &QPushButton::clicked, this, [&]() {
		//auto layer = d.glWindow->getCurLayer();
		////layer->addImageShader(ShaderCodeName::FS_Tex_Invert);
		//layer->addImageShader(ShaderCodeName::FS_Tex_Red_Invert);
		});

	//connect(d.ui.pbNormalColor, &QPushButton::clicked, this, [&]() {
	//	//auto layer = d.glWindow->getCurLayer();
	//	//layer->addImageShader(ShaderCodeName::FS_Tex);
	//	});
	connect(d.ui.pbRotate90, &QPushButton::clicked, this, [&]() {
		//auto layer = d.glWindow->getCurLayer();
		//layer->addImageShader(ShaderCodeName::FS_Tex_Rotate90);
		});
	connect(d.ui.pbClearAll, &QPushButton::clicked, this, [&]() {
		//auto layer = d.glWindow->getCurLayer();
		//layer->removeAllImageShader();
		});
	//auto layoutInsert = [&](bool visible) {
	//	for (int i = 0; i < d.ui.vlayoutInsert->count(); ++i) {
	//		QWidget* w = d.ui.vlayoutInsert->itemAt(i)->widget();
	//		if (w != NULL)
	//			w->setVisible(visible);
	//	}
	//	};

	//layoutInsert(false);

	//connect(d.ui.pbmove, &QPushButton::clicked, this, [&, layoutInsert]() {
	//	//if (d.image)
	//	//{
	//	//	d.image = nullptr;
	//	//	layoutInsert(false);
	//	//	return;
	//	//}
	//	//auto layer = d.glWindow->getCurLayer();
	//	//auto item = layer->getItem(CGData::CGItemType::CGItemImage);
	//	//if (item.size() <= 0)
	//	//	return;

	//	//d.image = dynamic_cast<CGData::CGItemImage*>(item.at(0));
	//	//if (!d.image)
	//	//	return;

	//	//layoutInsert(true);

	//	});

	static int insertWidth = 100;
	DWORD pbcolor1 = 0xffCDFAFF;
	DWORD pbcolor2 = 0xffFFFF97;


	{
		QColor color1(0xffFFFACD);


		QPalette pbColorPalette = d.ui.pbInsert1->palette();
		d.ui.pbInsert1->setAutoFillBackground(true);
		d.ui.pbInsert1->setFlat(true);
		pbColorPalette.setBrush(QPalette::Button, color1);
		d.ui.pbInsert1->setPalette(pbColorPalette);
	}

	{
		QColor color2(0xff97FFFF);

		QPalette pbColorPalette = d.ui.pbInsert2->palette();
		d.ui.pbInsert2->setAutoFillBackground(true);
		d.ui.pbInsert2->setFlat(true);
		pbColorPalette.setBrush(QPalette::Button, color2);
		d.ui.pbInsert2->setPalette(pbColorPalette);
	}

	uint16_t white = ConvertColorToRGB565(0xffffffff);
	auto insertcolor = [&, RollingHeight](DWORD color) {
		//if (!d.image)
		//	return;
		//int width = d.image->width();
		int height = RollingHeight;
		int width = insertWidth;
		if (d.use16unsignedint)
		{

			uint16_t color16 = ConvertColorToRGB565(color);
			char* colorbuffer = new char[width * height * 2];

			for (int i = 0; i < width * height * 2; i += 2) {
				int white = i % height;
				if (white >= 0 && white <= 10)
				{
					colorbuffer[i] = white & 0xff;
					colorbuffer[i + 1] = (white >> 8) & 0xff;
					continue;
				}

				colorbuffer[i] = color16 & 0xFF;
				colorbuffer[i + 1] = (color16 >> 8) & 0xFF;
			}

			//d.image->updateData(colorbuffer, width, insertHeight);
			d.render2D->SetImage(colorbuffer, width, height, false);
			d.render2D->Update();
			delete[]colorbuffer;
			return;
		}
		assert(0);
		};

	connect(d.ui.pbInsert1, &QPushButton::clicked, this, [&, insertcolor, pbcolor1]() {
		insertcolor(pbcolor1);
		});

	connect(d.ui.pbInsert2, &QPushButton::clicked, this, [&, insertcolor, pbcolor2]() {
		insertcolor(pbcolor2);
		});

	//new Window


	connect(d.ui.pbInsertWindow, &QPushButton::clicked, this, [&]() {
		//if (!d.oneTitleWindow)
		//	return;
		//auto items = d.glWindow->getCurLayer()->getItem(CGData::CGItemType::CGItemImage);
		//if (items.empty())
		//	return;
		//CGData::CGItemImage* oldImage = dynamic_cast<CGData::CGItemImage*>(items.at(0));
		//std::wstring path = oldImage->path();


		//if (!d.oneTitleWindow)
		//	return;
		//auto layer = d.oneTitleWindow->getCurLayer();
		//d.oneTitleWindow->addImage(path);
		//d.oneTitleWindow->syncWindowByParent(d.glWindow);

		});

	connect(d.ui.pbDrawRect, &QPushButton::clicked, this, [&]() {
		Parameter parameter;
		parameter.rbutton_mode = 1;
		parameter.windowType = 0;
		d.render2D->SetParameter(parameter);

		});



	connect(d.ui.pbDrawRectWindow, &QPushButton::clicked, this, [&]() {
		Parameter parameter;
		parameter.rbutton_mode = 2;
		parameter.windowType = 0;
		d.render2D->SetParameter(parameter);
		});

	connect(d.ui.pbInsertText, &QPushButton::clicked, this, [&]() {
		/*Label label;
		label.code = 1;
		label.type = -1;
		label.x = 0;
		label.y = 0;
		label.w = 30;
		label.h = 40;*/

		Label* label = new Label[2];
		label[0].code = 1;
		label[0].type = -1;
		label[0].x = 0;
		label[0].y = 0;
		label[0].w = 30;
		label[0].h = 40;

		label[1].code = 2;
		label[1].type = -1;
		label[1].x = -100;
		label[1].y = -100;
		label[1].w = 40;
		label[1].h = 90;

		d.render2D->SetLabels(label, 2);
		});


}
#else
QTDisplayWidget::QTDisplayWidget(QWidget* parent)
	: QWidget(parent), m_priv(new PrivateData)
{
	auto& d = *m_priv;
	d.ui.setupUi(this);
	d.ui.leImagePath->setReadOnly(true);

	int width = d.ui.w_glwindow->geometry().width();
	int height = d.ui.w_glwindow->geometry().height();

	HWND parentHwnd = reinterpret_cast<HWND>(d.ui.w_glwindow->winId()); // 获取窗口句柄
	RECT rect;
	GetWindowRect(parentHwnd, &rect);

	auto parentWindowPos = d.ui.w_glwindow->pos();
	int x = parentWindowPos.x();
	int y = parentWindowPos.y();


	d.renderview = std::make_unique<CGRender::CGRenderView>(width, height, parentHwnd);
	{

		auto renderWindow = d.renderview->getWindowByTitle(CGRender::g_windowMainStr.c_str());

		d.glWindow = dynamic_cast<CGRender::WindowsWindow*>((CGRender::Window*)renderWindow);


		//int contextId = renderWindow->ContextID();
		//int renderTarget = CGRender_CreateTextureFromData(contextId, 0, width, height, GLTexture_Normal2DTex);
		//CGRender_SetRenderTarget(contextId, renderTarget);

		//主循环
		QTimer* timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, &QTDisplayWidget::runLoop);
		timer->start(10);
	}
	auto layoutInsert = [&](bool visible) {
		for (int i = 0; i < d.ui.vlayoutInsert->count(); ++i) {
			QWidget* w = d.ui.vlayoutInsert->itemAt(i)->widget();
			if (w != NULL)
				w->setVisible(visible);
		}
		};

	layoutInsert(false);


	connect(d.ui.pbOpenImage, &QPushButton::clicked, this, [&]() {
		// 设置文件过滤器，仅显示 PNG 和 JPEG 格式的文件
		QStringList filters;
		filters << "All File(*.*)" << "PNG Files (*.png)" << "JPEG Files (*.jpeg *.jpg)";

		// 弹出文件选择对话框，设置过滤器
		QString selectedFile = QFileDialog::getOpenFileName(this, "Select Image", QDir::homePath(), filters.join(";;"));

		// 如果用户选择了文件，则输出文件路径
		if (selectedFile.isEmpty())
			return;
		{
			qDebug() << "Selected file:" << selectedFile;
		}

		// 创建 QFileInfo 对象
		QFileInfo fileInfo(selectedFile);

		// 获取文件后缀
		QString fileSuffix = fileInfo.suffix();
		if (fileSuffix == QString("raw"))
		{
			int width = d.ui.leW->text().toInt();
			int height = d.ui.leH->text().toInt();
			ImageData imageData;
			getImageData(selectedFile.toStdWString(), width, height, imageData);
			//getImageData(selectedFile.toStdWString(), 496, 448, imageData);

			d.glWindow->addImage(imageData.data, imageData.width, imageData.height, false, GLTextureType::GLTexture_Raw16);
			d.use16unsignedint = true;
		}
		else
		{
			d.glWindow->addImage(selectedFile.toStdWString());
			d.use16unsignedint = false;
		}

		d.ui.leImagePath->setText(selectedFile);

		d.image = nullptr;
		});

	connect(d.ui.pbGrayColor, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		//layer->addImageShader(ShaderCodeName::FS_Tex_Gray);
		//layer->addImageShader(ShaderCodeName::FS_Tex_RGBA_equalizeHistogramSource);
		layer->addImageShader(ShaderCodeName::FS_Tex_Red_equalizeHistogramSource);
		});

	connect(d.ui.pbInvertColor, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		//layer->addImageShader(ShaderCodeName::FS_Tex_Invert);
		layer->addImageShader(ShaderCodeName::FS_Tex_Red_Invert);
		});

	connect(d.ui.pbxRevert, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		//layer->addImageShader(ShaderCodeName::FS_Tex);
		layer->addImageShader(ShaderCodeName::FS_Tex_Red_xRevert);

		});
	connect(d.ui.pbRotate90, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		layer->addImageShader(ShaderCodeName::FS_Tex_Rotate90);
		});
	connect(d.ui.pbWindowWidthLevel, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		d.glWindow->setWindowWidthLevel(4000, 20000);
		layer->addImageShader(ShaderCodeName::CUDA_WindowWidthLevel);
		});

	connect(d.ui.pbClearAll, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		layer->removeAllImageShader();
		});


	connect(d.ui.pbmove, &QPushButton::clicked, this, [&, layoutInsert]() {
		if (d.image)
		{
			d.image = nullptr;
			layoutInsert(false);
			return;
		}
		auto layer = d.glWindow->getCurLayer();
		auto item = layer->getItem(CGData::CGItemType::CGItemImage);
		if (item.size() <= 0)
			return;

		d.image = dynamic_cast<CGData::CGItemImage*>(item.at(0));
		if (!d.image)
			return;

		layoutInsert(true);

		});

	static int insertHeight = 100;
	DWORD pbcolor1 = 0xffCDFAFF;
	DWORD pbcolor2 = 0xffFFFF97;



	{
		QColor color1(0xffFFFACD);


		QPalette pbColorPalette = d.ui.pbInsert1->palette();
		d.ui.pbInsert1->setAutoFillBackground(true);
		d.ui.pbInsert1->setFlat(true);
		pbColorPalette.setBrush(QPalette::Button, color1);
		d.ui.pbInsert1->setPalette(pbColorPalette);
	}

	{
		QColor color2(0xff97FFFF);

		QPalette pbColorPalette = d.ui.pbInsert2->palette();
		d.ui.pbInsert2->setAutoFillBackground(true);
		d.ui.pbInsert2->setFlat(true);
		pbColorPalette.setBrush(QPalette::Button, color2);
		d.ui.pbInsert2->setPalette(pbColorPalette);
	}


	auto insertcolor = [&](DWORD color) {
		if (!d.image)
			return;
		int width = d.image->width();
		if (d.use16unsignedint)
		{

			uint16_t color16 = ConvertColorToRGB565(color);
			char* colorbuffer = new char[width * insertHeight * 2];

			for (int i = 0; i < width * insertHeight * 2; i += 2) {
				colorbuffer[i] = color16 & 0xFF;
				colorbuffer[i + 1] = (color16 >> 8) & 0xFF;
			}

			d.image->updateData(colorbuffer, width, insertHeight);
			delete[]colorbuffer;
			return;
		}

		{
			DWORD* colorbuffer = new DWORD[width * insertHeight];
			for (int i = 0; i < width * insertHeight; i++)
			{
				colorbuffer[i] = color;
			}
			d.image->updateData(colorbuffer, width, insertHeight);
			delete[]colorbuffer;
		}



		};

	connect(d.ui.pbInsert1, &QPushButton::clicked, this, [&, insertcolor, pbcolor1]() {
		insertcolor(pbcolor1);
		});

	connect(d.ui.pbInsert2, &QPushButton::clicked, this, [&, insertcolor, pbcolor2]() {
		insertcolor(pbcolor2);
		});

	//new Window
	if (0)
	{
		CGRender::WindowProps windowProps;
		windowProps.parentWindow = nullptr;
		windowProps.Title = g_oneWindowTitle;
		windowProps.type = WindowType::Window_One;
		windowProps.windowWidth = 500;
		windowProps.windowHeight = 300;
		windowProps.share_Window = d.glWindow->GetNativeWindow();
		//d.renderview->createWindow(windowProps);
		d.renderview->createWindow(nullptr, wstr2utf8(g_oneWindowTitle).c_str(), 500, 300, d.glWindow->GetNativeWindow(), WindowType::Window_One);


		auto renderWindow = d.renderview->getWindowByTitle(g_oneWindowTitle.c_str());
		assert(renderWindow);
		d.oneTitleWindow = dynamic_cast<CGRender::WindowsWindow*>((CGRender::Window*)renderWindow);
		assert(d.oneTitleWindow);

		//int contextId = d.oneTitleWindow->ContextID();
		//int renderTarget = CGRender_CreateTextureFromData(contextId, 0, 500, 300, GLTexture_Normal2DTex);
		//CGRender_SetRenderTarget(contextId, renderTarget);
	}

	connect(d.ui.pbInsertWindow, &QPushButton::clicked, this, [&]() {
		if (!d.oneTitleWindow)
			return;
		auto items = d.glWindow->getCurLayer()->getItem(CGData::CGItemType::CGItemImage);
		if (items.empty())
			return;
		CGData::CGItemImage* oldImage = dynamic_cast<CGData::CGItemImage*>(items.at(0));
		std::wstring path = oldImage->path();


		if (!d.oneTitleWindow)
			return;
		auto layer = d.oneTitleWindow->getCurLayer();
		d.oneTitleWindow->addImage(path);
		d.oneTitleWindow->syncWindowByParent(d.glWindow);

		});

	connect(d.ui.pbDrawRect, &QPushButton::clicked, this, [&]() {
		d.glWindow->addCGRenderEvent(CGRenderEventType::RenderEvent_Rectangle);
		});

	connect(d.ui.pbDrawRectWindow, &QPushButton::clicked, this, [&]() {
		d.glWindow->addCGRenderEvent(CGRenderEventType::RenderEvent_RectWindow);
		});

	connect(d.ui.pbInsertText, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		CGData::CGItemText* text = new CGData::CGItemText(d.glWindow->ContextID());
		QString letext = d.ui.leText->text();
		text->setText(letext.toStdWString());
		layer->addItem(text);
		});


}
#endif
QTDisplayWidget::~QTDisplayWidget()
{
	auto& d = *m_priv;
#ifdef use_Render2D
	d.render2D->Release();
#else
	d.mutex.lock();
	d.renderview.release();
	d.mutex.unlock();
#endif // use_Render2D
	if (m_priv)
	{
		delete m_priv;
		m_priv = nullptr;
	}
}

void QTDisplayWidget::closeEvent(QCloseEvent* event)
{
	auto& d = *m_priv;
	std::lock_guard lock(d.mutex);
#ifndef use_Render2D
	d.renderview.release();
#else
	if (d.render2D)
	{
		d.render2D->Release();
	}
#endif // use_Render2D

}

void QTDisplayWidget::resizeEvent(QResizeEvent* event)
{
	auto& d = *m_priv;
	std::lock_guard lock(d.mutex);
#ifdef use_Render2D
#else
	//QSize size = event->size();
	QSize size = d.ui.w_glwindow->size();
	d.glWindow->ResizeWindow(size.width(), size.height());
#endif // use_Render2D

}


void QTDisplayWidget::runLoop()
{
	auto& d = *m_priv;
	std::lock_guard lock(d.mutex);
#ifdef use_Render2D
	if (d.render2D)
	{
		d.render2D->Update();
	}
#else
	if (d.renderview.get())
		d.renderview->Render();
#endif // use_Render2D

}
