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

private:
	struct PrivateData;
	PrivateData* m_priv = nullptr;

};
