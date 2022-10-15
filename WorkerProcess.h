#pragma once

#include <QObject>
#include <vector>
#include <map>
#include <QTimer>
#include <windows.h>

class WorkerProcess : public QObject
{
    Q_OBJECT
public:
    explicit WorkerProcess(QObject *parent = nullptr);

    struct UsageInfo
    {
        uint64_t totalPhysicalMemory;
        uint64_t usedPhysicalMemory;
        uint64_t totalVirtualMemory;
        uint64_t usedVirtualMemory;
    };

    struct ProcessInfo
    {
        uint32_t processID;
        QString description;
        QString baseName;
        QString filePath;

        uint64_t usedPhysicalMemory;
        uint64_t usedVirtualMemory;
    };

    void getProcessList(std::vector<WorkerProcess::ProcessInfo>& processList);

    BOOL EnumWindowsProc(HWND hwnd);
    static BOOL CALLBACK StaticEnumWindowsProc(HWND hwnd, LPARAM lParam);

public slots:
    void process();
    void start();

private:
    void getTotalUsage();
    void getProcessesUsage();
    void listRunningProcesses();

    std::map<uint32_t, ProcessInfo> processMap;
    UsageInfo totalUsageInfo;
    QTimer* timer;

signals:
    void finished();
    void receivedList();
    //void processAdded(QString& processBaseName, QString& processFilePath);
    //void processRemoved(QString& processBaseName);
};

