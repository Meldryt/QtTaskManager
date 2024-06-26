#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <QGridLayout>
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

    //m_benchmarkWindow = new QWindow();
    //m_gpuBenchmarkWidget = new GpuBenchmarkWidget();
    //m_gpuBenchmarkWidget->resize(800, 600);
    
    //m_gpuBenchmarkWidget->show();
    //m_gpuBenchmarkWidget->setWindowTitle("OpenGl Benchmark");
    //m_gpuBenchmarkWidget->move(1700, 350);
    //m_gpuBenchmarkWidget->update();
    //QGridLayout* gpuWidgetLayout = new QGridLayout(m_benchmarkWindow);
    //gpuWidgetLayout->addWidget(m_gpuBenchmarkWidget, 0, 0);
    //m_benchmarkWindow->set
    //gpuWidgetLayout->addWidget(m_gpuBenchmarkWidget, 1, 2);
}

MainWindow::~MainWindow()
{
    delete ui;
}
