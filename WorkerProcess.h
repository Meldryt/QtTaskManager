#pragma once

#include <QObject>
#include <QTimer>
#include <QMutex>

#include <vector>
#include <map>
#include <windows.h>
#include <pdh.h>

class WorkerProcess : public QObject
{
    Q_OBJECT
public:
    explicit WorkerProcess(QObject *parent = nullptr);

    struct StaticSystemInfo
    {
        uint64_t totalPhysicalMemory{0};
        uint64_t totalVirtualMemory{0};

        std::string cpuBrand;
        uint8_t processorCount{0};

        std::string gpuBrand;
    };

    struct DynamicSystemInfo
    {
        uint64_t usedPhysicalMemory{0};
        uint64_t usedVirtualMemory{0};

        double totalCPULoad{0.0};
        std::vector<double> singleCoreLoads;

        uint8_t totalGPULoad{0};
        uint8_t gpuTemperature{0};
    };

    struct ProcessInfo
    {
        uint32_t processID{0};
        QString description;
        QString baseName;
        QString filePath;

        uint64_t usedPhysicalMemory{0};
        uint64_t usedVirtualMemory{0};
        double usedCPULoad{0.0};
        double usedCPULoadSum{0.0};
        uint8_t usedCPULoadCounter{0};

        ULARGE_INTEGER lastCPU{0};
        ULARGE_INTEGER lastSysCPU{0};
        ULARGE_INTEGER lastUserCPU{0};
    };

    bool fillProcessList(std::vector<WorkerProcess::ProcessInfo>& processList);
    bool fillStaticSystemInfo(StaticSystemInfo& info);
    bool fillDynamicSystemInfo(DynamicSystemInfo& info);

    BOOL EnumWindowsProc(HWND hwnd);
    static BOOL CALLBACK StaticEnumWindowsProc(HWND hwnd, LPARAM lParam);

public slots:
    void process();
    void start();

private:
    void collectStaticSystemInfo();
    void collectDynamicSystemInfo();
    void collectCpuInfo();
    bool detectNvidiaGPU();
    void collectNvidiaGpuInfo();
    void collectMemoryInfo();

    void updateRunningProcesses();
    void updateTotalUsage();
    void updateProcessesUsage();
    void updateProcessUsage(const HANDLE& processHandle, ProcessInfo& processInfo);

    bool addProcess(const ProcessInfo& processInfo);
    bool addProcessModules(const QString& processName, const HANDLE& processHandle);

    std::map<uint32_t, ProcessInfo> processMap;
    std::vector<ProcessInfo> removedProcesses;
    StaticSystemInfo staticSystemInfo;
    DynamicSystemInfo dynamicSystemInfo;

    PDH_HQUERY totalCPUQuery;
    PDH_HCOUNTER totalCPUCounter;
    std::vector<PDH_HQUERY> singleCPUQueries;
    std::vector<PDH_HCOUNTER> singleCPUCounters;
//    ULARGE_INTEGER lastCPU{0};
//    ULARGE_INTEGER lastSysCPU{0};
//    ULARGE_INTEGER lastUserCPU{0};

    bool gpuDetected{false};
    QMutex mutex;
    QTimer* timer{nullptr};

signals:
    void finished();
    void receivedProcessList();
    void receivedTotalInfo();
    //void processAdded(QString& processBaseName, QString& processFilePath);
    //void processRemoved(QString& processBaseName);
};

