#include "main_window.h"
#include <QFileDialog>
#include <QMessageBox>
#include "settings.h"
#include "bugeditdialog.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags): QMainWindow(parent, flags), petri_dish(),
	histogramDialog(0), changed(true), auto_save(false), update_thread(this, this)
{
	setupUi(this);

	connect(&timer, SIGNAL(timeout()), &petri_dish, SLOT(step()));
	connect(&petri_dish, SIGNAL(changed()), worldView, SLOT(queueUpdate()));
	//connect(&petri_dish, SIGNAL(changed()), worldView, SLOT(repaint()));
	connect(&petri_dish, SIGNAL(changed()), this, SLOT(set_stats()));
	connect(runButton, SIGNAL(clicked()), this, SLOT(start_stop()));
	//connect(checkIntegrity, SIGNAL(clicked()), this, SLOT(check_integrity()));
	connect(stepBtn, SIGNAL(clicked()), this, SLOT(step()));

	connect(radViewDefault, SIGNAL(clicked()), &petri_dish, SLOT(set_default_view_mode()));
	connect(radViewBugs, SIGNAL(clicked()), &petri_dish, SLOT(set_bugs_view_mode()));
	connect(radViewEnergy, SIGNAL(clicked()), &petri_dish, SLOT(set_energy_view_mode()));
	connect(radViewAge, SIGNAL(clicked()), &petri_dish, SLOT(set_age_view_mode()));
	connect(radViewSize, SIGNAL(clicked()), &petri_dish, SLOT(set_dna_size_view_mode()));
	connect(radViewDNA, SIGNAL(clicked()), &petri_dish, SLOT(set_dna_view_mode()));
	connect(radViewTerrain, SIGNAL(clicked()), &petri_dish, SLOT(set_terrain_view_mode()));

	connect(actionNew, SIGNAL(triggered()), this, SLOT(new_world()));
	connect(actionSave, SIGNAL(triggered()), this, SLOT(save_world()));
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(load_world()));
	connect(actionSave_As, SIGNAL(triggered()), this, SLOT(save_as_world()));
	connect(settings, SIGNAL(settingsChanged()), this, SLOT(change_world()));
	connect(actionAutoSave, SIGNAL(triggered()), this, SLOT(autosave_world()));

	connect(actionCheck_Integrity, SIGNAL(triggered()), this, SLOT(check_integrity()));

	connect(worldView, SIGNAL(clicked(int, int)), this, SLOT(edit_bug(int, int)));

	connect(idleChk, SIGNAL(clicked(bool)), this, SLOT(enableIdleDetect(bool)));

	connect(&idleTimer, SIGNAL(timeout()), this, SLOT(checkIdle()));

	autosave_timer.setInterval(1000 * 60 * 10); // every ten mins
	connect(&autosave_timer, SIGNAL(timeout()), this, SLOT(save_world()));

	petri_dish.set_widget(worldView);
	petri_dish.init(settings->getParams());
	totalEnergy->setText(QString().setNum(petri_dish.get_total_energy()));
	spinDNAVal->setMaximum(settings->getParams().max_data);
	set_stats();

	last_time = 0;
	update_time.start();
	updates_per_sec = 0;

	//update_thread.start();
	timer.start(0);

	setAttribute(Qt::WA_QuitOnClose, true);

	repaint();
}

MainWindow::~MainWindow() {
	QApplication::instance()->quit();
}

void MainWindow::updt() {
		petri_dish.update_all_pixels();
	set_stats();
	update();
}

