#include "MemoryWorker.h"
#include "MemoryInfo.h"

MemoryWorker::MemoryWorker(int timerInterval, QObject* parent)
    : Worker{ timerInterval, parent}
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
    m_memoryInfo->update();

    emit signalDynamicInfo(m_memoryInfo->getDynamicInfo());
}