#include "main_window.h"

#include <QApplication>
#include <QTime>

int main(int argc, char *argv[]) {
	
	QTime midnight(0, 0, 0);
	qsrand(midnight.secsTo(QTime::currentTime()));
	
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("Scott Ellis");
	QCoreApplication::setOrganizationDomain("www.scottellis.com.au");
	QCoreApplication::setApplicationName("Bugs");
	QCoreApplication::setAttribute(Qt::AA_MSWindowsUseDirect3DByDefault, true);
			
	MainWindow *win = new MainWindow();
	win->show();
	
	app.exec();
	
	delete win;
	return 0;
}
