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
        double usedCpuLoad{0.0};
        double usedCpuLoadSum{0.0};
        uint8_t usedCpuLoadCounter{0};

#ifdef _WIN32
        ULARGE_INTEGER lastCpu{0};
        ULARGE_INTEGER lastSysCpu{0};
        ULARGE_INTEGER lastUserCpu{0};
#else
        uint32_t lastCPU{0};
        uint32_t lastSysCPU{0};
        uint32_t lastUserCPU{0};
#endif

        int64_t timestamp{0};
    };

    ProcessInfo();
    ~ProcessInfo();

    void setCoreCount(uint16_t newProcessorCount);
    const std::map<uint32_t, Process> &getProcessMap() const;

    void update();

private:

#ifdef _WIN32
    BOOL EnumWindowsProc(HWND hwnd);
    static BOOL CALLBACK StaticEnumWindowsProc(HWND hwnd, LPARAM lParam);
#else
#endif


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

    QElapsedTimer* m_elapsedTimer{nullptr};

    uint16_t m_cpuCoreCount{0};
};

