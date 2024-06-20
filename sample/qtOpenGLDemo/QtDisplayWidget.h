#pragma once

#include <QWidget>


class QTDisplayWidget : public QWidget
{
	Q_OBJECT


public:
	QTDisplayWidget(QWidget* parent = nullptr);
	~QTDisplayWidget();

public slots:
	void runLoop();

public:
	virtual void closeEvent(QCloseEvent* event)override;
	virtual void resizeEvent(QResizeEvent* event)override;

private:
	struct PrivateData;
	PrivateData* m_priv = nullptr;

};
