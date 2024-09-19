#include "ProcessWorker.h"

#include <QDebug>

ProcessWorker::ProcessWorker(int timerInterval, QObject* parent)
    : Worker{ timerInterval, parent}
{
    qDebug() << __FUNCTION__;

    m_processInfo = std::make_unique<ProcessInfo>();

    m_elapsedTimer = new QElapsedTimer();
}

ProcessWorker::~ProcessWorker()
{
    qDebug() << __FUNCTION__;
}

void ProcessWorker::start()
{   
    Worker::start();

    m_processInfo->init();

    //emit signalStaticInfo(m_processInfo->staticInfo());
}

void ProcessWorker::stop()
{
    Worker::stop();
}

void ProcessWorker::update()
{
    qint64 elapsedTime;
    m_elapsedTimer->start();

    m_processInfo->update();

    emit signalDynamicInfo(m_processInfo->processMap());

    elapsedTime = m_elapsedTimer->nsecsElapsed() / 1000000;

    if (elapsedTime >= 5)
    {
        qDebug() << __FUNCTION__ << elapsedTime << " ms";
    }
}

void ProcessWorker::slotCoreCount(uint16_t newCoreCount)
{
    m_processInfo->setCoreCount(newCoreCount);
}
