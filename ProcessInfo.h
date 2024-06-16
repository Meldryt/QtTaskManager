#pragma once

#include <string>
#include <vector>
#include <map>

#ifdef _WIN32
#include <windows.h>
#else
#endif

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

#ifdef _WIN32
        ULARGE_INTEGER lastCPU{0};
        ULARGE_INTEGER lastSysCPU{0};
        ULARGE_INTEGER lastUserCPU{0};
#else
        uint32_t lastCPU{0};
        uint32_t lastSysCPU{0};
        uint32_t lastUserCPU{0};
#endif

        int64_t timestamp{0};
    };

    ProcessInfo();

    void update();

#ifdef _WIN32
    BOOL EnumWindowsProc(HWND hwnd);
    static BOOL CALLBACK StaticEnumWindowsProc(HWND hwnd, LPARAM lParam);
#else
#endif
    void setProcessorCount(uint8_t newProcessorCount);

    const std::map<uint32_t, Process> &getProcessMap() const;

private:

    void updateRunningProcesses();
    void updateProcessesUsage();
#ifdef _WIN32
    void updateProcessUsage(const HANDLE& processHandle, Process& processInfo);
#else
    void updateProcessUsage(){};
#endif
    bool addProcess(const Process& processInfo);

#ifdef _WIN32
    bool addProcessModules(const std::string& processName, const HANDLE& processHandle);
#else
    bool addProcessModules(){return true;};
#endif

    std::map<uint32_t, Process> m_processMap;

    QElapsedTimer* elapsedTimer{nullptr};

    uint8_t processorCount{0};
};

