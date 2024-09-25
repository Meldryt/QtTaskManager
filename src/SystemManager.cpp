#include "SystemManager.h"

#include <QElapsedTimer>
#include <QDebug>

SystemManager::SystemManager(QWidget* parent) : QTabWidget(parent)
{
    qDebug() << __FUNCTION__;

    m_tabPerformance = new TabPerformance(parent);
    m_tabProcesses = new TabProcesses(parent);
    m_tabHardware = new TabHardware(parent);
    m_tabSystemInfo = new TabSystemInfo(parent);
    m_tabApiSupport = new TabApiSupport(parent);
    m_tabBenchmark = new TabBenchmark(parent);

    addTab(m_tabPerformance, QString("Performance"));
    addTab(m_tabProcesses, QString("Processes"));
    addTab(m_tabHardware, QString("Hardware"));
    addTab(m_tabSystemInfo, QString("SystemInfo"));
    addTab(m_tabApiSupport, QString("ApiSupport"));
    addTab(m_tabBenchmark, QString("Benchmark"));

    m_processWorker = std::make_unique<Worker>(2000, Worker::InfoType::Process); //500
    m_cpuWorker = std::make_unique<Worker>(1000, Worker::InfoType::Cpu);
    m_gpuWorker = std::make_unique<Worker>(1000, Worker::InfoType::Gpu);
    m_memoryWorker = std::make_unique<Worker>(1000, Worker::InfoType::Memory);
    m_networkWorker = std::make_unique<Worker>(1000, Worker::InfoType::Network);
    m_systemWorker = std::make_unique<Worker>(1000, Worker::InfoType::System);

#ifdef _WIN32
    m_wmiWorker = std::make_unique<Worker>(1000, Worker::InfoType::Wmi);
#endif
    m_worker.push_back(m_processWorker.get());
    m_worker.push_back(m_cpuWorker.get());
    m_worker.push_back(m_gpuWorker.get());
    m_worker.push_back(m_memoryWorker.get());
    m_worker.push_back(m_networkWorker.get());
    m_worker.push_back(m_systemWorker.get());
#ifdef _WIN32
    m_worker.push_back(m_wmiWorker.get());
#endif
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

    connect(m_cpuWorker.get(), &Worker::signalStaticInfo, this, [&](const QMap<uint8_t,QVariant>& staticInfo)
    {
        m_staticInfoCpu = staticInfo;
        m_staticInfoCpuChanged = true;
    });

    connect(m_cpuWorker.get(), &Worker::signalDynamicInfo, this, [&](const QMap<uint8_t,QVariant>& dynamicInfo)
    {     
        m_dynamicInfoCpu = dynamicInfo;
        m_dynamicInfoCpuChanged = true;
    });

    connect(m_gpuWorker.get(), &Worker::signalStaticInfo, this, [&](const QMap<uint8_t,QVariant>& staticInfo)
    {
        m_staticInfoGpu = staticInfo;
        m_staticInfoGpuChanged = true;
    });

    connect(m_gpuWorker.get(), &Worker::signalDynamicInfo, this, [&](const QMap<uint8_t,QVariant>& dynamicInfo)
    {
        m_dynamicInfoGpu = dynamicInfo;
        m_dynamicInfoGpuChanged = true;
    });

    connect(m_processWorker.get(), &Worker::signalDynamicInfo, this, [&](const QMap<uint8_t,QVariant>& dynamicInfo)
    {
        m_dynamicInfoProcess = dynamicInfo;
        m_dynamicInfoProcessChanged = true;
    });

    connect(m_memoryWorker.get(), &Worker::signalStaticInfo, this, [&](const QMap<uint8_t,QVariant>& staticInfo)
    {
        m_staticInfoMemory = staticInfo;
        m_staticInfoMemoryChanged = true;
    });

    connect(m_memoryWorker.get(), &Worker::signalDynamicInfo, this, [&](const QMap<uint8_t,QVariant>& dynamicInfo)
    {
        m_dynamicInfoMemory = dynamicInfo;
        m_dynamicInfoMemoryChanged = true;
    });

    connect(m_networkWorker.get(), &Worker::signalDynamicInfo, this, [&](const QMap<uint8_t,QVariant>& dynamicInfo)
    {
        m_dynamicInfoNetwork = dynamicInfo;
        m_dynamicInfoNetworkChanged = true;
    });

    connect(m_systemWorker.get(), &Worker::signalStaticInfo, this, [&](const QMap<uint8_t,QVariant>& staticInfo)
    {
        m_staticInfoSystem = staticInfo;
        m_staticInfoSystemChanged = true;
    });

#ifdef _WIN32
    connect(m_wmiWorker.get(), &Worker::signalStaticInfo, this, [&](const QMap<uint8_t, QVariant>& staticInfo)
    {
        m_staticInfoWmi = staticInfo;
        m_staticInfoWmiChanged = true;
    });

    connect(m_wmiWorker.get(), &Worker::signalDynamicInfo, this, [&](const QMap<uint8_t,QVariant>& dynamicInfo)
    {
        m_dynamicInfoWmi = dynamicInfo;
        m_dynamicInfoWmiChanged = true;
    });
#endif
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
    bool processTabSystem = false;

    if (m_staticInfoCpuChanged)
    {
        m_tabHardware->slotCpuStaticInfo(m_staticInfoCpu);
        processTabHardware = true;

        if (m_processWorker)
        {
            QVariant variant = m_staticInfoCpu[Globals::Key_Cpu_Static_CoreCount];
            if (variant.canConvert<uint16_t>())
            {
                const uint16_t coreCount = variant.value<uint16_t>();
                //m_processWorker->slotCoreCount(coreCount);
            }
        }
        m_staticInfoCpuChanged = false;
    }

    if (m_dynamicInfoCpuChanged)
    {
#ifdef _WIN32
        if(m_dynamicInfoWmi[Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadFrequencies] != Globals::SysInfo_Uninitialized)
        {
            m_dynamicInfoCpu[Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadFrequencies] = m_dynamicInfoWmi[Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadFrequencies];
        }
        if(m_dynamicInfoWmi[Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadUsages] != Globals::SysInfo_Uninitialized)
        {
            m_dynamicInfoCpu[Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadUsages] = m_dynamicInfoWmi[Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadUsages];
        }
#endif
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

    if (m_dynamicInfoProcessChanged)
    {
//#ifdef _WIN32
//        QVariant var = m_dynamicInfoWmi[Globals::SysInfoAttr::Key_Process_GpuUsages];
//        if (var.canConvert<QMap<uint32_t, QPair<uint8_t, uint64_t>>>())
//        {
//            QMap<uint32_t, QPair<uint8_t, uint64_t>> processGpuUsages = var.value<QMap<uint32_t, QPair<uint8_t, uint64_t>> >();
//            for (auto&& process : processGpuUsages.asKeyValueRange())
//            {
//                if (m_processMap.find(process.first) != m_processMap.end())
//                {
//                    m_processMap[process.first].gpuUsagePercent = process.second.first;
//                    m_processMap[process.first].videoRamUsageSize = process.second.second;
//                }
//            }
//        }
//#endif
        m_tabProcesses->slotProcessDynamicInfo(m_dynamicInfoProcess);
        m_dynamicInfoProcessChanged = false;
    }

    if (m_staticInfoMemoryChanged)
    {
        QVariant variant = m_staticInfoMemory[Globals::Key_Memory_Static_TotalPhysicalMemory];
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
        QVariant variant = m_dynamicInfoMemory[Globals::Key_Memory_Dynamic_UsedPhysicalMemory];
        if (variant.canConvert<uint32_t>())
        {
            const uint32_t usedPhysicalMemory = variant.value<uint32_t>();
            m_tabPerformance->slotUsedMemory(usedPhysicalMemory);
        }

        m_dynamicInfoMemoryChanged = false;
    }

    if (m_staticInfoSystemChanged)
    {
        m_tabSystemInfo->slotOSInfo(m_staticInfoSystem);
        processTabSystem = true;

        m_staticInfoSystemChanged = false;
    }

#ifdef _WIN32
    if (m_staticInfoWmiChanged)
    {
        m_tabApiSupport->slotApiSupportStaticInfo(m_staticInfoWmi);

        m_staticInfoWmiChanged = false;
        processTabApiSupport = true;
    }

    if (m_dynamicInfoWmiChanged)
    {
        m_tabPerformance->slotNetworkDynamicInfo(m_dynamicInfoWmi);

        m_dynamicInfoWmiChanged = false;
    }
#elif __linux__
    if (m_dynamicInfoNetworkChanged)
    {
        m_tabPerformance->slotNetworkDynamicInfo(m_dynamicInfoNetwork);

        m_dynamicInfoNetworkChanged = false;
    }
#endif
    if (processTabHardware)
    {
        m_tabHardware->process();
    }

    if (processTabApiSupport)
    {
        m_tabApiSupport->process();
    }

    if (processTabSystem)
    {
        m_tabSystemInfo->process();
    }
    
    m_tabProcesses->process();
    m_tabPerformance->process();

    elapsedTime = elapsedTimer.nsecsElapsed() / 1000000;

    if(elapsedTime > 5)
    {
        qDebug() << __FUNCTION__ << elapsedTime;
    }
}
