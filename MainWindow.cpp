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

    //QOpenGLContext context;
    //context.create();

    //QOffscreenSurface surface;
    //surface.create();
    //context.makeCurrent(&surface);

    //QOpenGLFunctions* gl = context.functions();

    //std::string vendor = reinterpret_cast<const char*>(gl->glGetString(GL_VENDOR));
    //std::string renderer = reinterpret_cast<const char*>(gl->glGetString(GL_RENDERER));
    //std::string version = reinterpret_cast<const char*>(gl->glGetString(GL_VERSION));
}

MainWindow::~MainWindow()
{
    delete ui;
}

