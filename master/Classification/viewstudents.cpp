#include "viewstudents.h"

ViewStudents::ViewStudents(QWidget *parent, const StudentList &curStudentList)
: QDialog(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::WindowCloseButtonHint;
	setWindowFlags(flags);

	_curStudentList = curStudentList;

	_iconBoy = QIcon(":/Classification/Resources/boy.png");
	_iconGirl = QIcon(":/Classification/Resources/girl.png");

	ui.tableView->setModel(&_studentModel);

	initHeader();
	showClassStudent();
}

ViewStudents::~ViewStudents()
{

}

void ViewStudents::showClassStudent()
{
	int cnt = _curStudentList.size();
	for (int i = 0; i < cnt; ++i)
	{
		QString sex = _curStudentList[i].sex;

		if ( !sex.compare(QString::fromStdWString(L"ÄÐ")) )
		{
			_studentModel.setItem(i, 0, new QStandardItem(_iconBoy, _curStudentList[i].name));
		} 
		else
		{
			_studentModel.setItem(i, 0, new QStandardItem(_iconGirl, _curStudentList[i].name));
		}
		_studentModel.item(i, 0)->setTextAlignment(Qt::AlignCenter);

		_studentModel.setItem(i, 1, new QStandardItem(_curStudentList[i].sex));
		_studentModel.item(i, 1)->setTextAlignment(Qt::AlignCenter);

		_studentModel.setItem(i, 2, new QStandardItem(_curStudentList[i].district));
		_studentModel.item(i, 2)->setTextAlignment(Qt::AlignCenter);
	}
}

void ViewStudents::initHeader()
{
	_studentModel.setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
	_studentModel.setHorizontalHeaderItem(1, new QStandardItem(tr("Sex")));
	_studentModel.setHorizontalHeaderItem(2, new QStandardItem(tr("District")));

	ui.tableView->setColumnWidth(0,200);
	ui.tableView->setColumnWidth(1,100);
}
