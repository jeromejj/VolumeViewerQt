#include "ExportDialog.h"


ExportDialog::ExportDialog(QWidget * _parent) : QDialog(_parent)
{
	QVBoxLayout * layout = new QVBoxLayout();
	layout->addWidget(createExportGroup());

	QDialogButtonBox * defaultButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(defaultButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(defaultButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

	layout->addWidget(defaultButtonBox);

	setLayout(layout);

	setWindowTitle(tr("Export:"));
	resize(360, 240);
};


ExportDialog::~ExportDialog()
{
};

void ExportDialog::accept()
{
	if (whole->isChecked())
	{
		done(1);
	}
	else if (cutBelow->isChecked())
	{
		done(2);
	}
	else if (cutAbove->isChecked())
	{
		done(3);
	}
};

QGroupBox * ExportDialog::createExportGroup()
{
	exportGroup = new QGroupBox(tr("Exporting Options"));
	whole = new QRadioButton(tr("The Whole Surface"));
	cutBelow = new QRadioButton(tr("The Cut Surface and Below"));
	cutAbove = new QRadioButton(tr("The Cut Surface and Above"));
	whole->setChecked(true);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addWidget(whole);
	vbox->addWidget(cutBelow);
	vbox->addWidget(cutAbove);
	vbox->addStretch(1);
	exportGroup->setLayout(vbox);

	return exportGroup;
};