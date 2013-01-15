#ifndef PROCESSORPAGE_H
#define PROCESSORPAGE_H

#include <QWidget>
#include "ui_processorpage.h"
#include "processor.h"

class ProcessorPage : public QWidget
{
	Q_OBJECT

public:
	ProcessorPage(QWidget *parent = 0);
	~ProcessorPage();

	void setProcessor(Processor *proc);
public slots:
	void updateInfo();

private:
	Ui::ProcessorPageClass ui;
	Processor *processor;

private slots:
	void on_ipEd_editingFinished();
	void on_ipEd_returnPressed();
	void on_disStartEd_editingFinished();
	void on_stepBtn_clicked();
	void on_spEd_editingFinished();
};

#endif // PROCESSORPAGE_H
