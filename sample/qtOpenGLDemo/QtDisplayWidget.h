#pragma once

#include <QWidget>

#define GREY_COLOR_BACKGROUND 0xFF4C4C4C

class QTDisplayWidget : public QWidget {
	Q_OBJECT
		Q_PROPERTY(QColor displayBackgroundColor MEMBER backgroundColor READ
			GetDisplayBackgroundColor WRITE
			SetDisplayBackgroundColor)


	void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	// 重新实现event函数，禁用所有事件处理
	bool event(QEvent* event) override {
		// 返回true表示事件已处理，不会继续传递处理
		// 返回false表示事件未处理，会继续传递给父类处理
		return true;
	}
	bool eventFilter(QObject* obj, QEvent* event)override;

signals:
	//void DisplayCreated(OBSQTDisplay* window);
	void DisplayResized();

public:
	QTDisplayWidget(QWidget* parent = nullptr,
		Qt::WindowFlags flags = Qt::WindowFlags());
	~QTDisplayWidget() {}

	virtual QPaintEngine* paintEngine() const override;

	//inline obs_display_t* GetDisplay() const { return display; }

	uint32_t backgroundColor = GREY_COLOR_BACKGROUND;

	QColor GetDisplayBackgroundColor() const;
	void SetDisplayBackgroundColor(const QColor& color);
	void UpdateDisplayBackgroundColor();
	void CreateDisplay(bool force = false);
};
