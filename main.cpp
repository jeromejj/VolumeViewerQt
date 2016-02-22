#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication VolumeViewer(argc, argv);
    MainWindow mainWin;
	mainWin.setGeometry(100, 100, mainWin.sizeHint().width(), mainWin.sizeHint().height());
	mainWin.resize(mainWin.sizeHint());
    mainWin.showMaximized();

    return VolumeViewer.exec();
}
