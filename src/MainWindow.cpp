#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QGridLayout>

#include "GlGlobals.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QOffscreenSurface surface;
    surface.create();

    QOpenGLContext context;
    context.create();
    context.makeCurrent(&surface);

    QOpenGLFunctions* glFunctions = context.functions();

    const std::string glVendor = reinterpret_cast<const char*>(glFunctions->glGetString(GL_VENDOR));
    const std::string glRenderer = reinterpret_cast<const char*>(glFunctions->glGetString(GL_RENDERER));
    const std::string glVersion = reinterpret_cast<const char*>(glFunctions->glGetString(GL_VERSION));
    const std::string glslVersion = reinterpret_cast<const char*>(glFunctions->glGetString(GL_SHADING_LANGUAGE_VERSION));

    GlGlobals::glVendor = glVendor;
    GlGlobals::glRenderer = glRenderer;
    GlGlobals::glVersion = glVersion;
    GlGlobals::glslVersion = glslVersion;

    qDebug() << "GL_VENDOR: " << QLatin1String(glVendor);
    qDebug() << "GL_RENDERER: " << QLatin1String(glVendor);
    qDebug() << "GL_VERSION: " << QLatin1String(glVersion);
    qDebug() << "GL_SHADING_LANGUAGE_VERSION: " << QLatin1String(glslVersion);

    systemManager = new SystemManager(this);
    setCentralWidget(systemManager);
}

MainWindow::~MainWindow()
{
    delete ui;
}
