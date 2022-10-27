#pragma once

#include "WorkerProcess.h"
#include <QObject>
#include <QThread>

class ProcessDatabase : public QObject
{
public:
    ProcessDatabase();
    const std::vector<WorkerProcess::ProcessInfo>& getProcessList();
    const WorkerProcess::TotalInfo& getTotalInfo();

    void setSortMode(int headerIndex);

    enum class SortMode : uint8_t
    {
        SortProcessName = 0,
        SortFileName,
        SortMemoryUsageHigh,
        SortMemoryUsageLow,
        SortCPUUsageHigh,
        SortCPUUsageLow,
        NoSort
    };

signals:
    //void receivedProcessList(const std::vector<WorkerProcess::ProcessInfo>& processList);

private:
    void sortTable();
    void sortNames();
    void sortFileNames();
    void sortMemoryUsage();
    void sortCPUUsage();

    WorkerProcess* workerProcess{nullptr};
    QThread* threadProcesses{nullptr};

    std::map<QString, int> processMap;
    std::vector<WorkerProcess::ProcessInfo> processList;
    WorkerProcess::TotalInfo totalInfo;

    SortMode sortMode{SortMode::NoSort};

private slots:
    void updateProcessList();
    void updateTotalInfo();
};

