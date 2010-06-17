#include "processorpage.h"
#include <QDebug>

ProcessorPage::ProcessorPage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

ProcessorPage::~ProcessorPage()
{

}

void ProcessorPage::setProcessor(Processor *proc) {
	processor = proc;
	updateInfo();
}

void ProcessorPage::updateInfo() {
	qDebug() << "updateInfo" << processor->ip;

	ui.ipEd->setText(QString().setNum(processor->ip));
	ui.accEd->setText(QString().setNum(processor->acc));
	ui.spEd->setText(QString().setNum(processor->sp));
	ui.bpEd->setText(QString().setNum(processor->bp));

	QString str("%1: %2");
	ui.inList->clear();
	for(int i = 0; i < INPUTS; i++) {
		ui.inList->addItem(str.arg(i).arg((int)(unsigned char)processor->in[i]));
	}
	ui.outList->clear();
	for(int i = 0; i < OUTPUTS; i++) {
		ui.outList->addItem(str.arg(i).arg((int)(unsigned char)processor->out[i]));
	}
	ui.stackList->clear();
	for(int i = 0; i < processor->stack.size(); i++) {
		ui.stackList->addItem(str.arg(i).arg((int)(unsigned char)processor->stack[i]));
	}

	if(ui.disIPChk->isChecked()) {
		ui.disStartEd->setText(QString().setNum(processor->ip));
		ui.disEd->setText(processor->disassemble_from(processor->ip));
	}
}


void ProcessorPage::on_spEd_editingFinished()
{
	bool ok = false;
	int sp = ui.spEd->text().toInt(&ok);
	if(ok) processor->sp = sp % processor->stack.size();
}

void ProcessorPage::on_stepBtn_clicked()
{
	processor->step();
	if(isVisible())
		updateInfo();
}

void ProcessorPage::on_disStartEd_editingFinished() {
	//qDebug() << "disStartEd editingFinished";

	bool ok = false;
	int start = ui.disStartEd->text().toInt(&ok);
	if(ok) {
		ui.disEd->setText(processor->disassemble_from(start));
	} else {
		ui.disEd->setText("Invalid address");
	}
}

void ProcessorPage::on_ipEd_returnPressed() {
	//qDebug() << "ipEd returnPressed";
}

void ProcessorPage::on_ipEd_editingFinished() {
	//qDebug() << "ipEd editingFinished";
	bool ok = false;
	int start = ui.ipEd->text().toInt(&ok);
	if(ok) {
		processor->ip = start % processor->ram.size();
		if(ui.disIPChk->isChecked()) {
			ui.disStartEd->setText(QString().setNum(processor->ip));
			ui.disEd->setText(processor->disassemble_from(processor->ip));
		}
	}
}
