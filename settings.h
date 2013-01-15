#ifndef _SETTINGS
#define _SETTINGS

#include "ui_settings.h"
#include "world_params.h"
#include <QWidget>

class Settings: public QWidget  {
	Q_OBJECT
	
public:
	Settings(QWidget *parent = 0);
	
	WorldParams getParams();
	void setParams(const WorldParams &p);
signals:
	void settingsChanged();

protected slots:
	void apply();
	
protected:

	WorldParams wp;
	Ui::Widget ui;
};

#endif
