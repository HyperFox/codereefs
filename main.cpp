#include "mainwindow.h"

int main(qint32 argc, char *argv[]) {
    QApplication code_reefs(argc, argv);
    MainWindow w(argc,argv,0);
    //w.setWindowFlags(Qt::FramelessWindowHint);
    w.setWindowState(Qt::WindowMaximized);
    w.setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);
    w.setCorner(Qt::BottomRightCorner,Qt::RightDockWidgetArea);
    w.show();
    w.openFiles(w.argv);

    return code_reefs.exec();
}
