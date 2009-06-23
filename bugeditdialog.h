#ifndef BUGEDITDIALOG_H
#define BUGEDITDIALOG_H

#include <QDialog>
#include "ui_bugeditdialog.h"
#include "bug.h"

class BugEditDialog : public QDialog
{
	Q_OBJECT

public:
	BugEditDialog(Bug *b, QWidget *parent = 0, Qt::WFlags flags = 0);
	~BugEditDialog();

private:
	Ui::BugEditDialogClass ui;
	Bug *bug;
};

#endif // BUGEDITDIALOG_H
