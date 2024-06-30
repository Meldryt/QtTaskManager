#include "TabBenchmark.h"

#include <QGridLayout>

TabBenchmark::TabBenchmark(QWidget *parent) : QWidget(parent)
{
    m_buttonStartBenchmark = new QPushButton("Start Benchmark", this);

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(m_buttonStartBenchmark, 0, 0);

    QObject::connect(m_buttonStartBenchmark, &QPushButton::clicked, this, &TabBenchmark::slotOpenBenchmarkWindow);
}

void TabBenchmark::slotOpenBenchmarkWindow()
{
    m_glWidget = new GlWidget();
    m_glWidget->resize(800, 600);
    QRect widgetRect = this->geometry();
    widgetRect.moveTopLeft(this->mapToGlobal(widgetRect.topRight()));
    m_glWidget->move(widgetRect.x() + 20, widgetRect.y());
    m_glWidget->show(); 
}
