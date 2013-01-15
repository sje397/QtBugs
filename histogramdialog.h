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
	void geneSelected(int gene);
signals:
	void selectGene(int gene);

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
	int selectedGene;
};

#endif // HISTOGRAMDIALOG_H