void MainWindow::set_stats() {
	//if(petri_dish.get_time() % 100) return;
	if(update_time.elapsed() < 500) return;

	int t = petri_dish.get_time();
	float rate = (t - last_time) / (float)update_time.elapsed() * 1000;
	last_time = t;
	update_time.restart();
		updates_per_sec = qMax(0.0f, 4 * updates_per_sec / 5 + rate / 5);
	QString updates = QString().setNum(updates_per_sec, 'f', 2);
	updatesPerSec->setText(updates);

	int totalEnergy = petri_dish.get_total_energy();
	bugEnergy->setText(QString().setNum(petri_dish.get_bug_energy()));
	energyEnergy->setText(QString().setNum(petri_dish.get_energy_energy()));

	int remaining = totalEnergy - petri_dish.get_bug_energy() - petri_dish.get_energy_energy();
	if(remaining < 0) remaining = 0;
	remainingEnergy->setText(QString().setNum(remaining));

	time->setText(QString().setNum(petri_dish.get_time()));
	QString pop = QString().setNum(petri_dish.get_population()),
		gen = QString().setNum(petri_dish.get_max_gen());
	population->setText(pop);
	generation->setText(gen);
	setWindowTitle("QtBugs - " + gen + " (UPS: " + updates + ")");
	//setWindowTitle("QtBugs - " + pop + " (UPS: " + updates + ")");
}

bool MainWindow::isRunning() {
	return timer.isActive();
	//return update_thread.isRunning();
}

void MainWindow::start_stop() {
	if(isRunning()) stop();
	else start();
	last_mouse = QCursor::pos();
}

void MainWindow::start() {
	if(!isRunning()) {
		runButton->setText("Stop");
		timer.start(0);
		//update_thread.start();
		changed = true;
	}
}

void MainWindow::stop() {
	if(isRunning()) {
		timer.stop();
		//update_thread.finish();
		//update_thread.wait();
		runButton->setText("Start");
	}
}

void MainWindow::step() {
	if(isRunning())
		start_stop();
	petri_dish.step();
	changed = true;
}

void MainWindow::check_integrity() {
	//if(isRunning())
	//	start_stop();
	petri_dish.check_integrity();
}

void MainWindow::new_world() {
	if(changed) {
		int ret = QMessageBox::warning(this, tr("Bugs"), tr("Save changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
		if(ret == QMessageBox::Cancel) return;
		else if(ret == QMessageBox::Save) save_world();
		else if(ret != QMessageBox::Discard) return; // in case of pressing escape
	 }

	bool was_running = false;
	if(isRunning()) {
		start_stop();
		was_running = true;
	}

	WorldParams newParams = settings->getParams();

	petri_dish.cleanup();
	petri_dish.init(newParams);
		petri_dish.balance();
		petri_dish.update_all_pixels();
		if(histogramDialog) histogramDialog->newData(petri_dish.get_dna_data(), petri_dish.get_population());

		totalEnergy->setText(QString().setNum(petri_dish.get_total_energy()));
	spinDNAVal->setMaximum(newParams.max_data);
	set_stats();
	changed = false;

	last_time = 0;
	update_time.restart();
	if(was_running) {
		start_stop();
	} else
		update();

		filename = "";
}

void MainWindow::load_world() {
	if(changed) {
		int ret = QMessageBox::warning(this, tr("Bugs"), tr("Save changes?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
		if(ret == QMessageBox::Cancel) return;
		else if(ret == QMessageBox::Save) save_world();
		else if(ret != QMessageBox::Discard) return; // in case of pressing escape
	 }

	bool running = false;
	if(isRunning()) {
		stop();
		running = true;
	}

	QString fn = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Bug data (*.bug)"));
	if(fn.isEmpty()) return;

	QFile file(fn);
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);   // we will serialize the data into the file
	in.setVersion(QDataStream::Qt_4_2);

	int ver;
	in >> ver;
	if(ver >= 2) {
		actionAutoSave->setEnabled(true);
		in >> auto_save;

		WorldParams params;
		in >> params;
		petri_dish.cleanup();
		settings->setParams(params);
		petri_dish.init(params);

		totalEnergy->setText(QString().setNum(petri_dish.get_total_energy()));
		spinDNAVal->setMaximum(params.max_data);
		in >> petri_dish;

				if(histogramDialog) histogramDialog->newData(petri_dish.get_dna_data(), petri_dish.get_population());

				last_time = petri_dish.get_time();
		update_time.restart();
		changed  = false;

		//if successfull
		filename = fn;

        actionAutoSave->setChecked(auto_save);
                autosave_world();

		if(ver >= 3) {
			bool detectIdle;
			in >> detectIdle;
			in >> running;
			idleChk->setChecked(detectIdle);
			enableIdleDetect(detectIdle);
		}
	} else {
		QMessageBox::warning(this, "Load Failed", "Sorry, could not load that file.");
	}

	if(running) start();
	else updt();
}

void MainWindow::save_world() {
	bool running = false;
	if(isRunning()) {
		start_stop();
		running = true;
	}

	QString fn;
	if(filename.isEmpty()) {
		fn = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Bug data (*.bug)"));
		if(fn.isEmpty()) return;
	} else
		fn = filename;

	QFile file(fn);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);   // we will serialize the data into the file
	out.setVersion(QDataStream::Qt_4_2);

	if(write_to_stream (out)) {
		filename = fn;
		changed = false;
		actionAutoSave->setEnabled(true);
	}
	if(running) start_stop();
	else update();
}

void MainWindow::save_as_world() {
	bool running = false;
	if(isRunning()) {
		start_stop();
		running = true;
	}

	QString fn = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Bug data (*.bug)"));
	if(fn.isEmpty()) return;

	QFile file(fn);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);   // we will serialize the data into the file
	out.setVersion(QDataStream::Qt_4_2);

	if(write_to_stream(out)) {
		filename = fn;
		changed = false;
		actionAutoSave->setEnabled(true);
	}
	if(running) start_stop();
	else update();
}

void MainWindow::autosave_world() {
	if(actionAutoSave->isChecked()) {
		auto_save = true;
		autosave_timer.start();
		save_world();
				qDebug() << "autosave timer activated";
	} else {
		auto_save = false;
		autosave_timer.stop();
	}
}

void MainWindow::change_world() {
	qDebug()<< "change_world called";
	WorldParams newParams = settings->getParams();
	petri_dish.set_world_params(newParams);
	totalEnergy->setText(QString().setNum(petri_dish.get_total_energy()));
	spinDNAVal->setMaximum(newParams.max_data);
}

bool MainWindow::write_to_stream(QDataStream &out) {
	out << (int)3;
	out << auto_save;
	out << petri_dish.get_world_params();
	out << petri_dish;
	out << idleChk->isChecked();
	out << isRunning();
	return true;
}

void MainWindow::edit_bug(int x, int y) {
	Bug *b = petri_dish.get_closest_bug(x, y);
	if(b) {
		stop();
		BugEditDialog *d = new BugEditDialog(b, this);
		connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), d, SLOT(close()));
		d->show();
	}
}

