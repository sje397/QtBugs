#include "bugeditdialog.h"

BugEditDialog::BugEditDialog(Bug *b, QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags), bug(b)
{
	ui.setupUi(this);

	connect(bug, SIGNAL(dying()), this, SLOT(close()));
	connect(bug, SIGNAL(dying()), this, SLOT(deleteLater()));

	ui.procPage->setProcessor(&bug->get_processor());
	ui.dnaPage->setDNA(&bug->get_dna());

	connect(bug, SIGNAL(changed()), ui.procPage, SLOT(updateInfo()));
	connect(bug, SIGNAL(changed()), ui.dnaPage, SLOT(repaint()));

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
}

BugEditDialog::~BugEditDialog()
{

}
