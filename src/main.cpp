#include "MainWindow.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(4, 6);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setAlphaBufferSize(8);
    //format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
    //format.setSwapInterval(1);
    format.setSwapInterval(0);
    QSurfaceFormat::setDefaultFormat(format);

    // @note: please run project as admin to work properly!

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}