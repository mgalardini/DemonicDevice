#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    qsrand(time(NULL));

    QApplication a(argc, argv);
    MainWindow w;

    if(!w.resInit())
        return -1;
    w.initObjects();

    // Show fullscreen
    w.showMaximized();

    return a.exec();
}
