#include "WmiWorker.h"

#include "WmiInfo.h"

#include <QElapsedTimer>
#include <QDebug>

WmiWorker::WmiWorker(int timerInterval, QObject *parent)
    : Worker{ timerInterval, parent}
{
    m_wmiInfo = std::make_unique<WmiInfo>();

    m_elapsedTimer = new QElapsedTimer();
}

void WmiWorker::start()
{   
    Worker::start();

    m_elapsedTimer->start();

    m_wmiInfo->init();

    emit signalStaticInfo(m_wmiInfo->staticInfo());
}

void WmiWorker::stop()
{
    Worker::stop();
}

void WmiWorker::update()
{ 
    QElapsedTimer elapsedTimer;
    qint64 elapsedTime;
    elapsedTimer.start();

    m_wmiInfo->update();

    emit signalDynamicInfo(m_wmiInfo->dynamicInfo());

    elapsedTime = elapsedTimer.nsecsElapsed() / 1000000;

    if (elapsedTime >= 10)
    {
        qDebug() << "WmiWorker::update(): " << elapsedTime << " ms";
    }
}
