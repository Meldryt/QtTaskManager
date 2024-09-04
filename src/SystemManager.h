#pragma once

#include "tabs/TabHardware.h"
#include "tabs/TabPerformance.h"
#include "tabs/TabProcesses.h"
#include "tabs/TabBenchmark.h"
#include "tabs/TabApiSupport.h"

#include "Worker.h"

#include <QTabWidget>
#include <QThread>
#include <QTimer>

class ProcessWorker;
class CpuWorker;
class GpuWorker;
class MemoryWorker;

#ifdef _WIN32
class WmiWorker;
#endif

class SystemManager : public QTabWidget
{
    Q_OBJECT
public:
    SystemManager(QWidget* parent);
    ~SystemManager();

private:
    void update();

    TabHardware* m_tabHardware{nullptr};
    TabProcesses* m_tabProcesses{nullptr};
    TabPerformance* m_tabPerformance{nullptr};
    TabBenchmark* m_tabBenchmark{ nullptr };
    TabApiSupport* m_tabApiSupport{ nullptr };

    std::vector<QThread*> m_workerThreads;
    std::vector<Worker*> m_worker;

    std::unique_ptr<ProcessWorker> m_processWorker{ nullptr };
    std::unique_ptr<CpuWorker> m_cpuWorker{nullptr};
    std::unique_ptr<GpuWorker> m_gpuWorker{nullptr};
    std::unique_ptr<MemoryWorker> m_memoryWorker{ nullptr };

#ifdef _WIN32
    std::unique_ptr<WmiWorker> m_wmiWorker{ nullptr };
#endif

    QTimer* m_timer{ nullptr };

    bool m_staticInfoCpuChanged{ false };
    bool m_dynamicInfoCpuChanged{ false };

    bool m_staticInfoGpuChanged{ false };
    bool m_dynamicInfoGpuChanged{ false };

    bool m_processMapChanged{ false };

    bool m_staticInfoMemoryChanged{ false };
    bool m_dynamicInfoMemoryChanged{ false };

#ifdef _WIN32
    bool m_staticInfoWmiChanged{ false };
    bool m_dynamicInfoWmiChanged{ false };
#endif
    QMap<uint8_t,QVariant> m_staticInfoCpu;
    QMap<uint8_t,QVariant> m_dynamicInfoCpu;

    QMap<uint8_t,QVariant> m_staticInfoGpu;
    QMap<uint8_t,QVariant> m_dynamicInfoGpu;

    std::map<uint32_t, ProcessInfo::Process> m_processMap;

    QMap<uint8_t,QVariant> m_staticInfoMemory;
    QMap<uint8_t,QVariant> m_dynamicInfoMemory;

#ifdef _WIN32
    QMap<uint8_t, QVariant> m_staticInfoWmi;
    QMap<uint8_t, QVariant> m_dynamicInfoWmi;
#endif
};

