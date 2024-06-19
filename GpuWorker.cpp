#include "GpuWorker.h"

#include "GpuInfo.h"

#include <QElapsedTimer>
#include <QDebug>

GpuWorker::GpuWorker(int timerInterval, QObject* parent)
    : Worker{ timerInterval, parent }
{
    m_gpuInfo = std::make_unique<GpuInfo>();
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
    //QElapsedTimer elapsedTimer;
    //qint64 elapsedTime;

    //elapsedTimer.start();

    m_gpuInfo->update();

    //elapsedTime = elapsedTimer.elapsed();

    //qDebug() << "GpuWorker::update(): " << elapsedTime;

    emit signalDynamicInfo(m_gpuInfo->getDynamicInfo());
}