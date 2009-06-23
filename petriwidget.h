#ifndef _PETRI_WIDGET
#define _PETRI_WIDGET

#include <QWidget>
#include <QColor>
#include <QPixmap>
#include <QMutex>

class PetriWidget: public QWidget {
	Q_OBJECT
public:
	PetriWidget(QWidget *parent = 0);
	void set_image_size(int x, int y);	
	void update_pixel(int x, int y, unsigned char col);

signals:
	void clicked(int x, int y);

protected:
	QPixmap image;

	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent * event);

	QColor colorTable[256];	

	QPainter *imagePainter;
	QMutex imageMutex;
};

#endif
