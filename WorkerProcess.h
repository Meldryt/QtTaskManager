#pragma once

#include <QObject>
#include <vector>
#include <map>
#include <QTimer>
#include <windows.h>
#include <pdh.h>

class WorkerProcess : public QObject
{
    Q_OBJECT
public:
    explicit WorkerProcess(QObject *parent = nullptr);

    struct TotalInfo
    {
        uint64_t totalPhysicalMemory;
        uint64_t usedPhysicalMemory;
        uint64_t totalVirtualMemory;
        uint64_t usedVirtualMemory;

        double totalCPULoad;
        std::vector<double> singleCoreLoads;
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
    bool fillTotalInfo(TotalInfo& t);

    BOOL EnumWindowsProc(HWND hwnd);
    static BOOL CALLBACK StaticEnumWindowsProc(HWND hwnd, LPARAM lParam);

public slots:
    void process();
    void start();

private:
    void updateRunningProcesses();
    void updateTotalUsage();
    void updateProcessesUsage();
    void updateProcessUsage(const HANDLE& processHandle, ProcessInfo& processInfo);

    bool addProcess(const ProcessInfo& processInfo);
    bool addProcessModules(const QString& processName, const HANDLE& processHandle);

    std::map<uint32_t, ProcessInfo> processMap;
    std::vector<ProcessInfo> removedProcesses;
    TotalInfo totalInfo;
    QTimer* timer{nullptr};

    PDH_HQUERY totalCPUQuery;
    PDH_HCOUNTER totalCPUCounter;
    std::vector<PDH_HQUERY> singleCPUQueries;
    std::vector<PDH_HCOUNTER> singleCPUCounters;
//    ULARGE_INTEGER lastCPU{0};
//    ULARGE_INTEGER lastSysCPU{0};
//    ULARGE_INTEGER lastUserCPU{0};
    int numProcessors{0};


signals:
    void finished();
    void receivedProcessList();
    void receivedTotalInfo();
    //void processAdded(QString& processBaseName, QString& processFilePath);
    //void processRemoved(QString& processBaseName);
};

