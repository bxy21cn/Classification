#ifndef VIEWSTUDENTS_H
#define VIEWSTUDENTS_H

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include "ui_viewstudents.h"

struct StudentInfo
{
	QString name;
	QString sex;
	QString district;
};

typedef QList<StudentInfo> StudentList;

class ViewStudents : public QDialog
{
	Q_OBJECT

public:
	ViewStudents(QWidget *parent, const StudentList &curStudentList);
	~ViewStudents();

	void showClassStudent();
	void initHeader();

private:
	Ui::ViewStudents ui;

	QStandardItemModel _studentModel;
	StudentList _curStudentList;

	QIcon _iconBoy;
	QIcon _iconGirl;
};

#endif // VIEWSTUDENTS_H
