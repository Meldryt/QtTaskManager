#include "TabBenchmark.h"

#include <QGridLayout>
#include <QApplication>

TabBenchmark::TabBenchmark(QWidget *parent) : QWidget(parent)
{
    qDebug() << __FUNCTION__;

    m_buttonStartBenchmark = new QPushButton("Start Benchmark", this);

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(m_buttonStartBenchmark, 0, 0);

    QObject::connect(m_buttonStartBenchmark, &QPushButton::clicked, this, &TabBenchmark::slotOpenBenchmarkWindow);
}

TabBenchmark::~TabBenchmark()
{
    qDebug() << __FUNCTION__;
}

void TabBenchmark::slotOpenBenchmarkWindow()
{
    //m_glWidget = new GlWidget();
    //QRect widgetRect = this->geometry();
    //widgetRect.moveTopLeft(this->mapToGlobal(widgetRect.topRight()));
    //m_glWidget->move(widgetRect.x() + 20, widgetRect.y());
    //m_glWidget->show();
    //MainWindow* mainWindow{ nullptr };
    //for(QWidget* widget : QApplication::topLevelWidgets())
    //{
    //    mainWindow = qobject_cast<MainWindow*>(widget);
    //    if (mainWindow)
    //    {
    //        break;
    //    }
    //}

    m_glWindow = new GlWindow();
    m_glWindow->show();
}
