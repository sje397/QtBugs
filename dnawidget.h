#ifndef DNAWIDGET_H
#define DNAWIDGET_H

#include <QWidget>
#include "ui_dnawidget.h"

class dnawidget : public QWidget
{
	Q_OBJECT

public:
	dnawidget(QWidget *parent = 0);
	~dnawidget();

protected:
	void paintEvent(QPaintEvent *event);

private:
	Ui::dnawidgetClass ui;
};

#endif // DNAWIDGET_H
