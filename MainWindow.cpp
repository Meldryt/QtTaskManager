#include "MainWindow.h"
#include "./ui_MainWindow.h"

//#include <QtGui/QOpenGLFunctions>
//#include <QtGui/QOffscreenSurface>
//#include <QtGui/QOpenGLContext>
//#include <QtGui/QOpenGLFunctions>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    systemManager = new SystemManager(this);
    setCentralWidget(systemManager);
}

MainWindow::~MainWindow()
{
    delete ui;
}
