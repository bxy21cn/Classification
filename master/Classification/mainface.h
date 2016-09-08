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

	int _classesNumber;   // ���ٸ��༶��ѧ����Ϣ����������ļ���ȡ
	StudentList *_studentLists;
	int _classNO; // ��ǰ�༶�����
	QList<QString> _classes;   // �ַ���������123456��
	StudentList _curStudentList;  // ��ǰ��excel��ȡ��ѧ����Ϣ
	QString _fileExt;

	QIcon _iconClass;
	QIcon _iconBoy;
	QIcon _iconGirl;
};

#endif // MAINFACE_H
