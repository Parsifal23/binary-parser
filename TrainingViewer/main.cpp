#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle("fusion");
    QFile file(":/Style.css");
    if(file.open(QIODevice::ReadOnly)){
      qApp->setStyleSheet(file.readAll());
    }
    MainWindow w;
    w.show();

    return a.exec();
}
