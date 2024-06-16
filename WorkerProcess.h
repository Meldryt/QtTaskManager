#pragma once

#include <QObject>
#include <QTimer>
#include <QMutex>

#include <vector>

#include "GpuInfo.h"
#include "CpuInfo.h"
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

    std::unique_ptr<GpuInfo> m_gpuInfo{nullptr};
    std::unique_ptr<CpuInfo> m_cpuInfo{nullptr};
    std::unique_ptr<MemoryInfo> memoryInfo{nullptr};
    std::unique_ptr<ProcessInfo> processInfo{nullptr};
    std::unique_ptr<DevicesInfo> devicesInfo{nullptr};

    QMutex mutex;
    QTimer* timer{nullptr};

signals:
    void signalStaticInfoCpu(const CpuInfo::CpuStaticInfo&);
    void signalStaticInfoGpu(const Globals::GpuStaticInfo&);
    void signalStaticInfoMemory(const MemoryInfo::MemoryStaticInfo&);
    void signalDynamicInfoCpu(const CpuInfo::CpuDynamicInfo&);
    void signalDynamicInfoGpu(const Globals::GpuDynamicInfo&);
    void signalDynamicInfoMemory(const MemoryInfo::MemoryDynamicInfo&);
    void signalDynamicInfoProcesses(const std::map<uint32_t, ProcessInfo::Process>&);

    void finished();
};

