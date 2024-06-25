#pragma once

#include "TabHardware.h"
#include "TabPerformance.h"
#include "TabProcesses.h"
#include "Worker.h"

#include <QTabWidget>
#include <QThread>
#include <QTimer>
#include <QDockWidget>

class ProcessWorker;
class CpuWorker;
class GpuWorker;
class MemoryWorker;

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
    QDockWidget* m_dockWidget{ nullptr };
    std::vector<QThread*> m_workerThreads;
    std::vector<Worker*> m_worker;

    std::unique_ptr<ProcessWorker> m_processWorker{ nullptr };
    std::unique_ptr<CpuWorker> m_cpuWorker{nullptr};
    std::unique_ptr<GpuWorker> m_gpuWorker{nullptr};
    std::unique_ptr<MemoryWorker> m_memoryWorker{ nullptr };

    QTimer* m_timer{ nullptr };

    bool m_staticInfoCpuChanged{ false };
    bool m_dynamicInfoCpuChanged{ false };

    bool m_staticInfoGpuChanged{ false };
    bool m_dynamicInfoGpuChanged{ false };

    bool m_processMapChanged{ false };

    bool m_staticInfoMemoryChanged{ false };
    bool m_dynamicInfoMemoryChanged{ false };

    Globals::CpuStaticInfo m_staticInfoCpu;
    Globals::CpuDynamicInfo m_dynamicInfoCpu;

    Globals::GpuStaticInfo m_staticInfoGpu;
    Globals::GpuDynamicInfo m_dynamicInfoGpu;

    std::map<uint32_t, ProcessInfo::Process> m_processMap;

    Globals::MemoryStaticInfo m_staticInfoMemory;
    Globals::MemoryDynamicInfo m_dynamicInfoMemory;
};

