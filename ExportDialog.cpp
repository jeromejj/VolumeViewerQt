#include "ExportDialog.h"


ExportDialog::ExportDialog(QWidget * _parent) : QDialog(_parent)
{
	QVBoxLayout * layout = new QVBoxLayout();
	layout->addWidget(createGroupBox());
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
	int r = 0;
	if (whole->isChecked())
	{
		r = 1;
	}
	else if (cutBelow->isChecked())
	{
		r = 2;
	}
	else if (cutAbove->isChecked())
	{
		r = 3;
	}

	if (withGroup->isChecked())
	{
		r = r << 1;
		r = r | 1;
	}
	else if (withoutGroup->isChecked())
	{
		r = r << 1;
		r = r | 0;
	}

	done(r);
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

QGroupBox * ExportDialog::createGroupBox()
{
	groupBox = new QGroupBox(tr("Exporting Group Options"));
	withGroup = new QRadioButton(tr("Export with Group Attribute"));
	withoutGroup = new QRadioButton(tr("Export without Group Attribute"));
	withoutGroup->setChecked(true);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addWidget(withoutGroup);
	vbox->addWidget(withGroup);
	vbox->addStretch(1);

	groupBox->setLayout(vbox);

	return groupBox;
};