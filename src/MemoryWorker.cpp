#include "MemoryWorker.h"

#include "MemoryInfo.h"

#include <QElapsedTimer>
#include <QDebug>

MemoryWorker::MemoryWorker(int timerInterval, QObject* parent)
    : Worker{ timerInterval, parent }
{
    m_memoryInfo = std::make_unique<MemoryInfo>();
}

void MemoryWorker::start()
{   
    Worker::start();

    m_memoryInfo->init();

    emit signalStaticInfo(m_memoryInfo->getStaticInfo());
}

void MemoryWorker::stop()
{
    Worker::stop();
}

void MemoryWorker::update()
{ 
    //QElapsedTimer elapsedTimer;
    //qint64 elapsedTime;

    //elapsedTimer.start();

    m_memoryInfo->update();

    //elapsedTime = elapsedTimer.elapsed();

    //qDebug() << "MemoryWorker::update(): " << elapsedTime;

    emit signalDynamicInfo(m_memoryInfo->getDynamicInfo());
}