#include "dnawidget.h"
#include <QPainter>

DNAWidget::DNAWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

DNAWidget::~DNAWidget()
{

}

void DNAWidget::setDNA(DNA *d) {
	dna = d;
}

inline QColor acol(char v) {
	char v1 = v / 9,
		v2 = (v - v1 * 9) / 3,
		v3 = v % 3;

	return QColor(v2 * 127, v1 * 127, v3 * 127);
}

void DNAWidget::paintEvent(QPaintEvent */*event*/) {
	if(!isVisible()) return;

	QPainter painter(this);
	QRect r = rect();
	r.adjust(20, 20, -20, -20);
	QByteArray b = dna->getData();

	int s = b.size(), h = height();
	int cols = s / h + (s % h == 0 ? 0 : 1);

	int pos;
	float w = width() / (float)cols, x = 0;
	unsigned char m, d;
	for(int i = 0; i < cols; i++) {
		for(int j = 0; j < h; j++) {
			pos = i * h + j;
			if(pos >= s) break;

			m = (b.at(pos) & 0x0f);
			d = ((b.at(pos) & 0xf0 )>> 4);

			painter.setPen(acol(m));
			painter.drawRect((int)x, j, (int)(w/3), 1);
			painter.setPen(acol(d));
			painter.drawRect((int)(x + w/3), j, (int)(w/3), 1);
		}
		x += w;
	}
}
