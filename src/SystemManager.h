#pragma once

#include "tabs/TabHardware.h"
#include "tabs/TabPerformance.h"
#include "tabs/TabProcesses.h"
#include "tabs/TabBenchmark.h"
#include "tabs/TabApiSupport.h"
#include "tabs/TabSystemInfo.h"
#include "Worker.h"

#include <QTabWidget>
#include <QThread>
#include <QTimer>

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
    TabSystemInfo* m_tabSystemInfo{nullptr};
    
    std::vector<QThread*> m_workerThreads;
    std::vector<Worker*> m_worker;

    std::unique_ptr<Worker> m_cpuWorker{nullptr};
    std::unique_ptr<Worker> m_gpuWorker{nullptr};
    std::unique_ptr<Worker> m_memoryWorker{ nullptr };
    std::unique_ptr<Worker> m_networkWorker{ nullptr };
    std::unique_ptr<Worker> m_processWorker{ nullptr };
    std::unique_ptr<Worker> m_systemWorker{nullptr};

#ifdef _WIN32
    std::unique_ptr<Worker> m_wmiWorker{ nullptr };
#endif

    QTimer* m_timer{ nullptr };

    QMap<BaseInfo::InfoType, QMap<uint8_t, QVariant>> m_staticInfos;
    QMap<BaseInfo::InfoType, QMap<uint8_t, QVariant>> m_dynamicInfos;

    QMap<BaseInfo::InfoType, bool> m_staticInfosChanged;
    QMap<BaseInfo::InfoType, bool> m_dynamicInfosChanged;
};

