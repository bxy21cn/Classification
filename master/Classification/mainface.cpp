#include "mainface.h"

MainFace::MainFace(QWidget *parent)
: QMainWindow(parent), _classNO(0),_classesNumber(0),_studentLists(NULL)
{
	ui.setupUi(this);

	QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
	animation->setDuration(500);
	animation->setStartValue(0);
	animation->setEndValue(1);
	animation->start();

	if (!readConfig())
	{
		QMessageBox::information(this, tr("Config"), tr("Open config.xml failed!"));
		setEnabled(false);
		return;
	}

	if (_classesNumber < 3 || _classesNumber > 20)
	{
		QMessageBox::information(this, tr("Config"), tr("Classes range : [3, 20]!"));
		setEnabled(false);
		return;
	}

	setEnabled(true);

	#if TRIAL
	QDate qDate = QDate::currentDate();
	if (qDate.year() == YEAR && qDate.month() == MONTH && qDate.day() <= DAY)
	{
		setEnabled(true);
	}
	else
	{
		setEnabled(false);
		return;
	}
	#endif	

	QDir dir;
	dir.mkdir("./input");
	dir.mkdir("./output");

	_iconClass = QIcon(":/Classification/Resources/class.png");
	_iconBoy = QIcon(":/Classification/Resources/boy.png");
	_iconGirl = QIcon(":/Classification/Resources/girl.png");

	QString classNum[20] = {"1", "2", 
		"3", "4", "5", "6", "7", "8", "9", "10", "11", "12", 
		"13", "14", "15", "16", "17", "18", "19", "20"};

	for (int i = 0; i < _classesNumber; ++i)
	{
		QString className;
		className.append(classNum[i]);
		className.append(tr("class"));
		_classes.append(className);
	}

	_studentLists = new StudentList[_classesNumber];

	ui.tableView->setModel(&_studentModel);

	initListView();
	initHeader();
	
	ui.listView->setEnabled(false);
	ui.actionSave->setEnabled(false);
	ui.actionView->setEnabled(false);
	ui.actionClass->setEnabled(false);
}

MainFace::~MainFace()
{

}

void MainFace::on_actionOpen_triggered()
{
	ui.statusBar->showMessage(tr("Select an Excel file"));
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",	 "Excel (*.xlsx *.xls)");

	if (fileName.isEmpty())
	{
		ui.statusBar->clearMessage();
		return;
	}

	QFileInfo fileinfo = QFileInfo(fileName);
	_fileExt = fileinfo.suffix();

	ui.statusBar->showMessage(tr("Opening Excel..."));

	QAxObject *excel = NULL;
	QAxObject *workbooks = NULL;
	QAxObject *workbook = NULL;

	excel = new QAxObject("Excel.Application");
	if (!excel)
	{
		QMessageBox::critical(this, tr("Error"), tr("Excel is not installed!"));
		ui.statusBar->clearMessage();
		return;
	}

	excel->setProperty("DisplayAlerts", false);
	excel->dynamicCall("SetVisible(bool)", false);
	workbooks = excel->querySubObject("WorkBooks");
	if (!workbooks)
	{
		QMessageBox::critical(this, tr("Error"), tr("Excel is not installed!"));
		ui.statusBar->clearMessage();
		return;
	}

	workbook = workbooks->querySubObject("Open(QString, QVariant)", fileName);
	if (!workbook)
	{
		QMessageBox::critical(this, tr("Error"), tr("Excel is not installed!"));
		ui.statusBar->clearMessage();
		return;
	}

	QAxObject * worksheet = workbook->querySubObject("WorkSheets(int)", 1);

	QAxObject * usedrange = worksheet->querySubObject("UsedRange");
	if (!usedrange)
	{
		QMessageBox::critical(this, tr("Error"), tr("Excel is not installed!"));
		ui.statusBar->clearMessage();
		return;
	}

	QAxObject * rows = usedrange->querySubObject("Rows");
	if (!rows)
	{
		QMessageBox::critical(this, tr("Error"), tr("Excel is not installed!"));
		ui.statusBar->clearMessage();
		return;
	}
	QAxObject * columns = usedrange->querySubObject("Columns");
	if (!columns)
	{
		QMessageBox::critical(this, tr("Error"), tr("Excel is not installed!"));
		ui.statusBar->clearMessage();
		return;
	}

	ui.statusBar->showMessage(tr("Reading students information..."));
	int intRowStart = usedrange->property("Row").toInt();
	int intColStart = usedrange->property("Column").toInt();
	int intCols = columns->property("Count").toInt();
	int intRows = rows->property("Count").toInt();

	for (int i = intRowStart; i < intRowStart + intRows; i++)
	{
		StudentInfo studentInfo;
		for (int j = intColStart; j < intColStart + intCols; j++)
		{
			QAxObject * cell = worksheet->querySubObject("Cells(int,int)", i, j );
			if (!cell)
			{
				QMessageBox::critical(this, tr("Error"), tr("Excel is not installed!"));
				ui.statusBar->clearMessage();
				return;
			}
			if (j == 1)
			{
				studentInfo.name = cell->dynamicCall("Value2()").toString();
			}

			if (j == 2)
			{
				studentInfo.sex = cell->dynamicCall("Value2()").toString();
			}

			if (j == 3)
			{
				studentInfo.district = cell->dynamicCall("Value2()").toString();
			}			
		}
		_curStudentList.append(studentInfo);
	}

	workbook->dynamicCall("Close()", false);
	excel->dynamicCall("Quit()");
	delete excel;
	excel = NULL;

	QString msg = tr("Reading students information finished£¡Student total : ");
	msg.append(QString::number(_curStudentList.size()));

	ui.statusBar->showMessage(msg, 5000);
	ui.actionOpen->setEnabled(false);
	ui.actionView->setEnabled(true);
	ui.actionClass->setEnabled(true);
}

