#include "QtDisplayWidget.h"
#include "ui_QtDisplayWidget.h"


#include "CGRenderView.h"
#include "CGRender.h"
#include "CGData.h"

#include <memory>
#include <thread>
#include <qtimer.h>
//#include <qfile.h>
#include <qfiledialog.h>
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

struct QTDisplayWidget::PrivateData
{
	Ui_MainWidget ui;
	std::unique_ptr<CGRender::CGRenderView>renderview = nullptr;
	CGRender::WindowsWindow* glWindow = nullptr;
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
	d.renderview = std::make_unique<CGRender::CGRenderView>(width, height, parentHwnd);
	{

		auto renderWindow = d.renderview->getWindowByType(CGRender::WindowType::Window_Main);
		d.glWindow = dynamic_cast<CGRender::WindowsWindow*>(renderWindow.get());
		assert(d.glWindow);

		int contextId = renderWindow->ContextID();
		int renderTarget = CGRender_CreateTextureFromData(contextId, 0, width, height, GLTexture_Normal2DTex);
		CGRender_SetRenderTarget(contextId, renderTarget);

		//主循环
		QTimer* timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, &QTDisplayWidget::runLoop);
		timer->start(10);
	}

	connect(d.ui.pbOpenImage, &QPushButton::clicked, this, [&]() {
		// 设置文件过滤器，仅显示 PNG 和 JPEG 格式的文件
		QStringList filters;
		//filters << "PNG Files (*.png)" << "JPEG Files (*.jpeg *.jpg)";
		filters << "PNG Files (*.png);;JPEG Files (*.jpeg *.jpg)";

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
		});

	connect(d.ui.pbGrayColor, &QPushButton::clicked, this, [&]() {
		auto layer= d.glWindow->getCurLayer();
		layer->setImageShader(ShaderCodeName::FS_Tex_Gray);
		});

	connect(d.ui.pbInvertColor, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		layer->setImageShader(ShaderCodeName::FS_Tex_Invert);
		});

	connect(d.ui.pbNormalColor, &QPushButton::clicked, this, [&]() {
		auto layer = d.glWindow->getCurLayer();
		layer->setImageShader(ShaderCodeName::FS_Tex);
		});

}

QTDisplayWidget::~QTDisplayWidget()
{

}


void QTDisplayWidget::runLoop()
{
	auto& d = *m_priv;
	d.renderview->Render();
}
