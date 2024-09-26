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

    m_processWorker.reset(new Worker(1000, { BaseInfo::InfoType::Process }));
    m_cpuWorker.reset(new Worker(1000, { BaseInfo::InfoType::Cpu}));
    m_gpuWorker.reset(new Worker(1000, { BaseInfo::InfoType::Gpu}));
    m_memoryWorker.reset(new Worker(1000, { BaseInfo::InfoType::Memory}));
    m_networkWorker.reset(new Worker(1000, { BaseInfo::InfoType::Network}));
    m_systemWorker.reset(new Worker(1000, { BaseInfo::InfoType::System}));

#ifdef _WIN32
    m_wmiWorker.reset(new Worker(1000, { BaseInfo::InfoType::Wmi }));
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

        connect(m_worker[i], &Worker::signalStaticInfo, this, [&](const BaseInfo::InfoType& infoType, const QMap<uint8_t, QVariant>& staticInfo)
        {
            m_staticInfos[infoType] = staticInfo;
            m_staticInfosChanged[infoType] = true;
        });

        connect(m_worker[i], &Worker::signalDynamicInfo, this, [&](const BaseInfo::InfoType& infoType, const QMap<uint8_t, QVariant>& dynamicInfo)
        {
            m_dynamicInfos[infoType] = dynamicInfo;
            m_dynamicInfosChanged[infoType] = true;
        });
    }

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

    if (m_staticInfosChanged[BaseInfo::InfoType::Cpu])
    {
        m_tabHardware->slotCpuStaticInfo(m_staticInfos[BaseInfo::InfoType::Cpu]);
        processTabHardware = true;

        m_staticInfosChanged[BaseInfo::InfoType::Cpu] = false;
    }

    if (m_dynamicInfosChanged[BaseInfo::InfoType::Cpu])
    {
#ifdef _WIN32
        if(m_dynamicInfos[BaseInfo::InfoType::Wmi][Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadFrequencies] != Globals::SysInfo_Uninitialized)
        {
            m_dynamicInfos[BaseInfo::InfoType::Cpu][Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadFrequencies] = m_dynamicInfos[BaseInfo::InfoType::Wmi][Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadFrequencies];
        }
        if(m_dynamicInfos[BaseInfo::InfoType::Wmi][Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadUsages] != Globals::SysInfo_Uninitialized)
        {
            m_dynamicInfos[BaseInfo::InfoType::Cpu][Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadUsages] = m_dynamicInfos[BaseInfo::InfoType::Wmi][Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadUsages];
        }
#endif
        m_tabPerformance->slotCpuDynamicInfo(m_dynamicInfos[BaseInfo::InfoType::Cpu]);
        m_dynamicInfosChanged[BaseInfo::InfoType::Cpu] = false;
    }

    if (m_staticInfosChanged[BaseInfo::InfoType::Gpu])
    {
        m_tabHardware->slotGpuStaticInfo(m_staticInfos[BaseInfo::InfoType::Gpu]);
        m_tabApiSupport->slotApiSupportStaticInfo(m_staticInfos[BaseInfo::InfoType::Gpu]);
        m_staticInfosChanged[BaseInfo::InfoType::Gpu] = false;
        processTabHardware = true;
        processTabApiSupport = true;
    }

    if (m_dynamicInfosChanged[BaseInfo::InfoType::Gpu])
    {
        m_tabPerformance->slotGpuDynamicInfo(m_dynamicInfos[BaseInfo::InfoType::Gpu]);
        m_dynamicInfosChanged[BaseInfo::InfoType::Gpu] = false;
    }

    if (m_dynamicInfosChanged[BaseInfo::InfoType::Process])
    {
#ifdef _WIN32
        const QVariant varGpuUsages = m_dynamicInfos[BaseInfo::InfoType::Wmi][Globals::SysInfoAttr::Key_Process_GpuUsages];
        if (varGpuUsages.canConvert<QMap<uint32_t, QPair<uint8_t, uint64_t>>>())
        {
            const QVariant varProcesses = m_dynamicInfos[BaseInfo::InfoType::Process][Globals::SysInfoAttr::Key_Process_Dynamic_Info];
            if (varProcesses.canConvert<std::map<uint32_t, ProcessInfo::Process>>())
            {
                const QMap<uint32_t, QPair<uint8_t, uint64_t>> processGpuUsages = varGpuUsages.value<QMap<uint32_t, QPair<uint8_t, uint64_t>> >();
                std::map<uint32_t, ProcessInfo::Process> processMap = varProcesses.value<std::map<uint32_t, ProcessInfo::Process>>();
                for (auto&& process : processGpuUsages.asKeyValueRange())
                {
                    if (processMap.find(process.first) != processMap.end())
                    {
                        processMap[process.first].gpuUsagePercent = process.second.first;
                        processMap[process.first].videoRamUsageSize = process.second.second;
                    }
                }
                m_dynamicInfos[BaseInfo::InfoType::Process][Globals::SysInfoAttr::Key_Process_Dynamic_Info] = QVariant::fromValue(processMap);
            }          
        }
#endif
        m_tabProcesses->slotProcessDynamicInfo(m_dynamicInfos[BaseInfo::InfoType::Process]);
        m_dynamicInfosChanged[BaseInfo::InfoType::Process] = false;
    }

    if (m_staticInfosChanged[BaseInfo::InfoType::Memory])
    {
        QVariant variant = m_staticInfos[BaseInfo::InfoType::Memory][Globals::Key_Memory_Static_TotalPhysicalMemory];
        if (variant.canConvert<uint32_t>())
        {
            const uint32_t totalPhysicalMemory = variant.value<uint32_t>();
            m_tabHardware->slotTotalPhysicalMemory(totalPhysicalMemory);
            m_tabPerformance->slotTotalMemory(totalPhysicalMemory);
            processTabHardware = true;
        }
        
        m_staticInfosChanged[BaseInfo::InfoType::Memory] = false;
    }

    if (m_dynamicInfosChanged[BaseInfo::InfoType::Memory])
    {
        QVariant variant = m_dynamicInfos[BaseInfo::InfoType::Memory][Globals::Key_Memory_Dynamic_UsedPhysicalMemory];
        if (variant.canConvert<uint32_t>())
        {
            const uint32_t usedPhysicalMemory = variant.value<uint32_t>();
            m_tabPerformance->slotUsedMemory(usedPhysicalMemory);
        }

        m_dynamicInfosChanged[BaseInfo::InfoType::Memory] = false;
    }

    if (m_staticInfosChanged[BaseInfo::InfoType::System])
    {
        m_tabSystemInfo->slotOSInfo(m_staticInfos[BaseInfo::InfoType::System]);
        processTabSystem = true;

        m_staticInfosChanged[BaseInfo::InfoType::System] = false;
    }

#ifdef _WIN32
    if (m_staticInfosChanged[BaseInfo::InfoType::Wmi])
    {
        m_tabApiSupport->slotApiSupportStaticInfo(m_staticInfos[BaseInfo::InfoType::Wmi]);

        m_staticInfosChanged[BaseInfo::InfoType::Wmi] = false;
        processTabApiSupport = true;
    }

    if (m_dynamicInfosChanged[BaseInfo::InfoType::Wmi])
    {
        m_tabPerformance->slotNetworkDynamicInfo(m_dynamicInfos[BaseInfo::InfoType::Wmi]);

        m_dynamicInfosChanged[BaseInfo::InfoType::Wmi] = false;
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
