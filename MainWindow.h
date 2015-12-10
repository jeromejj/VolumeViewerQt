#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "VolViewer.h"
#include "checkableAction.h"
#include <QAction>
#include <QtGui>
#include <QToolBar>
#include <QtWidgets>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	void createActions();
	void createToolbar();
	void dragEnterEvent(QDragEnterEvent * e);
	void dropEvent(QDropEvent * e);

private:

	VolViewer * viewer;

	QAction * openAction;
	QAction * saveAction;

	checkableAction * lightControl;
	checkableAction * rotationControl;

	QAction * viewPoints;
	QAction * viewWireframe;
	QAction * viewFlatlines;
	QAction * viewFlat;
	QAction * viewTexture;
	QAction * viewTextureModulate;
	QAction * viewBoundary;
	QAction * viewVector;

	QToolBar * fileToolbar;
	QToolBar * viewToolbar;
	QToolBar * editToolbar;
	QActionGroup * drawModeGroup;

	checkableAction * selectAction;
	checkableAction * selectCutFaces;
	QAction * cutAction;
	QAction * clearSelected;

	QAction * xCut;
	QAction * yCut;
	QAction * zCut;
	QActionGroup * cutGroup;
	
	QAction * plusMove;
	QAction * minusMove;

private slots:

	void showPoints();

	void showWireframe();

	void showFlatlines();

	void showFlat();

	void showTexture();

	void showTextureModulate();

	void showBoundary();

	void showVector();
};

#endif // MAINWINDOW_H
