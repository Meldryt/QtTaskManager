#include "GpuBenchmarkWidget.h"

#include <QGridLayout>
#include <QPainter>

GpuBenchmarkWidget::GpuBenchmarkWidget(QWidget *parent) : QWidget{parent}
{
    m_glWidget = new GlWidget(this);
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_glWidget,0,0);
    setLayout(layout);

    //connect(listWidget,&QListWidget::itemSelectionChanged, this, &TabHardware::showSelectionWidget);
}

void GpuBenchmarkWidget::startBenchmark()
{
    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    //connect(m_timer, &QTimer::timeout, this, &TabHardware::process);
    m_timer->start();
}

void GpuBenchmarkWidget::stopBenchmark()
{

}