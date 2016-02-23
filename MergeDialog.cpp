#include "MergeDialog.h"


MergeDialog::MergeDialog(QWidget * _parent) : QDialog(_parent)
{
	QVBoxLayout * layout = new QVBoxLayout();
	layout->addWidget(createMergeGroup());

	QDialogButtonBox * defaultButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(defaultButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(defaultButtonBox, SIGNAL(rejected()), this, SLOT(reject()));

	layout->addWidget(defaultButtonBox);

	setLayout(layout);

	setWindowTitle(tr("Select Merge Way:"));
	resize(360, 240);
}

MergeDialog::~MergeDialog()
{
}
void MergeDialog::accept()
{
	if (average->isChecked())
	{
		done(1);
	}
	else if (one2two->isChecked())
	{
		done(2);
	}
	else if (two2one->isChecked())
	{
		done(3);
	}
};

QGroupBox * MergeDialog::createMergeGroup()
{
	mergeGroup = new QGroupBox(tr("Merge Options"));
	average = new QRadioButton(tr("Average of Two Vertices"));
	one2two = new QRadioButton(tr("Move the vertex in Mesh 1 to the vertex in Mesh 2"));
	two2one = new QRadioButton(tr("Move the vertex in Mesh 1 to the vertex in Mesh 2"));
	average->setChecked(true);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addWidget(average);
	vbox->addWidget(one2two);
	vbox->addWidget(two2one);
	vbox->addStretch(1);
	mergeGroup->setLayout(vbox);

	return mergeGroup;
}