void MainFace::on_actionSave_triggered()
{
	for (int i = 0; i < _classesNumber; ++i)
	{
		ui.statusBar->showMessage(tr("Saving") + _classes[i]);
		writeExcel(i);
	}

	ui.statusBar->showMessage(tr("Saving finished!"), 3000);

	ui.actionSave->setEnabled(false);
}

void MainFace::on_actionView_triggered()
{
	ViewStudents viewStudents(this, _curStudentList);
	viewStudents.exec();
}

void MainFace::on_actionClass_triggered()
{
	if (_curStudentList.size() == 0)
	{
		QMessageBox::information(this, tr("Select"), tr("Please select a correct Excel"));
		return;
	}

	ui.statusBar->showMessage(tr("Random classification..."));

	// classify
	int districtNum = 0; 
	QStringList districtList;
	for (int i = 0; i < _curStudentList.size(); ++i)
	{
		if (districtList.indexOf(_curStudentList[i].district) == -1)
		{
			districtNum++;
			districtList.append(_curStudentList[i].district);
		}
		else
		{
			continue;
		}
	}

	qSort(districtList.begin(), districtList.end());

	StudentList *districtStuLists = new StudentList[3];  // boy girl  bad
	for (int i = 0; i < _curStudentList.size(); ++i)
	{
		int index = districtList.indexOf(_curStudentList[i].district);

		if (index == (districtList.size() - 1))
		{
			districtStuLists[2].append(_curStudentList[i]);
			continue;
		}

		QString sex = _curStudentList[i].sex;
		if ( !sex.compare(QString::fromStdWString(L"ÄÐ")) )
		{
			districtStuLists[0].append(_curStudentList[i]);
		} 
		else
		{
			districtStuLists[1].append(_curStudentList[i]);
		}
	}

	for (int i = 0; i < 3; ++i)
	{
		while( districtStuLists[i].size() != 0 )
		{
			QTime time;
			time= QTime::currentTime();
			qsrand(time.msec()+time.second()*1000);

			int NO = qrand()%districtStuLists[i].size();

			StudentInfo studentInfo;
			studentInfo = districtStuLists[i][NO];
			districtStuLists[i].removeAt(NO);

			_studentLists[_classNO].append(studentInfo);

			_classNO++;
			if (_classNO == _classesNumber)
			{
				_classNO = 0;
			}
		}
	}

	delete [] districtStuLists;
	districtStuLists = NULL;

	// show class 1
	_studentModel.clear();
	initHeader();
	showClassStudent(0);

	ui.listView->setEnabled(true);
	ui.actionClass->setEnabled(false);
	ui.actionSave->setEnabled(true);

	ui.statusBar->showMessage(tr("Random classification finished!"), 3000);
}

void MainFace::on_listView_clicked(const QModelIndex & index)
{
	_studentModel.clear();
	initHeader();
	showClassStudent(index.row());
}

