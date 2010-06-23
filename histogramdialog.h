#ifndef HISTOGRAMDIALOG_H
#define HISTOGRAMDIALOG_H

#include <QDialog>
#include <QVector>
#include <QByteArray>
#include "ui_histogramdialog.h"
#include "world_params.h"

class HistogramDialog : public QDialog
{
	Q_OBJECT

public:
	HistogramDialog(const WorldParams &params, QWidget *parent = 0);
	~HistogramDialog();

public slots:
	void newData(const QVector<QByteArray> &data, int pop);
signals:
	void geneSelected(int gene);

protected:
	void paintEvent(QPaintEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);

private:
	Ui::HistogramDialogClass ui;
	QVector<QByteArray> dnaData;
	double *variance;
	WorldParams params;
	int population;
	QColor colorTable[256];
};

#endif // HISTOGRAMDIALOG_H
