#ifndef HISTOGRAMDIALOG_H
#define HISTOGRAMDIALOG_H

#include <QDialog>
#include <QMap>
#include "ui_histogramdialog.h"

class HistogramDialog : public QDialog
{
	Q_OBJECT

public:
	HistogramDialog(QWidget *parent = 0);
	~HistogramDialog();

public slots:
	void newData(const QMap<int, int> &data, int pop);
protected:
	void paintEvent(QPaintEvent *ev);
private:
	Ui::HistogramDialogClass ui;
	QMap<int, int> histData;
	int population;
};

#endif // HISTOGRAMDIALOG_H
