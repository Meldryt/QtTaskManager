#include "NetworkWorker.h"

#include "NetworkInfo.h"
#include "../Globals.h"

NetworkWorker::NetworkWorker(int timerInterval, QObject* parent)
    : Worker{ timerInterval, parent}
{
    qDebug() << __FUNCTION__;

    m_networkInfo = std::make_unique<NetworkInfo>();
}

NetworkWorker::~NetworkWorker()
{
    qDebug() << __FUNCTION__;
}

void NetworkWorker::start()
{   
    Worker::start();

    m_networkInfo->init();

    emit signalStaticInfo(m_networkInfo->staticInfo());
}

void NetworkWorker::stop()
{
    Worker::stop();
}

void NetworkWorker::update()
{ 
    m_networkInfo->update();

    emit signalDynamicInfo(m_networkInfo->dynamicInfo());
}
