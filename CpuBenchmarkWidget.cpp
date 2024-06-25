#include "CpuBenchmarkWidget.h"

#include <QGridLayout>
#include <QPainter>

CpuBenchmarkWidget::CpuBenchmarkWidget(QWidget *parent) : QWidget{parent}
{
    QGridLayout *layout = new QGridLayout(this);
    //layout->addWidget(listWidget,0,0);
    setLayout(layout);

    //connect(listWidget,&QListWidget::itemSelectionChanged, this, &TabHardware::showSelectionWidget);
}

void CpuBenchmarkWidget::startBenchmark()
{
    m_timer = new QTimer(this);
    m_timer->setInterval(100);
    //connect(m_timer, &QTimer::timeout, this, &TabHardware::process);
    m_timer->start();
}

void CpuBenchmarkWidget::stopBenchmark()
{

}