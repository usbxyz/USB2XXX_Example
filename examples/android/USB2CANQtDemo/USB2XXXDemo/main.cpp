#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.connect(&w,SIGNAL(exitApp()),&a,SLOT(quit()));
    return a.exec();
}
