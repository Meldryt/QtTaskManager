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

Worker::Worker(const int timerInterval, const InfoType type, QObject* parent)
    : m_infoType{type}, QObject{parent}
{
    qDebug() << __FUNCTION__;

    switch (m_infoType) {
    case Cpu:
        m_baseInfo = std::make_unique<CpuInfo>();
        break;
    case Gpu:
        m_baseInfo = std::make_unique<GpuInfo>();
        break;
    case Memory:
        m_baseInfo = std::make_unique<MemoryInfo>();
        break;
    case Network:
        m_baseInfo = std::make_unique<NetworkInfo>();
        break;
    case Process:
        m_baseInfo = std::make_unique<ProcessInfo>();
        break;
    case System:
        m_baseInfo = std::make_unique<SystemInfo>();
        break;
    case Wmi:
        m_baseInfo = std::make_unique<WmiInfo>();
        break;
    default:
        break;
    }

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

    m_baseInfo->init();

    emit signalStaticInfo(m_baseInfo->staticInfo());
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

    m_baseInfo->update();

    emit signalDynamicInfo(m_baseInfo->dynamicInfo());

    const qint64 elapsedTime = elapsedTimer.nsecsElapsed() / 1000000;

    if (elapsedTime >= 5)
    {
        qDebug() << __FUNCTION__ << " " << m_baseInfo->name().c_str() << " " << elapsedTime << " ms";
    }
}
