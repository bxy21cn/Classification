#include "classification.h"

Classification::Classification(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	ui.lblTrial->setText(tr("Official ") + VERSION);

#if TRIAL
	QDate qDate = QDate::currentDate();
	if (qDate.year() == YEAR && qDate.month() == MONTH && qDate.day() <= DAY)
	{
		setEnabled(true);
		ui.lblTrial->setText(tr("Trial ") + VERSION);
	}
	else
	{
		setEnabled(false);
		ui.lblTrial->setText(tr("Expired"));
		ui.pbtnEnter->setEnabled(false);
		return;
	}
#endif	
}

Classification::~Classification()
{

}

void Classification::on_pbtnEnter_clicked()
{
	this->hide();
	MainFace * mainFace = new MainFace;
	mainFace->show();
}