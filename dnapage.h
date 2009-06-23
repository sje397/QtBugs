#ifndef DNAPAGE_H
#define DNAPAGE_H

#include <QWidget>
#include "ui_dnapage.h"
#include "dna.h"

class DNAPage : public QWidget
{
	Q_OBJECT

public:
	DNAPage(QWidget *parent = 0);
	~DNAPage();

	void setDNA(DNA *d);

protected:
	void paintEvent(QPaintEvent *event);

private:
	Ui::DNAPageClass ui;
	DNA *dna;
};

#endif // DNAPAGE_H
