#include "CpuWorker.h"

#include "CpuInfo.h"

#include <QElapsedTimer>
#include <QDebug>

CpuWorker::CpuWorker(int timerInterval, QObject *parent)
    : Worker{ timerInterval, parent}
{
    m_cpuInfo = std::make_unique<CpuInfo>();

    m_elapsedTimer = new QElapsedTimer();
}

void CpuWorker::start()
{   
    Worker::start();

    m_elapsedTimer->start();

    m_cpuInfo->init();

    emit signalStaticInfo(m_cpuInfo->getStaticInfo());
}

void CpuWorker::stop()
{
    Worker::stop();
}

void CpuWorker::update()
{ 
    QElapsedTimer elapsedTimer;
    qint64 elapsedTime;
    elapsedTimer.start();

    m_cpuInfo->update();

    emit signalDynamicInfo(m_cpuInfo->getDynamicInfo());

    elapsedTime = elapsedTimer.nsecsElapsed() / 1000000;

    if (elapsedTime >= 10)
    {
        qDebug() << "CpuWorker::update(): " << elapsedTime << " ms";
    }
}
