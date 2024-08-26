#include "ProcessWorker.h"
#include <QDebug>
#include <QElapsedTimer>

ProcessWorker::ProcessWorker(int timerInterval, QObject* parent)
    : Worker{ timerInterval, parent}
{
    qDebug() << __FUNCTION__;

    m_processInfo = std::make_unique<ProcessInfo>();
}

ProcessWorker::~ProcessWorker()
{
    qDebug() << __FUNCTION__;
}

void ProcessWorker::start()
{   
    Worker::start();
}

void ProcessWorker::stop()
{
    Worker::stop();
}

void ProcessWorker::update()
{ 
    QElapsedTimer elapsedTimer;
    qint64 elapsedTime;
    elapsedTimer.start();

    m_processInfo->update();

    emit signalDynamicInfo(m_processInfo->getProcessMap());

    elapsedTime = elapsedTimer.nsecsElapsed() * 0.000000001;

    if(elapsedTime > 0)
    {
        qDebug() << "ProcessWorker::update(): " << elapsedTime;        
    }  
}

void ProcessWorker::slotProcessorCount(uint8_t newProcessorCount)
{
    m_processInfo->setProcessorCount(newProcessorCount);
}