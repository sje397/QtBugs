#ifndef _PETRI_WIDGET
#define _PETRI_WIDGET

#include <QWidget>
#include <QColor>
#include <QImage>
#include <QMutex>
#include <QTime>
#include <QPainter>

class PetriWidget: public QWidget {
	Q_OBJECT
public:
	PetriWidget(QWidget *parent = 0);
	void set_image_size(int x, int y);	
	void update_pixel(int x, int y, unsigned char col);

public slots:
        void queueUpdate();

signals:
	void clicked(int x, int y);

protected:
        QImage image;
        QTime paintTime;
        QPainter painter;

	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent * event);

	QColor colorTable[256];	

	QPainter *imagePainter;
	QMutex imageMutex;
};

#endif
