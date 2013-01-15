#include "dnapage.h"
#include <QPainter>

DNAPage::DNAPage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

DNAPage::~DNAPage()
{

}

void DNAPage::setBug(Bug *b) {
	bug = b;
	ui.widget->setDNA(&bug->get_dna());
	updateInfo();
}

void DNAPage::updateInfo() {
	ui.edAge->setText(QString("%1").arg(bug->get_age()));
	ui.edEnergy->setText(QString("%1").arg(bug->get_energy()));
}
