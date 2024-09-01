#include "SystemManager.h"

#include "ProcessWorker.h"
#include "cpu/CpuWorker.h"
#include "gpu/GpuWorker.h"
#include "MemoryWorker.h"
#include "windows/WmiWorker.h"

#include <QElapsedTimer>
#include <QDebug>

SystemManager::SystemManager(QWidget* parent) : QTabWidget(parent)
{
    qDebug() << __FUNCTION__;

    m_tabProcesses = new TabProcesses(parent);
    m_tabPerformance = new TabPerformance(parent);
    m_tabHardware = new TabHardware(parent);
    m_tabApiSupport = new TabApiSupport(parent);
    m_tabBenchmark = new TabBenchmark(parent);

    addTab(m_tabProcesses, QString("Processes"));
    addTab(m_tabPerformance, QString("Performance"));
    addTab(m_tabHardware, QString("Hardware"));
    addTab(m_tabApiSupport, QString("ApiSupport"));
    addTab(m_tabBenchmark, QString("Benchmark"));

    m_processWorker = std::make_unique<ProcessWorker>(500);
    m_cpuWorker = std::make_unique<CpuWorker>(1000);
    m_gpuWorker = std::make_unique<GpuWorker>(500);
    m_memoryWorker = std::make_unique<MemoryWorker>(500);
    m_wmiWorker = std::make_unique<WmiWorker>(1000);

    m_worker.push_back(m_processWorker.get());
    m_worker.push_back(m_cpuWorker.get());
    m_worker.push_back(m_gpuWorker.get());
    m_worker.push_back(m_memoryWorker.get());
    m_worker.push_back(m_wmiWorker.get());

    for (int i = 0; i < m_worker.size(); ++i)
    {
        QThread* thread = new QThread(this);
        m_worker[i]->moveToThread(thread);
        m_workerThreads.push_back(thread);

        connect(thread, &QThread::started, m_worker[i], &Worker::start);
        connect(m_worker[i], &Worker::signalFinished, thread, &QThread::quit);
        connect(m_worker[i], &Worker::signalFinished, m_worker[i], &Worker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    }

    connect(m_cpuWorker.get(), &CpuWorker::signalStaticInfo, this, [&](const QMap<uint8_t,QVariant>& staticInfo)
    {
        m_staticInfoCpu = staticInfo;
        m_staticInfoCpuChanged = true;
    });

    connect(m_cpuWorker.get(), &CpuWorker::signalDynamicInfo, this, [&](const QMap<uint8_t,QVariant>& dynamicInfo)
    {     
        m_dynamicInfoCpu = dynamicInfo;
        m_dynamicInfoCpuChanged = true;
    });

    connect(m_gpuWorker.get(), &GpuWorker::signalStaticInfo, this, [&](const QMap<uint8_t,QVariant>& staticInfo)
    {
        m_staticInfoGpu = staticInfo;
        m_staticInfoGpuChanged = true;
    });

    connect(m_gpuWorker.get(), &GpuWorker::signalDynamicInfo, this, [&](const QMap<uint8_t,QVariant>& dynamicInfo)
    {
        m_dynamicInfoGpu = dynamicInfo;
        m_dynamicInfoGpuChanged = true;
    });

    connect(m_processWorker.get(), &ProcessWorker::signalDynamicInfo, this, [&](const std::map<uint32_t, ProcessInfo::Process>& processMap)
    {
        m_processMap = processMap;
        m_processMapChanged = true;
    });

    connect(m_memoryWorker.get(), &MemoryWorker::signalStaticInfo, this, [&](const QMap<uint8_t,QVariant>& staticInfo)
    {
        m_staticInfoMemory = staticInfo;
        m_staticInfoMemoryChanged = true;
    });

    connect(m_memoryWorker.get(), &MemoryWorker::signalDynamicInfo, this, [&](const QMap<uint8_t,QVariant>& dynamicInfo)
    {
        m_dynamicInfoMemory = dynamicInfo;
        m_dynamicInfoMemoryChanged = true;
    });

    connect(m_wmiWorker.get(), &WmiWorker::signalStaticInfo, this, [&](const QMap<uint8_t, QVariant>& staticInfo)
    {
        m_staticInfoWmi = staticInfo;
        m_staticInfoWmiChanged = true;

        //Key_Api_Functions_StatusSupport_Adlx,
        //    Key_Api_Functions_StatusSupport_Nvapi,
        //    Key_Api_Functions_StatusSupport_Nvml,
        //    Key_Api_Functions_StatusSupport_RyzenMaster,
        //    Key_Api_Functions_StatusSupport_Wmi,

        //    Key_Api_Functions_ErrorMessage_Adlx,
        //    Key_Api_Functions_ErrorMessage_Nvapi,
        //    Key_Api_Functions_ErrorMessage_Nvml,
        //    Key_Api_Functions_ErrorMessage_RyzenMaster,
        //    Key_Api_Functions_ErrorMessage_Wmi,
    });

    connect(m_wmiWorker.get(), &WmiWorker::signalDynamicInfo, this, [&](const QMap<uint8_t,QVariant>& dynamicInfo)
    {
        m_dynamicInfoWmi = dynamicInfo;
        m_dynamicInfoWmiChanged = true;
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
    qDebug() << __FUNCTION__;
}

void SystemManager::update()
{
    QElapsedTimer elapsedTimer;
    qint64 elapsedTime;

    elapsedTimer.start();

    bool processTabHardware = false;
    bool processTabApiSupport = false;

    if (m_staticInfoCpuChanged)
    {
        m_tabHardware->slotCpuStaticInfo(m_staticInfoCpu);
        processTabHardware = true;

        if (m_processWorker)
        {
            QVariant variant = m_staticInfoCpu[Globals::Key_Cpu_ProcessorCount];
            if (variant.canConvert<uint8_t>())
            {
                const uint8_t processorCount = variant.value<uint8_t>();
                m_processWorker->slotProcessorCount(processorCount);
            }
        }
        m_staticInfoCpuChanged = false;
    }

    if (m_dynamicInfoCpuChanged)
    {
        m_dynamicInfoCpu[Globals::SysInfoAttr::Key_Cpu_ThreadFrequencies] = m_dynamicInfoWmi[Globals::SysInfoAttr::Key_Cpu_ThreadFrequencies];
        m_dynamicInfoCpu[Globals::SysInfoAttr::Key_Cpu_ThreadUsages] = m_dynamicInfoWmi[Globals::SysInfoAttr::Key_Cpu_ThreadUsages];

        m_tabPerformance->slotCpuDynamicInfo(m_dynamicInfoCpu);
        m_dynamicInfoCpuChanged = false;
    }

    if (m_staticInfoGpuChanged)
    {
        m_tabHardware->slotGpuStaticInfo(m_staticInfoGpu);
        m_tabApiSupport->slotApiSupportStaticInfo(m_staticInfoGpu);
        m_staticInfoGpuChanged = false;
        processTabHardware = true;
        processTabApiSupport = true;
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
        QVariant variant = m_staticInfoMemory[Globals::Key_Memory_TotalPhysicalMemory];
        if (variant.canConvert<uint32_t>())
        {
            const uint32_t totalPhysicalMemory = variant.value<uint32_t>();
            m_tabHardware->slotTotalPhysicalMemory(totalPhysicalMemory);
            m_tabPerformance->slotTotalMemory(totalPhysicalMemory);
            processTabHardware = true;
        }
        
        m_staticInfoMemoryChanged = false;
    }

    if (m_dynamicInfoMemoryChanged)
    {
        QVariant variant = m_dynamicInfoMemory[Globals::Key_Memory_UsedPhysicalMemory];
        if (variant.canConvert<uint32_t>())
        {
            const uint32_t usedPhysicalMemory = variant.value<uint32_t>();
            m_tabPerformance->slotUsedMemory(usedPhysicalMemory);
        }

        m_dynamicInfoMemoryChanged = false;
    }

    if (m_staticInfoWmiChanged)
    {
        m_tabApiSupport->slotApiSupportStaticInfo(m_staticInfoWmi);
        m_staticInfoWmiChanged = false;
        processTabApiSupport = true;
    }

    if (m_dynamicInfoWmiChanged)
    {
        QVariant variant = m_dynamicInfoWmi[Globals::Key_Network_BytesReceivedPerSec];
        if (variant.canConvert<uint32_t>())
        {
            const uint32_t usedBytesReceivedPerSec = variant.value<uint32_t>();
            m_tabPerformance->slotUsedNetworkSpeed(usedBytesReceivedPerSec);
        }

        m_dynamicInfoWmiChanged = false;
    }

    if (processTabHardware)
    {
        m_tabHardware->process();
    }

    if (processTabApiSupport)
    {
        m_tabApiSupport->process();
    }
    
    m_tabProcesses->process();
    m_tabPerformance->process();

    elapsedTime = elapsedTimer.nsecsElapsed() * 0.000000001;

    if(elapsedTime > 1)
    {
        qDebug() << "SystemManager::update(): " << elapsedTime;        
    }  
}