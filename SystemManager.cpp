#include "SystemManager.h"

SystemManager::SystemManager(QWidget* parent) : QTabWidget(parent)
{
    tabHardware = new TabHardware(parent);
    tabProcesses = new TabProcesses(parent);
    tabPerformance = new TabPerformance(parent);
    addTab(tabProcesses, QString("Processes"));
    addTab(tabPerformance, QString("Performance"));
    addTab(tabHardware, QString("Hardware"));

    threadProcesses = new QThread();
    workerProcess = new WorkerProcess();
    workerProcess->moveToThread(threadProcesses);

    //connect( threadProcesses, &QThread::started, workerProcess, &WorkerProcess::process);
    connect( threadProcesses, &QThread::started, workerProcess, &WorkerProcess::start);
    connect( workerProcess, &WorkerProcess::finished, threadProcesses, &QThread::quit);
    connect( workerProcess, &WorkerProcess::finished, workerProcess, &WorkerProcess::deleteLater);
    connect( threadProcesses, &QThread::finished, threadProcesses, &QThread::deleteLater);

    connect( workerProcess, &WorkerProcess::signalStaticInfoCpu, this, [&](const CpuInfo::CpuStaticInfo& staticInfo)
    {
        tabHardware->slotCPUBrand(staticInfo.cpuBrand);
        tabHardware->slotProcessorCount(staticInfo.processorCount);
        tabHardware->slotThreadCount(staticInfo.threadCount);
        tabHardware->slotL1CacheSize(staticInfo.l1CacheSize);
        tabHardware->slotL2CacheSize(staticInfo.l2CacheSize);
        tabHardware->slotL3CacheSize(staticInfo.l3CacheSize);
    });
    connect( workerProcess, &WorkerProcess::signalStaticInfoGpu, this, [&](const GpuInfo::GpuStaticInfo& staticInfo)
    {
        tabHardware->slotGPUBrand(staticInfo.gpuBrand);
    });
    connect( workerProcess, &WorkerProcess::signalStaticInfoMemory, this, [&](const MemoryInfo::MemoryStaticInfo& staticInfo)
    {
        tabHardware->slotTotalPhysicalMemory(staticInfo.totalPhysicalMemory);
    });
    connect( workerProcess, &WorkerProcess::signalDynamicInfoCpu, this, [&](const CpuInfo::CpuDynamicInfo& dynamicInfo)
    {
        tabPerformance->slotCPUTotalLoad(dynamicInfo.cpuTotalLoad);

    });
    connect( workerProcess, &WorkerProcess::signalDynamicInfoGpu, this, [&](const GpuInfo::GpuDynamicInfo& dynamicInfo)
    {
        tabPerformance->slotGPUTotalLoad(dynamicInfo.gpuTotalLoad);
        tabPerformance->slotGPUTemperature(dynamicInfo.gpuTemperature);
    });
    connect( workerProcess, &WorkerProcess::signalDynamicInfoMemory, this, [&](const MemoryInfo::MemoryDynamicInfo& dynamicInfo)
    {
    });
    connect( workerProcess, &WorkerProcess::signalDynamicInfoProcesses, this, [&](const std::map<uint32_t, ProcessInfo::Process>& processMap)
    {
        tabProcesses->slotProcesses(processMap);
    });

    threadProcesses->start();
}
