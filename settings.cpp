#include "settings.h"
#include <QIntValidator>
#include <QDoubleValidator>
#include <QMessageBox>
#include <QDebug>

Settings::Settings(QWidget *parent): QWidget(parent) {
	ui.setupUi(this);

	ui.maxDataEd->setValidator(new QIntValidator(this));
	ui.minDataEd->setValidator(new QIntValidator(this));
	ui.maxAgeEd->setValidator(new QIntValidator(this));
	ui.minBugEnergyEd->setValidator(new QIntValidator(this));
	ui.maxBugEnergyEd->setValidator(new QIntValidator(this));
	ui.childEnergyEd->setValidator(new QIntValidator(this));
	ui.moveEnergyEd->setValidator(new QIntValidator(this));
	ui.stayEnergyEd->setValidator(new QIntValidator(this));
	ui.minNodeEnergyEd->setValidator(new QIntValidator(this));
	ui.maxNodeEnergyEd->setValidator(new QIntValidator(this));
	ui.totalEnergyEd->setValidator(new QIntValidator(this));
	ui.minTotalBugEd->setValidator(new QIntValidator(this));
	ui.minTotalNodeEd->setValidator(new QIntValidator(this));
	ui.widthEd->setValidator(new QIntValidator(this));
	ui.heightEd->setValidator(new QIntValidator(this));
	ui.stepsEd->setValidator(new QIntValidator(this));
	ui.stackSizeEd->setValidator(new QIntValidator(this));
	ui.mutationEd->setValidator(new QDoubleValidator(this));
	
	ui.energyLayoutCmb->addItem("Random", QVariant(EL_RANDOM));
	ui.energyLayoutCmb->addItem("Dot", QVariant(EL_DOT));
	ui.energyLayoutCmb->addItem("Two dots", QVariant(EL_TWODOTS));
	ui.energyLayoutCmb->addItem("Ring", QVariant(EL_RING));
	ui.energyLayoutCmb->addItem("Rectangle", QVariant(EL_RECT));
	ui.energyLayoutCmb->addItem("Many dots", QVariant(EL_MANYDOTS));
	ui.energyLayoutCmb->addItem("Change (Dot / Two dots)", QVariant(EL_CHANGE));

	connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton *)), this, SLOT(apply()));
	
//	if(!allowResize) {
		ui.widthEd->setEnabled(false);
		ui.heightEd->setEnabled(false);
//	}
	setParams(wp);
}

void Settings::setParams(const WorldParams &params) {
	ui.maxDataEd->setText(QString().setNum(params.max_data));
	ui.minDataEd->setText(QString().setNum(params.min_data));
	ui.maxAgeEd->setText(QString().setNum(params.max_age));
	ui.minBugEnergyEd->setText(QString().setNum(params.min_energy));
	ui.maxBugEnergyEd->setText(QString().setNum(params.max_energy));
	ui.childEnergyEd->setText(QString().setNum(params.child_energy));
	ui.moveEnergyEd->setText(QString().setNum(params.move_energy));
	ui.stayEnergyEd->setText(QString().setNum(params.stay_energy));
	ui.minNodeEnergyEd->setText(QString().setNum(params.min_e_energy));
	ui.maxNodeEnergyEd->setText(QString().setNum(params.max_e_energy));
	ui.totalEnergyEd->setText(QString().setNum(params.total_energy));
	ui.minTotalBugEd->setText(QString().setNum(params.min_bug_energy));
	ui.minTotalNodeEd->setText(QString().setNum(params.min_energy_energy));
	ui.widthEd->setText(QString().setNum(params.pd_width));
	ui.heightEd->setText(QString().setNum(params.pd_height));
	ui.stepsEd->setText(QString().setNum(params.steps_per_update));
	ui.stackSizeEd->setText(QString().setNum(params.stack_size));
	ui.mutationEd->setText(QString().setNum(params.mutation));
	
	ui.energyLayoutCmb->setCurrentIndex((int)params.energy_layout);

	ui.chkKillOnEdge->setChecked(params.kill_on_edge);
}

WorldParams Settings::getParams() {
	return wp;
}

void Settings::apply() {
	WorldParams newParams;
	bool ok = true;
	if(ok) newParams.max_data = ui.maxDataEd->text().toInt(&ok);
	if(ok) newParams.min_data = ui.minDataEd->text().toInt(&ok);
	if(ok) newParams.max_age = ui.maxAgeEd->text().toInt(&ok);
	if(ok) newParams.min_energy = ui.minBugEnergyEd->text().toInt(&ok);
	if(ok) newParams.max_energy = ui.maxBugEnergyEd->text().toInt(&ok);
	if(ok) newParams.child_energy = ui.childEnergyEd->text().toInt(&ok);
	if(ok) newParams.move_energy = ui.moveEnergyEd->text().toInt(&ok);
	if(ok) newParams.stay_energy = ui.stayEnergyEd->text().toInt(&ok);
	if(ok) newParams.min_e_energy = ui.minNodeEnergyEd->text().toInt(&ok);
	if(ok) newParams.max_e_energy = ui.maxNodeEnergyEd->text().toInt(&ok);
	if(ok) newParams.total_energy = ui.totalEnergyEd->text().toInt(&ok);
	if(ok) newParams.min_bug_energy = ui.minTotalBugEd->text().toInt(&ok);
	if(ok) newParams.min_energy_energy = ui.minTotalNodeEd->text().toInt(&ok);
	if(ok) newParams.pd_width = ui.widthEd->text().toInt(&ok);
	if(ok) newParams.pd_height = ui.heightEd->text().toInt(&ok);
	if(ok) newParams.steps_per_update = ui.stepsEd->text().toInt(&ok);
	if(ok) newParams.stack_size = ui.stackSizeEd->text().toInt(&ok);
	if(ok) newParams.mutation = ui.mutationEd->text().toFloat(&ok);
	if(ok) newParams.kill_on_edge = ui.chkKillOnEdge->isChecked();
	
	newParams.energy_layout = (EnergyLayout)ui.energyLayoutCmb->currentIndex();
	
	if(ok) {
		wp = newParams;
		emit settingsChanged();
		qDebug() << "settingsChanged signal sent";
	} else {
		QMessageBox::warning(this, "Invalid parameter", "There is an error in one of the input parameters.");
	}

}
