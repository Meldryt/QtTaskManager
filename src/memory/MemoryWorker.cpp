#include "MemoryWorker.h"

#include "MemoryInfo.h"
#include "../Globals.h"

MemoryWorker::MemoryWorker(int timerInterval, QObject* parent)
    : Worker{ timerInterval, parent}
{
    qDebug() << __FUNCTION__;

    m_memoryInfo = std::make_unique<MemoryInfo>();
}

MemoryWorker::~MemoryWorker()
{
    qDebug() << __FUNCTION__;
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