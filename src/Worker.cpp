#include "Worker.h"

#include "process/ProcessInfo.h"
#include "cpu/CpuInfo.h"
#include "gpu/GpuInfo.h"
#include "memory/MemoryInfo.h"
#include "network/NetworkInfo.h"
#include "system/SystemInfo.h"
#ifdef _WIN32
#include "windows/WmiInfo.h"
#endif
#include <QElapsedTimer>

Worker::Worker(const int timerInterval, std::initializer_list<BaseInfo::InfoType> infoTypes, QObject* parent)
    : QObject{parent}
{
    qDebug() << __FUNCTION__;

    createInfoObjects(infoTypes);

    m_timer = new QTimer(this);
    m_timer->setInterval(timerInterval);
    m_timer->setSingleShot(false);
    connect(m_timer, &QTimer::timeout, this, &Worker::update);
}

Worker::~Worker()
{
    qDebug() << __FUNCTION__;
}

void Worker::createInfoObjects(std::initializer_list<BaseInfo::InfoType>& infoTypes)
{
    for (auto&& infoType : infoTypes)
    {
        switch (infoType) 
        {
            case BaseInfo::InfoType::Cpu:
                m_infoObjects.push_back(std::unique_ptr<CpuInfo>(new CpuInfo()));
                break;
            case BaseInfo::InfoType::Gpu:
                m_infoObjects.push_back(std::unique_ptr<GpuInfo>(new GpuInfo()));
                break;
            case BaseInfo::InfoType::Memory:
                m_infoObjects.push_back(std::unique_ptr<MemoryInfo>(new MemoryInfo()));
                break;
            case BaseInfo::InfoType::Network:
                m_infoObjects.push_back(std::unique_ptr<NetworkInfo>(new NetworkInfo()));
                break;
            case BaseInfo::InfoType::Process:
                m_infoObjects.push_back(std::unique_ptr<ProcessInfo>(new ProcessInfo()));
                break;
            case BaseInfo::InfoType::System:
                m_infoObjects.push_back(std::unique_ptr<SystemInfo>(new SystemInfo()));
                break;
            case BaseInfo::InfoType::Wmi:
                m_infoObjects.push_back(std::unique_ptr<WmiInfo>(new WmiInfo()));
                break;
            default:
                break;
        }

    }
}

void Worker::start()
{   
    m_timer->start();

    emit signalStarted();

    for (auto&& infoObject : m_infoObjects)
    {
        infoObject->init();

        emit signalStaticInfo(infoObject->infoType(), infoObject->staticInfo());
    }
}

void Worker::stop()
{   
    m_timer->stop();

    emit signalStopped();
}

void Worker::update()
{
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    std::string names;

    for (auto&& infoObject : m_infoObjects)
    {
        infoObject->update();

        emit signalDynamicInfo(infoObject->infoType(), infoObject->dynamicInfo());

        names += infoObject->name();
    }

    const qint64 elapsedTime = elapsedTimer.nsecsElapsed() / 1000000;

    if (elapsedTime >= 5)
    {
        qDebug() << __FUNCTION__ << " info class: " << names.c_str() << " process time: " << elapsedTime << " ms";
    }
}
