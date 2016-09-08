#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include "mainface.h"
#include "ui_classification.h"

class Classification : public QMainWindow
{
	Q_OBJECT

public:
	Classification(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Classification();

	public slots:
		void on_pbtnEnter_clicked();

private:
	Ui::ClassificationClass ui;
};

#endif // CLASSIFICATION_H
