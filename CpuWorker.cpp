#include "CpuWorker.h"

#include "CpuInfo.h"

#include <QElapsedTimer>
#include <QDebug>

CpuWorker::CpuWorker(int timerInterval, QObject *parent)
    : Worker{ timerInterval, parent}
{
    m_cpuInfo = std::make_unique<CpuInfo>();
}

void CpuWorker::start()
{   
    Worker::start();

    m_cpuInfo->init();

    emit signalStaticInfo(m_cpuInfo->getStaticInfo());
}

void CpuWorker::stop()
{
    Worker::stop();
}

void CpuWorker::update()
{ 
    //QElapsedTimer elapsedTimer;
    //qint64 elapsedTime;

    //elapsedTimer.start();

    m_cpuInfo->update();

    //elapsedTime = elapsedTimer.elapsed();

    //qDebug() << "CpuWorker::update(): " << elapsedTime;

    emit signalDynamicInfo(m_cpuInfo->getDynamicInfo());
}
