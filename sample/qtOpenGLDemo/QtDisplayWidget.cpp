#include "QtDisplayWidget.h"
#include "ui_QtDisplayWidget.h"


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

//static inline long long color_to_int(const QColor& color)
//{
//	auto shift = [&](unsigned val, int shift) {
//		return ((val & 0xff) << shift);
//		};
//
//	return shift(color.red(), 0) | shift(color.green(), 8) |
//		shift(color.blue(), 16) | shift(color.alpha(), 24);
//}
//
//static inline QColor rgba_to_color(uint32_t rgba)
//{
//	return QColor::fromRgb(rgba & 0xFF, (rgba >> 8) & 0xFF,
//		(rgba >> 16) & 0xFF, (rgba >> 24) & 0xFF);
//}
const std::wstring g_oneWindowTitle = L"oneTitleWindow";

struct QTDisplayWidget::PrivateData
{
	Ui_MainWidget ui;
	std::unique_ptr<CGRender::CGRenderView>renderview = nullptr;
	CGRender::WindowsWindow* glWindow = nullptr;


	CGRender::WindowsWindow* oneTitleWindow = nullptr;

	CGData::CGItemImage* image = nullptr;
};

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
		filters << "PNG Files (*.png)" << "JPEG Files (*.jpeg *.jpg)" << "All File(*.*)";

		// 弹出文件选择对话框，设置过滤器
		QString selectedFile = QFileDialog::getOpenFileName(this, "Select Image", QDir::homePath(), filters.join(";;"));

		// 如果用户选择了文件，则输出文件路径
		if (selectedFile.isEmpty())
			return;
		{
			qDebug() << "Selected file:" << selectedFile;
		}
		d.glWindow->addImage(selectedFile.toStdWString());
		d.ui.leImagePath->setText(selectedFile);

		d.image = nullptr;
		});

	connect(d.ui.pbGrayColor, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		layer->addImageShader(ShaderCodeName::FS_Tex_Gray);
		});

	connect(d.ui.pbInvertColor, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		layer->addImageShader(ShaderCodeName::FS_Tex_Invert);
		});

	connect(d.ui.pbNormalColor, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		layer->addImageShader(ShaderCodeName::FS_Tex);
		});
	connect(d.ui.pbRotate90, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		layer->addImageShader(ShaderCodeName::FS_Tex_Rotate90);
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
		DWORD* colorbuffer = new DWORD[width * insertHeight];
		for (int i = 0; i < width * insertHeight; i++)
		{
			colorbuffer[i] = color;
		}

		d.image->updateData(colorbuffer, width, insertHeight);

		delete[]colorbuffer;
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

QTDisplayWidget::~QTDisplayWidget()
{
	auto& d = *m_priv;

	d.renderview.release();
	if (m_priv)
	{
		delete m_priv;
		m_priv = nullptr;
	}
}

void QTDisplayWidget::closeEvent(QCloseEvent* event)
{
	auto& d = *m_priv;
	d.renderview.release();
}


void QTDisplayWidget::runLoop()
{
	auto& d = *m_priv;

	if (d.renderview.get())
		d.renderview->Render();
}
