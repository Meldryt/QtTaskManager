#include "ProcessWorker.h"
#include <QDebug>
#include <QElapsedTimer>

ProcessWorker::ProcessWorker(int timerInterval, QObject* parent)
    : Worker{ timerInterval, parent }
{
    m_processInfo = std::make_unique<ProcessInfo>();
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

    if (m_finished)
    {
        m_finished = false;

        elapsedTimer.start();

        m_processInfo->update();

        elapsedTime = elapsedTimer.elapsed();

        emit signalDynamicInfo(m_processInfo->getProcessMap());

        m_finished = true;
    }

    //qDebug() << "ProcessWorker::update(): " << elapsedTime;
}

void ProcessWorker::slotProcessorCount(uint8_t newProcessorCount)
{
    m_processInfo->setProcessorCount(newProcessorCount);
}