//#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )
#include <QApplication>
//#include <QPushButton>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();
    return QApplication::exec();
}
