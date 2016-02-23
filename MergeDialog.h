#pragma once
#include <QDialog>
#include <QLayout>
#include <QGRoupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QDialogButtonBox>

#include <iostream>

class MergeDialog :
	public QDialog
{

	Q_OBJECT

public:
	MergeDialog(QWidget * _parent = 0);
	~MergeDialog();

public slots:
	void accept();

private:
	QGroupBox * createMergeGroup();

	QGroupBox * mergeGroup;
	QRadioButton * average;
	QRadioButton * one2two;
	QRadioButton * two2one;

};

