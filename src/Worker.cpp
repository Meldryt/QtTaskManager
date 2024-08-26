#include "Worker.h"

Worker::Worker(int timerInterval, QObject *parent)
    : QObject{parent}
{
    qDebug() << __FUNCTION__;

    m_timer = new QTimer(this);
    m_timer->setInterval(timerInterval);
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, &Worker::update);
}

Worker::~Worker()
{
    qDebug() << __FUNCTION__;
}

void Worker::start()
{   
    m_timer->start();

    emit signalStarted();
}

void Worker::stop()
{   
    m_timer->stop();

    emit signalStopped();
}