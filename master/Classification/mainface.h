#ifndef MAINFACE_H
#define MAINFACE_H

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtXML/QtXML>
#include <ActiveQt/QAxObject>
#include <string>

#include "viewstudents.h"

#include "ui_mainface.h"

#define TRIAL 0
#define YEAR 2016
#define MONTH 8
#define DAY 28
#define VERSION "1.2"

class MainFace : public QMainWindow
{
	Q_OBJECT

public:
	MainFace(QWidget *parent = 0);
	~MainFace();

	public slots:
		void on_actionOpen_triggered();
		void on_actionSave_triggered();
		void on_actionView_triggered();
		void on_actionClass_triggered();

		void on_listView_clicked( const QModelIndex & index );

private:
	bool readConfig();
	void initListView();
	void initHeader();
	void writeExcel(int index);
	void showClassStudent(int nRow);
	void setEnabled(bool enable);

private:
	Ui::MainFace ui;

	QStandardItemModel _classesModel;
	QStandardItemModel _studentModel;

	int _classesNumber;   // 多少个班级的学生信息，会从配置文件读取
	StudentList *_studentLists;
	int _classNO; // 当前班级的序号
	QList<QString> _classes;   // 字符串，例如123456班
	StudentList _curStudentList;  // 当前从excel读取的学生信息
	QString _fileExt;

	QIcon _iconClass;
	QIcon _iconBoy;
	QIcon _iconGirl;
};

#endif // MAINFACE_H
