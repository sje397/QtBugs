#include "histogramdialog.h"
#include <QPainter>

HistogramDialog::HistogramDialog(QWidget *parent)
	: QDialog(parent), population(0)
{
	ui.setupUi(this);
}

HistogramDialog::~HistogramDialog()
{

}

void HistogramDialog::newData(const QMap<int, int> &data, int pop) {
	histData = data;
	population = pop;
	update();
}

void HistogramDialog::paintEvent(QPaintEvent *ev) {
	QPainter painter(this);
	int i;
	float w = width() / 256.0f, x = 0, h, max = 0;

	painter.setPen(Qt::black);
	painter.setBrush(Qt::blue);

	int hi = height() - 1, v;
	for(i = 0; i < 256; i++) {
		if((v = histData[i]) > 0) {
			h = hi - v / (float)population * hi;
			painter.drawRect(x, h, w, hi - h);
		}
		x += w;
	}
}