void MainWindow::on_spinDNAVal_valueChanged(int byte) {
	petri_dish.set_show_byte(byte);
	if(histogramDialog) histogramDialog->geneSelected(byte);
}

void MainWindow::on_btnHist_clicked() {
	//stop();
	btnHist->setEnabled(false);
	if(!histogramDialog) {
		histogramDialog = new HistogramDialog(settings->getParams(), this);
		connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), histogramDialog, SLOT(close()));
		connect(&petri_dish, SIGNAL(dnaData(const QVector<QByteArray> &, int)), histogramDialog, SLOT(newData(const QVector<QByteArray> &, int)));
		petri_dish.enable_histogram_calc(true);
				histogramDialog->newData(petri_dish.get_dna_data(), petri_dish.get_population());
		histogramDialog->show();
		connect(histogramDialog, SIGNAL(finished(int)), this, SLOT(histogramClosed()));
		connect(histogramDialog, SIGNAL(selectGene(int)), this->spinDNAVal, SLOT(setValue(int)));
	}
}

void MainWindow::histogramClosed() {
	histogramDialog = 0;
	petri_dish.enable_histogram_calc(false);
	btnHist->setEnabled(true);
}

void MainWindow::enableIdleDetect(bool enable) {
	if(enable) idleTimer.start(1000);
	else idleTimer.stop();
}

void MainWindow::checkIdle() {
	QPoint pos = QCursor::pos();
	QDateTime now = QDateTime::currentDateTime();
	if(pos != last_mouse) {
		stop();
		last_mouse = pos;
		idleTime = now;
	} else {
		if(idleTime.secsTo(now) > 10)
			start();
	}
}
