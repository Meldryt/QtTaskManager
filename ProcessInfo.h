#pragma once

#include <string>
#include <vector>
#include <map>
#include <windows.h>

#include <QElapsedTimer>

class ProcessInfo
{
public:
    struct Process
    {
        uint32_t processID{0};
        std::string description{""};
        std::string baseName{""};
        std::string filePath{""};

        uint64_t usedPhysicalMemory{0};
        uint64_t usedVirtualMemory{0};
        double usedCPULoad{0.0};
        double usedCPULoadSum{0.0};
        uint8_t usedCPULoadCounter{0};

        ULARGE_INTEGER lastCPU{0};
        ULARGE_INTEGER lastSysCPU{0};
        ULARGE_INTEGER lastUserCPU{0};

        int64_t timestamp{0};
    };

    ProcessInfo();

    void update();

    BOOL EnumWindowsProc(HWND hwnd);
    static BOOL CALLBACK StaticEnumWindowsProc(HWND hwnd, LPARAM lParam);

    void setProcessorCount(uint8_t newProcessorCount);

    const std::map<uint32_t, Process> &getProcessMap() const;

private:

    void updateRunningProcesses();
    void updateProcessesUsage();
    void updateProcessUsage(const HANDLE& processHandle, Process& processInfo);

    bool addProcess(const Process& processInfo);
    bool addProcessModules(const std::string& processName, const HANDLE& processHandle);

    std::map<uint32_t, Process> processMap;

    QElapsedTimer* elapsedTimer{nullptr};

    uint8_t processorCount{0};
};

