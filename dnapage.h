#ifndef DNAPAGE_H
#define DNAPAGE_H

#include <QWidget>
#include "ui_dnapage.h"
#include "bug.h"

class DNAPage : public QWidget
{
	Q_OBJECT

public:
	DNAPage(QWidget *parent = 0);
	~DNAPage();

	void setBug(Bug *b);
public slots:
	void updateInfo();

private:
	Ui::DNAPageClass ui;
	Bug *bug;
};

#endif // DNAPAGE_H
