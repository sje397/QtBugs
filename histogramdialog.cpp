#include "histogramdialog.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>

HistogramDialog::HistogramDialog(const WorldParams &p, QWidget *parent)
	: QDialog(parent), params(p), population(0)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose, true);

	float f;
	char r, g;
	for(int i = 0; i < 256; i++) {
		f = i / 255.0f;
		//colorTable[i] = QColor(qRgb((int)(0xff * cos(M_PI / 2 * r)), (int)(0xff * cos(M_PI/2 - M_PI /2 * r)), 0));
		//colorTable[i] = QColor(qRgb((int)(0xff * (1 - f)), (int)(0xff * f), 0));
		if(f <= 0.5) {
			r = (char)0xff;
			g = (char)(0xff * f * 2);
		} else {
			r = (char)(0xff * (1 - f) * 2);
			g = (char)0xff;
		}
		colorTable[i] = QColor(qRgb(r, g, 0));
	}

	QTimer *t = new QTimer(this);
	t->setInterval(500);
	connect(t, SIGNAL(timeout()), this, SLOT(update()));
	t->start();
}

HistogramDialog::~HistogramDialog()
{
}

void HistogramDialog::newData(const QVector<QByteArray> &data, int pop) {
	dnaData = data;
	population = pop;
	//update();
}

void HistogramDialog::paintEvent(QPaintEvent *ev) {
	QPainter painter(this);
	float wr = (width() - 1) / (float)params.max_data, h = (height() - 1), x = 0;
        int pop = dnaData.size();
	//qDebug() << "w:" << w << "h:" << h;

	int i, j, d, c, maxi;
	double scaled_v;
	for(i = 0; i < params.max_data; i++) {
		int buckets[256] = {0};
		maxi = 0;
                for(j = 0; j < pop; j++) {
                        if(dnaData[j].size() > i) {
				d = (unsigned char)dnaData[j][i];
				++buckets[d];
				if(buckets[d] > buckets[maxi]) maxi = d;
			}
		}

                if(pop > 0 && buckets[maxi] > 0) {
                    scaled_v = (pop - buckets[maxi]) / (double)pop;
                    c = (int)(scaled_v * 255);
                    painter.fillRect(x, (int)(h * (1 - scaled_v) + 0.5f), (int)(wr + 0.5f), h, colorTable[c]);
                }
		x += wr;
	}
}
