#pragma once

#include "TabHardware.h"
#include "TabPerformance.h"
#include "TabProcesses.h"
#include "TabBenchmark.h"

#include "Worker.h"

#include <QTabWidget>
#include <QThread>
#include <QTimer>

class ProcessWorker;
class CpuWorker;
class GpuWorker;
class MemoryWorker;
class WmiWorker;

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
    std::vector<QThread*> m_workerThreads;
    std::vector<Worker*> m_worker;

    std::unique_ptr<ProcessWorker> m_processWorker{ nullptr };
    std::unique_ptr<CpuWorker> m_cpuWorker{nullptr};
    std::unique_ptr<GpuWorker> m_gpuWorker{nullptr};
    std::unique_ptr<MemoryWorker> m_memoryWorker{ nullptr };
    std::unique_ptr<WmiWorker> m_wmiWorker{ nullptr };

    QTimer* m_timer{ nullptr };

    bool m_staticInfoCpuChanged{ false };
    bool m_dynamicInfoCpuChanged{ false };

    bool m_staticInfoGpuChanged{ false };
    bool m_dynamicInfoGpuChanged{ false };

    bool m_processMapChanged{ false };

    bool m_staticInfoMemoryChanged{ false };
    bool m_dynamicInfoMemoryChanged{ false };

    bool m_dynamicInfoWmiChanged{ false };

    QMap<uint8_t,QVariant> m_staticInfoCpu;
    QMap<uint8_t,QVariant> m_dynamicInfoCpu;

    QMap<uint8_t,QVariant> m_staticInfoGpu;
    QMap<uint8_t,QVariant> m_dynamicInfoGpu;

    std::map<uint32_t, ProcessInfo::Process> m_processMap;

    QMap<uint8_t,QVariant> m_staticInfoMemory;
    QMap<uint8_t,QVariant> m_dynamicInfoMemory;

    QMap<uint8_t, QVariant> m_dynamicInfoWmi;
};

