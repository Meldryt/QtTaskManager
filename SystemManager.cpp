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

    connect( workerProcess, &WorkerProcess::signalStaticInfo_CPU, this, [&](const CPUInfo::CPU_StaticInfo& staticInfo)
    {
        tabHardware->slotCPUBrand(staticInfo.cpuBrand);
        tabHardware->slotProcessorCount(staticInfo.processorCount);
        tabHardware->slotThreadCount(staticInfo.threadCount);
        tabHardware->slotL1CacheSize(staticInfo.l1CacheSize);
        tabHardware->slotL2CacheSize(staticInfo.l2CacheSize);
        tabHardware->slotL3CacheSize(staticInfo.l3CacheSize);
    });
    connect( workerProcess, &WorkerProcess::signalStaticInfo_GPU, this, [&](const GPUInfo::GPU_StaticInfo& staticInfo)
    {
        tabHardware->slotGPUBrand(staticInfo.gpuBrand);
    });
    connect( workerProcess, &WorkerProcess::signalStaticInfo_Memory, this, [&](const MemoryInfo::Memory_StaticInfo& staticInfo)
    {
        tabHardware->slotTotalPhysicalMemory(staticInfo.totalPhysicalMemory);
    });
    connect( workerProcess, &WorkerProcess::signalDynamicInfo_CPU, this, [&](const CPUInfo::CPU_DynamicInfo& dynamicInfo)
    {
        tabPerformance->slotCPUTotalLoad(dynamicInfo.cpuTotalLoad);

    });
    connect( workerProcess, &WorkerProcess::signalDynamicInfo_GPU, this, [&](const GPUInfo::GPU_DynamicInfo& dynamicInfo)
    {
        tabPerformance->slotGPUTotalLoad(dynamicInfo.gpuTotalLoad);
        tabPerformance->slotGPUTemperature(dynamicInfo.gpuTemperature);
    });
    connect( workerProcess, &WorkerProcess::signalDynamicInfo_Memory, this, [&](const MemoryInfo::Memory_DynamicInfo& dynamicInfo)
    {
    });
    connect( workerProcess, &WorkerProcess::signalDynamicInfo_Processes, this, [&](const std::map<uint32_t, ProcessInfo::Process>& processMap)
    {
        tabProcesses->slotProcesses(processMap);
    });

    threadProcesses->start();
}
