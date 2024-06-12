#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // @note: please run project as admin to work properly!

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
