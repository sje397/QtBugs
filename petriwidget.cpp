#include "petriwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>

PetriWidget::PetriWidget(QWidget *parent): QWidget(0), imagePainter(0) {
	//setAttribute(Qt::WA_OpaquePaintEvent, true);

	float f;
	char r, g;
	for(int i = 1; i < 255; i++) {
		f = (i - 1) / 253.0f;
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
	colorTable[0] = QColor(qRgb(0, 0, 0));
	colorTable[255] = QColor(qRgb(0, 0, 255));

	/*
	QTimer *timer = new QTimer(this);
	timer->setInterval(100);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start();
	*/
}

void PetriWidget::set_image_size(int x, int y) {
	QMutexLocker locker(&imageMutex);
	image = QPixmap(x, y);
	//image.fill(Qt::black);
	if(imagePainter) delete imagePainter;
	imagePainter = new QPainter(&image);
}

void PetriWidget::update_pixel(int x, int y, unsigned char col) {
	QMutexLocker locker(&imageMutex);
	imagePainter->setPen(colorTable[col]);
	imagePainter->drawPoint(x, y);
}


void PetriWidget::paintEvent(QPaintEvent *event) {
	if(isVisible()) {
		QMutexLocker locker(&imageMutex);
		QPainter painter(this);	
		//painter.setRenderHint(QPainter::Antialiasing, false);
		//painter.setRenderHint(QPainter::SmoothPixmapTransform,false);
		painter.drawPixmap(rect(), image, image.rect());
	}
}

void PetriWidget::mousePressEvent(QMouseEvent *event) {
	int x = (int)(event->x() / (float)width() * image.width()),
		y = (int)(event->y() / (float)height() * image.height());

	emit clicked(x, y);
}
