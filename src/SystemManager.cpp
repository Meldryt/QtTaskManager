#include "SystemManager.h"

#include "ProcessWorker.h"
#include "CpuWorker.h"
#include "GpuWorker.h"
#include "MemoryWorker.h"

#include <QElapsedTimer>
#include <QDebug>

SystemManager::SystemManager(QWidget* parent) : QTabWidget(parent)
{
    m_tabHardware = new TabHardware(parent);
    m_tabProcesses = new TabProcesses(parent);
    m_tabPerformance = new TabPerformance(parent);
    m_tabBenchmark = new TabBenchmark(parent);

    //m_dockWidget = new QDockWidget("Benchmark",parent);
    addTab(m_tabProcesses, QString("Processes"));
    addTab(m_tabPerformance, QString("Performance"));
    addTab(m_tabHardware, QString("Hardware"));
    addTab(m_tabBenchmark, QString("Benchmark"));

    m_processWorker = std::make_unique<ProcessWorker>(500);
    m_cpuWorker = std::make_unique<CpuWorker>(1000);
    m_gpuWorker = std::make_unique<GpuWorker>(500);
    m_memoryWorker = std::make_unique<MemoryWorker>(500);

    m_worker.push_back(m_processWorker.get());
    m_worker.push_back(m_cpuWorker.get());
    m_worker.push_back(m_gpuWorker.get());
    m_worker.push_back(m_memoryWorker.get());

    for (int i = 0; i < m_worker.size(); ++i)
    {
        QThread* thread = new QThread(this);
        m_worker[i]->moveToThread(thread);
        m_workerThreads.push_back(thread);

        connect(thread, &QThread::started, m_worker[i], &Worker::start);
        connect(m_worker[i], &CpuWorker::finished, thread, &QThread::quit);
        connect(m_worker[i], &CpuWorker::finished, m_worker[i], &CpuWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    }


    connect(m_cpuWorker.get(), &CpuWorker::signalStaticInfo, this, [&](const Globals::CpuStaticInfo& staticInfo)
    {
            m_staticInfoCpu = staticInfo;
            m_staticInfoCpuChanged = true;
    });

    connect(m_cpuWorker.get(), &CpuWorker::signalDynamicInfo, this, [&](const Globals::CpuDynamicInfo& dynamicInfo)
    {     
            m_dynamicInfoCpu = dynamicInfo;
            m_dynamicInfoCpuChanged = true;
    });

    connect(m_gpuWorker.get(), &GpuWorker::signalStaticInfo, this, [&](const Globals::GpuStaticInfo& staticInfo)
    {
            m_staticInfoGpu = staticInfo;
            m_staticInfoGpuChanged = true;
    });

    connect(m_gpuWorker.get(), &GpuWorker::signalDynamicInfo, this, [&](const Globals::GpuDynamicInfo& dynamicInfo)
    {
            m_dynamicInfoGpu = dynamicInfo;
            m_dynamicInfoGpuChanged = true;
    });

    connect(m_processWorker.get(), &ProcessWorker::signalDynamicInfo, this, [&](const std::map<uint32_t, ProcessInfo::Process>& processMap)
    {
            m_processMap = processMap;
            m_processMapChanged = true;
    });

    connect(m_memoryWorker.get(), &MemoryWorker::signalStaticInfo, this, [&](const Globals::MemoryStaticInfo& staticInfo)
    {
            m_staticInfoMemory = staticInfo;
            m_staticInfoMemoryChanged = true;
    });

    connect(m_memoryWorker.get(), &MemoryWorker::signalDynamicInfo, this, [&](const Globals::MemoryDynamicInfo& dynamicInfo)
    {
            m_dynamicInfoMemory = dynamicInfo;
            m_dynamicInfoMemoryChanged = true;
    });

    for (int i = 0; i < m_workerThreads.size(); ++i)
    {
        m_workerThreads[i]->start();
    }

    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    m_timer->setSingleShot(false);
    QObject::connect(m_timer, &QTimer::timeout, this, &SystemManager::update);

    m_timer->start();
}

SystemManager::~SystemManager()
{

}

void SystemManager::update()
{
    QElapsedTimer elapsedTimer;
    qint64 elapsedTime;

    elapsedTimer.start();

    if (m_staticInfoCpuChanged)
    {
        m_tabHardware->slotCpuStaticInfo(m_staticInfoCpu);
        if (m_processWorker)
        {
            m_processWorker->slotProcessorCount(m_staticInfoCpu.processorCount);
        }
        m_staticInfoCpuChanged = false;
    }

    if (m_dynamicInfoCpuChanged)
    {
        m_tabPerformance->slotCpuDynamicInfo(m_dynamicInfoCpu);
        m_dynamicInfoCpuChanged = false;
    }

    if (m_staticInfoGpuChanged)
    {
        m_tabHardware->slotGpuStaticInfo(m_staticInfoGpu);
        m_staticInfoGpuChanged = false;
    }

    if (m_dynamicInfoGpuChanged)
    {
        m_tabPerformance->slotGpuDynamicInfo(m_dynamicInfoGpu);
        m_dynamicInfoGpuChanged = false;
    }

    if (m_processMapChanged)
    {
        m_tabProcesses->slotProcesses(m_processMap);
        m_processMapChanged = false;
    }

    if (m_staticInfoMemoryChanged)
    {
        m_tabHardware->slotTotalPhysicalMemory(m_staticInfoMemory.totalPhysicalMemory);
        m_tabPerformance->slotTotalMemory(m_staticInfoMemory.totalPhysicalMemory);
        m_staticInfoMemoryChanged = false;
    }

    if (m_dynamicInfoMemoryChanged)
    {
        m_tabPerformance->slotUsedMemory(m_dynamicInfoMemory.usedPhysicalMemory);
        m_dynamicInfoMemoryChanged = false;
    }

    m_tabHardware->process();
    m_tabProcesses->process();
    m_tabPerformance->process();

    elapsedTime = elapsedTimer.nsecsElapsed() * 0.000000001;

    qDebug() << "SystemManager::update(): " << elapsedTime;
}