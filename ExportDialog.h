#pragma once
#include <QDialog>
#include <QLayout>
#include <QGRoupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QDialogButtonBox>

#include <iostream>

class ExportDialog :
	public QDialog
{
	Q_OBJECT

public:
	ExportDialog(QWidget * _parent = 0);
	~ExportDialog();

public slots:
	void accept();

private:
	QGroupBox * createExportGroup();
	QGroupBox * createGroupBox();

	QGroupBox * exportGroup;
	QGroupBox * groupBox;
	QRadioButton * withGroup;
	QRadioButton * withoutGroup;
	QRadioButton * whole;
	QRadioButton * cutBelow;
	QRadioButton * cutAbove;
};

