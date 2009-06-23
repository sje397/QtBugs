#include "ui_main_window.h"
#include "petri_dish.h"

#include <QTimer>
#include <QTime>
#include <QAction>
#include <QString>
#include <QThread>

class MainWindow: public QMainWindow, public Ui::MainWindow {
	Q_OBJECT
	
public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	virtual ~MainWindow();
	bool isRunning();
public slots:
	void set_stats();
	void start_stop();
	void stop();
	void check_integrity();
	void step();	
	void updt();
	
	void new_world();
	void load_world();
	void save_world();
	void save_as_world();
	void autosave_world();
	void change_world();

	void edit_bug(int x, int y);

protected:
	class UpdateThread: public QThread {
	public:
		UpdateThread(MainWindow *mw, QObject *parent = 0): QThread(parent), win(mw), done(false) {}
		~UpdateThread() {
			finish();
			wait();
		}

		void finish() {done = true;}

		void run() {
			QTime midnight(0, 0, 0);
			qsrand(midnight.secsTo(QTime::currentTime()));

			unsigned int counter = 0;
			done = false;
			while(!done) {
				counter++;
				win->updt();
				if((counter % 10) == 0)
					usleep(1);
				//sleep(0);
			}
		}
	protected:
		bool done;
		MainWindow *win;
	};

	bool write_to_stream(QDataStream &out);

	PetriDish petri_dish;
	QTimer timer;
	QTime update_time;
	float updates_per_sec;
	
	QString filename;
	bool changed;

	bool auto_save;
	QTimer autosave_timer;

	UpdateThread update_thread;

private slots:
	void on_btnHist_clicked();
	void on_spinDNAVal_valueChanged(int);
};