bool MainFace::readConfig()
{
	QString fileName = "./config.xml";

	QFile file(fileName);
	if(!file.open(QFile::ReadOnly | QFile::Text))
	{
		return false;
	}

	QDomDocument document;
	if( !document.setContent(&file) )
	{
		return false;
	}

	if( document.isNull() )
	{
		return false;
	}

	QDomElement root = document.documentElement();
	if (root.isNull())
	{
		return false;
	}

	QDomElement classesElement = root.firstChildElement();
	if( classesElement.isNull() )
		return false;
	else
		_classesNumber = classesElement.text().toInt();

	file.close();
	return true;
}

void MainFace::initListView()
{
	_classesModel.clear();

	for (int i = 0; i < _classesNumber; ++i)
	{
		QStandardItem *item = new QStandardItem(_iconClass, _classes[i]);
		_classesModel.appendRow(item);
	}

	ui.listView->setModel(&_classesModel);
}

void MainFace::initHeader()
{
	_studentModel.setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
	_studentModel.setHorizontalHeaderItem(1, new QStandardItem(tr("Sex")));
	_studentModel.setHorizontalHeaderItem(2, new QStandardItem(tr("District")));

	ui.tableView->setColumnWidth(0,200);
	ui.tableView->setColumnWidth(1,100);
}

void MainFace::writeExcel(int index)
{
	QAxObject *excel = new QAxObject(this);  
	excel->setControl("Excel.Application");
	excel->dynamicCall("SetVisible (bool Visible)","false");
	excel->setProperty("DisplayAlerts", false);

	QAxObject *workbooks = excel->querySubObject("WorkBooks");
	workbooks->dynamicCall("Add");
	QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
	QAxObject *worksheets = workbook->querySubObject("Sheets");
	QAxObject *worksheet = worksheets->querySubObject("Item(int)",1);

	QAxObject *cellA,*cellB,*cellC;

	int size = _studentLists[index].size();
	for(int i=0;i<size;i++)
	{  
		QString A="A"+QString::number(i+1);
		QString B="B"+QString::number(i+1);
		QString C="C"+QString::number(i+1);

		cellA = worksheet->querySubObject("Range(QVariant, QVariant)",A);
		cellB = worksheet->querySubObject("Range(QVariant, QVariant)",B);
		cellC = worksheet->querySubObject("Range(QVariant, QVariant)",C);

		cellA->dynamicCall("SetValue(const QVariant&)",QVariant(_studentLists[index][i].name));
		cellB->dynamicCall("SetValue(const QVariant&)",QVariant(_studentLists[index][i].sex));
		cellC->dynamicCall("SetValue(const QVariant&)",QVariant(_studentLists[index][i].district));
	}  

	QString className = QCoreApplication::applicationDirPath();
	className.append("/output/");
	className.append(_classes[index]);
	className.append(".");
	className.append(_fileExt);

	workbook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(className));
	workbook->dynamicCall("Close()");
	excel->dynamicCall("Quit()");
	delete excel;
	excel=NULL;
}

void MainFace::showClassStudent(int nRow)
{
	int cnt = _studentLists[nRow].size();

	for (int i = 0; i < cnt; ++i)
	{
		QString sex = _studentLists[nRow][i].sex;

		if ( !sex.compare(QString::fromStdWString(L"ÄÐ")) )
		{
			_studentModel.setItem(i, 0, new QStandardItem(_iconBoy, _studentLists[nRow][i].name));
		} 
		else
		{
			_studentModel.setItem(i, 0, new QStandardItem(_iconGirl, _studentLists[nRow][i].name));
		}
		_studentModel.item(i, 0)->setTextAlignment(Qt::AlignCenter);

		_studentModel.setItem(i, 1, new QStandardItem(_studentLists[nRow][i].sex));
		_studentModel.item(i, 1)->setTextAlignment(Qt::AlignCenter);

		_studentModel.setItem(i, 2, new QStandardItem(_studentLists[nRow][i].district));
		_studentModel.item(i, 2)->setTextAlignment(Qt::AlignCenter);
	}

	_studentModel.sort(0);
}

void MainFace::setEnabled(bool enable)
{
	ui.actionOpen->setEnabled(enable);
	ui.actionSave->setEnabled(enable);
	ui.actionView->setEnabled(enable);
	ui.actionClass->setEnabled(enable);
}
