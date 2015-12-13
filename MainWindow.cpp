#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
	viewer = new VolViewer();
	setCentralWidget(viewer);
	createActions();
	createToolbar();
	setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::dragEnterEvent(QDragEnterEvent * e)
{
	if (e->mimeData()->hasUrls())
	{
		e->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent * e)
{
	for each (const QUrl &url in e->mimeData()->urls() )
	{
		const QString & filename = url.toLocalFile();
		std::string sFileName = filename.toStdString();
		std::string fExt;
		if (filename.endsWith(".t"))
		{
			fExt = "t";
		}
		else if (filename.endsWith(".tet"))
		{
			fExt = "tet";
		}
		else if (filename.endsWith(".f"))
		{
			fExt = "f";
		}
		else if (filename.endsWith(".hm"))
		{
			fExt = "hm";
		}
		else
		{
			QMessageBox dragFileFailed;
			QString fExtQ(fExt.c_str());
			QString info = "Illegal File Type " + fExtQ.toUpper();
			dragFileFailed.setText(info);
			dragFileFailed.exec();
			continue;
		}

		QString canonicalFilePath = QFileInfo(filename).canonicalFilePath();
		viewer->loadFromMainWin(sFileName, fExt);
		QString title = "VolViewerQt - " + canonicalFilePath;
		setWindowTitle(title);
	}
}

void MainWindow::createActions()
{
	openAction = new QAction(tr("&Open"), this);
	openAction->setIcon(QIcon(":/icons/images/open.png"));
	openAction->setShortcut(QKeySequence::Open);
	openAction->setStatusTip(tr("Open a mesh file"));
	connect(openAction, SIGNAL(triggered()), viewer, SLOT(openMesh()));

	saveAction = new QAction(tr("&Save"), this);
	saveAction->setIcon(QIcon(":/icons/images/save.png"));
	saveAction->setShortcut(QKeySequence::Save);
	saveAction->setStatusTip(tr("save a mesh file"));
	connect(saveAction, SIGNAL(triggered()), viewer, SLOT(saveMesh()));

	selectAction = new checkableAction(this);
	selectAction->setIcon(QIcon(":/icons/images/select.png"));
	selectAction->setText(tr("Select Vertices"));
	selectAction->setStatusTip(tr("Select Vertices"));
	selectAction->setCheckable(true);
	selectAction->setChecked(false);
	connect(selectAction, SIGNAL(actionCheck()), viewer, SLOT(enterSelectionMode()));
	connect(selectAction, SIGNAL(actionUncheck()), viewer, SLOT(quitSelectionMode()));

	selectCutFaces = new checkableAction(this);
	selectCutFaces->setIcon(QIcon(":/icons/images/selectcutface.png"));
	selectCutFaces->setText(tr("Select Cut Faces"));
	selectCutFaces->setStatusTip(tr("Select Cut Faces"));
	selectCutFaces->setCheckable(true);
	selectCutFaces->setChecked(false);
	connect(selectCutFaces, SIGNAL(actionCheck()), viewer, SLOT(enterSelectionCutFaceMode()));
	connect(selectCutFaces, SIGNAL(actionUncheck()), viewer, SLOT(quitSelectionCutFaceMode()));

	clearSelected = new QAction(tr("&Clear"), this);
	clearSelected->setIcon(QIcon(":/icons/images/clear.png"));
	clearSelected->setStatusTip(tr("Clear Selected Vertices and Faces"));
	connect(clearSelected, SIGNAL(triggered()), viewer, SLOT(clearSelectedVF()));

	cutAction = new QAction(tr("&Cut"), this);
	cutAction->setIcon(QIcon(":/icons/images/cut.png"));
	cutAction->setStatusTip(tr("Cut the Volume along Selected Faces"));
	connect(cutAction, SIGNAL(triggered()), viewer, SLOT(cutVolume()));

	lightControl = new checkableAction(this);
	lightControl->setText(tr("&Light Control"));
	lightControl->setIcon(QIcon(":/icons/images/light.png"));
	lightControl->setCheckable(true);
	lightControl->setChecked(true);
	connect(lightControl, SIGNAL(actionCheck()), viewer, SLOT(lightOn()));
	connect(lightControl, SIGNAL(actionUncheck()), viewer, SLOT(lightOff()));

	rotationControl = new checkableAction(this);
	rotationControl->setText(tr("&Light Control"));
	rotationControl->setIcon(QIcon(":/icons/images/rotation.png"));
	rotationControl->setCheckable(true);
	rotationControl->setChecked(true);
	connect(rotationControl, SIGNAL(actionCheck()), viewer, SLOT(rotationViewOn()));
	connect(rotationControl, SIGNAL(actionUncheck()), viewer, SLOT(rotationViewOff()));

	viewPoints = new QAction(tr("&Points"), this);
	viewPoints->setIcon(QIcon(":/icons/images/points.png"));
	viewPoints->setText(tr("Draw Points"));
	viewPoints->setStatusTip(tr("Points"));
	viewPoints->setCheckable(true);
	viewPoints->setChecked(false);
	connect(viewPoints, SIGNAL(triggered()), this, SLOT(showPoints()));

	viewWireframe = new QAction(tr("&Wireframe"), this);
	viewWireframe->setIcon(QIcon(":/icons/images/wireframe.png"));
	viewWireframe->setText(tr("Draw Wireframe"));
	viewWireframe->setStatusTip(tr("Wireframe"));
	viewWireframe->setCheckable(true);
	viewWireframe->setChecked(false);
	connect(viewWireframe, SIGNAL(triggered()), this, SLOT(showWireframe()));

	viewFlatlines = new QAction(tr("Flat&lines"), this);
	viewFlatlines->setIcon(QIcon(":/icons/images/flatlines.png"));
	viewFlatlines->setText(tr("Draw Flatlines"));
	viewFlatlines->setStatusTip(tr("Flatlines"));
	viewFlatlines->setCheckable(true);
	viewFlatlines->setChecked(false);
	connect(viewFlatlines, SIGNAL(triggered()), this, SLOT(showFlatlines()));

	viewFlat = new QAction(tr("&Flat"), this);
	viewFlat->setIcon(QIcon(":/icons/images/flat.png"));
	viewFlat->setText(tr("Draw Flat"));
	viewFlat->setStatusTip(tr("Flat"));
	viewFlat->setCheckable(true);
	viewFlat->setChecked(false);
	connect(viewFlat, SIGNAL(triggered()), this, SLOT(showFlat()));

	viewBoundary = new QAction(tr("&Boundary"), this);
	viewBoundary->setIcon(QIcon(":/icons/images/boundary.png"));
	viewBoundary->setText(tr("Draw Boundary Faces"));
	viewBoundary->setStatusTip(tr("Boundary"));
	viewBoundary->setCheckable(true);
	viewBoundary->setChecked(false);
	connect(viewBoundary, SIGNAL(triggered()), this, SLOT(showBoundary()));

	viewVector = new QAction(tr("&Vector"), this);
	viewVector->setIcon(QIcon(":/icons/images/vector.png"));
	viewVector->setText(tr("Draw Tetrahedra Vectors"));
	viewVector->setStatusTip(tr("Vector"));
	viewVector->setCheckable(true);
	viewVector->setChecked(false);
	connect(viewVector, SIGNAL(triggered()), this, SLOT(showVector()));

	viewTexture = new QAction(tr("&Texture"), this);
	viewTexture->setIcon(QIcon(":/icons/images/texture.png"));
	viewTexture->setText(tr("Draw Texture"));
	viewTexture->setStatusTip(tr("Texture"));
	viewTexture->setCheckable(true);
	viewTexture->setChecked(false);
	connect(viewTexture, SIGNAL(triggered()), this, SLOT(showTexture()));

	viewTextureModulate = new QAction(tr("Texture&Modulate"), this);
	viewTextureModulate->setIcon(QIcon(":/icons/images/texturemodulate.png"));
	viewTextureModulate->setText(tr("Draw Texture Modulate"));
	viewTextureModulate->setStatusTip(tr("Texture Modulate"));
	viewTextureModulate->setCheckable(true);
	viewTextureModulate->setChecked(false);
	connect(viewTextureModulate, SIGNAL(triggered()), this, SLOT(showTextureModulate()));

	xCut = new QAction(tr("&XCut"), this);
	xCut->setIcon(QIcon(":/icons/images/xcut.png"));
	xCut->setText(tr("Cut Tet Mesh along X"));
	xCut->setStatusTip(tr("Cut Tet Mesh along X"));
	xCut->setCheckable(true);
	xCut->setChecked(false);
	connect(xCut, SIGNAL(triggered()), viewer, SLOT(xCut()));

	yCut = new QAction(tr("&YCut"), this);
	yCut->setIcon(QIcon(":/icons/images/ycut.png"));
	yCut->setText(tr("Cut Tet Mesh along Y"));
	yCut->setStatusTip(tr("Cut Tet Mesh along Y"));
	yCut->setCheckable(true);
	yCut->setChecked(false);
	connect(yCut, SIGNAL(triggered()), viewer, SLOT(yCut()));

	zCut = new QAction(tr("&ZCut"), this);
	zCut->setIcon(QIcon(":/icons/images/zcut.png"));
	zCut->setText(tr("Cut Tet Mesh along Z"));
	zCut->setStatusTip(tr("Cut Tet Mesh along Z"));
	zCut->setCheckable(true);
	zCut->setChecked(false);
	connect(zCut, SIGNAL(triggered()), viewer, SLOT(zCut()));

	plusMove = new QAction(tr("&+Move"), this);
	plusMove->setIcon(QIcon(":/icons/images/plus.png"));
	plusMove->setText(tr("Move the Cut Plane plus 0.5"));
	connect(plusMove, SIGNAL(triggered()), viewer, SLOT(plusMove()));

	minusMove = new QAction(tr("&+Move"), this);
	minusMove->setIcon(QIcon(":/icons/images/minus.png"));
	minusMove->setText(tr("Move the Cut Plane minus 0.5"));
	connect(minusMove, SIGNAL(triggered()), viewer, SLOT(minusMove()));
}

void MainWindow::createToolbar()
{
	fileToolbar = addToolBar(tr("&File"));
	fileToolbar->addAction(openAction);
	fileToolbar->addAction(saveAction);
	
	editToolbar = addToolBar(tr("&Edit"));
	editToolbar->addAction(selectAction);
	editToolbar->addAction(selectCutFaces);
	editToolbar->addAction(cutAction);
	editToolbar->addAction(clearSelected);

	viewToolbar = addToolBar(tr("&View"));
	viewToolbar->addAction(lightControl);
	viewToolbar->addAction(viewPoints);
	viewToolbar->addAction(viewWireframe);
	viewToolbar->addAction(viewFlatlines);
	viewToolbar->addAction(viewFlat);
	viewToolbar->addAction(viewBoundary);
	viewToolbar->addAction(viewTexture);
	viewToolbar->addAction(viewTextureModulate);
	viewToolbar->addAction(viewVector);
	viewToolbar->addAction(rotationControl);
	
	viewToolbar->addAction(xCut);
	viewToolbar->addAction(yCut);
	viewToolbar->addAction(zCut);
	viewToolbar->addAction(plusMove);
	viewToolbar->addAction(minusMove);

	drawModeGroup = new QActionGroup(this);
	
	drawModeGroup->addAction(viewPoints);
	drawModeGroup->addAction(viewWireframe);
	drawModeGroup->addAction(viewFlatlines);
	drawModeGroup->addAction(viewFlat);
	drawModeGroup->addAction(viewBoundary);
	drawModeGroup->addAction(viewTexture);
	drawModeGroup->addAction(viewTextureModulate);
	drawModeGroup->addAction(viewVector);

	cutGroup = new QActionGroup(this);
	cutGroup->addAction(xCut);
	cutGroup->addAction(yCut);
	cutGroup->addAction(zCut);
}

void MainWindow::showPoints()
{
	viewer->setDrawMode(DRAW_MODE::POINTS);
}

void MainWindow::showWireframe()
{
	viewer->setDrawMode(DRAW_MODE::WIREFRAME);
}

void MainWindow::showFlatlines()
{
	viewer->setDrawMode(DRAW_MODE::FLATLINES);
}

void MainWindow::showFlat()
{
	viewer->setDrawMode(DRAW_MODE::FLAT);
}

void MainWindow::showBoundary()
{
	viewer->setDrawMode(DRAW_MODE::BOUNDARY);
}

void MainWindow::showTexture()
{
	viewer->loadTextMesh();
	viewer->setDrawMode(DRAW_MODE::TEXTURE);
}

void MainWindow::showTextureModulate()
{
	viewer->loadTextMesh();
	viewer->setDrawMode(DRAW_MODE::TEXTUREMODULATE);
}

void MainWindow::showVector()
{
	viewer->setDrawMode(DRAW_MODE::VECTOR);
}