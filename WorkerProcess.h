#pragma once

#include <QObject>
#include <QTimer>
#include <QMutex>

#include <vector>

#include "GPUInfo.h"
#include "CPUInfo.h"
#include "MemoryInfo.h"
#include "ProcessInfo.h"
#include "DevicesInfo.h"

class WorkerProcess : public QObject
{
    Q_OBJECT
public:
    explicit WorkerProcess(QObject *parent = nullptr);

public slots:
    void process();
    void start();

private:

    std::unique_ptr<GPUInfo> gpuInfo{nullptr};
    std::unique_ptr<CPUInfo> cpuInfo{nullptr};
    std::unique_ptr<MemoryInfo> memoryInfo{nullptr};
    std::unique_ptr<ProcessInfo> processInfo{nullptr};
    std::unique_ptr<DevicesInfo> devicesInfo{nullptr};

    QMutex mutex;
    QTimer* timer{nullptr};

signals:
    void signalStaticInfo_CPU(const CPUInfo::CPU_StaticInfo&);
    void signalStaticInfo_GPU(const GPUInfo::GPU_StaticInfo&);
    void signalStaticInfo_Memory(const MemoryInfo::Memory_StaticInfo&);
    void signalDynamicInfo_CPU(const CPUInfo::CPU_DynamicInfo&);
    void signalDynamicInfo_GPU(const GPUInfo::GPU_DynamicInfo&);
    void signalDynamicInfo_Memory(const MemoryInfo::Memory_DynamicInfo&);
    void signalDynamicInfo_Processes(const std::map<uint32_t, ProcessInfo::Process>&);

    void finished();
};

