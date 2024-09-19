#include "CpuWorker.h"

#include "CpuInfo.h"

#include <QElapsedTimer>
#include <QDebug>

CpuWorker::CpuWorker(int timerInterval, QObject *parent)
    : Worker{ timerInterval, parent}
{
    qDebug() << __FUNCTION__;

    m_cpuInfo = std::make_unique<CpuInfo>();

    m_elapsedTimer = new QElapsedTimer();
}

CpuWorker::~CpuWorker()
{
    qDebug() << __FUNCTION__;
}

void CpuWorker::start()
{   
    Worker::start();

    m_cpuInfo->init();

    emit signalStaticInfo(m_cpuInfo->staticInfo());
}

void CpuWorker::stop()
{
    Worker::stop();
}

void CpuWorker::update()
{ 
    qint64 elapsedTime;
    m_elapsedTimer->start();

    m_cpuInfo->update();

    emit signalDynamicInfo(m_cpuInfo->dynamicInfo());

    elapsedTime = m_elapsedTimer->nsecsElapsed() / 1000000;

    if (elapsedTime >= 5)
    {
        qDebug() << __FUNCTION__ << elapsedTime << " ms";
    }
}
