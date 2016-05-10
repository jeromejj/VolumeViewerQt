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
	resize(360, 180);
};


ExportDialog::~ExportDialog()
{
};

void ExportDialog::accept()
{
	int r = 0;
	if (whole->isChecked())
	{
		r = 1;
	}
	else if (onlyBelow->isChecked())
	{
		r = 2;
	}
	else if (onlyAbove->isChecked())
	{
		r = 3;
	}
	else if (onlyCut->isChecked()) {
		r = 4;
	}

	done(r);
};

QGroupBox * ExportDialog::createExportGroup()
{
	exportGroup = new QGroupBox(tr("Exporting Options"));
	whole = new QRadioButton(tr("The Whole Surface"));
	onlyBelow = new QRadioButton(tr("The Below Surface"));
	onlyAbove = new QRadioButton(tr("The Above Surface"));
	onlyCut = new QRadioButton(tr("The Cut Surface"));
	whole->setChecked(true);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addWidget(whole);
	vbox->addWidget(onlyBelow);
	vbox->addWidget(onlyAbove);
	vbox->addWidget(onlyCut);
	vbox->addStretch(1);
	exportGroup->setLayout(vbox);

	return exportGroup;
};
