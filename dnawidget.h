#ifndef DNAWIDGET_H
#define DNAWIDGET_H

#include <QWidget>
#include "ui_dnawidget.h"
#include "dna.h"

class DNAWidget : public QWidget
{
	Q_OBJECT

public:
	DNAWidget(QWidget *parent = 0);
	~DNAWidget();

	void setDNA(DNA *d);

protected:
	void paintEvent(QPaintEvent *event);

private:
	Ui::dnawidgetClass ui;
	DNA *dna;
};

#endif // DNAWIDGET_H
