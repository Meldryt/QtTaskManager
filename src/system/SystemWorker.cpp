#include "SystemWorker.h"

#include "SystemInfo.h"
#include "../Globals.h"

SystemWorker::SystemWorker(int timerInterval, QObject* parent)
    : Worker{ timerInterval, parent}
{
    qDebug() << __FUNCTION__;

    m_systemInfo = std::make_unique<SystemInfo>();
}

SystemWorker::~SystemWorker()
{
    qDebug() << __FUNCTION__;
}

void SystemWorker::start()
{   
    Worker::start();

    m_systemInfo->init();

    emit signalStaticInfo(m_systemInfo->getStaticInfo());
}

void SystemWorker::stop()
{
    Worker::stop();
}

void SystemWorker::update()
{ 
    m_systemInfo->update();

    emit signalDynamicInfo(m_systemInfo->getDynamicInfo());
}