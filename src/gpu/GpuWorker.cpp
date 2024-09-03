#include "GpuWorker.h"

#include "GpuInfo.h"

#include <QElapsedTimer>
#include <QDebug>

GpuWorker::GpuWorker(int timerInterval, QObject* parent)
    : Worker{ timerInterval, parent}
{
    qDebug() << __FUNCTION__;
    
    m_gpuInfo = std::make_unique<GpuInfo>();
}

GpuWorker::~GpuWorker()
{
    qDebug() << __FUNCTION__;
}

void GpuWorker::start()
{   
    Worker::start();

    m_gpuInfo->init();

    emit signalStaticInfo(m_gpuInfo->getStaticInfo());
}

void GpuWorker::stop()
{
    Worker::stop();
}

void GpuWorker::update()
{ 
    QElapsedTimer elapsedTimer;
    qint64 elapsedTime;
    elapsedTimer.start();

    m_gpuInfo->update();

    emit signalDynamicInfo(m_gpuInfo->getDynamicInfo());

    elapsedTime = elapsedTimer.nsecsElapsed() / 1000000;

    if (elapsedTime >= 10)
    {
        qDebug() << "GpuWorker::update(): " << elapsedTime << " ms";
    }
}