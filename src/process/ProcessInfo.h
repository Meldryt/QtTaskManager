#pragma once

#include "../main/BaseInfo.h"

#include <string>
#include <vector>
#include <map>

#ifdef _WIN32
#include <windows.h>
#else
#endif

#include <QElapsedTimer>

class ProcessInfo : public BaseInfo
{
public:
    struct ProcProcess
    {
        int pid{-1};
        int ppid{-1};
        std::string comm{""};
        char state{' '};
        double percent_cpu{0.0};
        double percent_mem{0.0};
        unsigned long used_memory{0};
        long resident{0};
        long virtualRam{0};
        unsigned long long processTime{0};
        unsigned long long starttime{0};
        double timestamp{0};

        ProcProcess() = default;
        ProcProcess(const ProcProcess& other)
        {
            this->pid = other.pid;
            this->ppid = other.ppid;
            this->comm = other.comm;
            this->state = other.state;
            this->percent_cpu = other.percent_cpu;
            this->percent_mem = other.percent_mem;
            this->used_memory = other.used_memory;
            this->resident = other.resident;
            this->virtualRam = other.virtualRam;
            this->processTime = other.processTime;
            this->starttime = other.starttime;
            this->timestamp = other.timestamp;
        };
        //ProcProcess(ProcProcess& other){};
    };

    struct Process
    {
        uint32_t processID{0};
        std::string processName{""};
        std::string description{""};
        std::string filePath{""};
        uint32_t frameIdx{0};
        char state{'R'};

        double ramUsagePercent{0.0};
        double cpuUsagePercent{0.0};
        double gpuUsagePercent{0.0};
        double videoRamUsagePercent{0.0};

        uint64_t ramUsageSize{0};
        uint64_t virtualRamUsageSize{0};
        uint64_t videoRamUsageSize{0};

        double cpuAverageUsagePercentSum{0.0};
        double cpuAverageUsagePercent{ 0.0 };
        double gpuAverageUsagePercentSum{0.0};
        double gpuAverageUsagePercent{ 0.0 };

        uint8_t cpuAverageUsageCounter{0};

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
        bool visited{false};
    };

    ProcessInfo();
    ~ProcessInfo();

    const std::map<uint32_t, Process> &processMap() const;

    virtual void init() override;
    virtual void update() override;

private:

#ifdef _WIN32
    BOOL EnumWindowsProc(HWND hwnd);
    static BOOL CALLBACK StaticEnumWindowsProc(HWND hwnd, LPARAM lParam);
#else
#endif


private:
    void readStaticInfo();
    void readDynamicInfo();

    void readCpuCount();

    void updateRunningProcesses();
    void updateProcessesUsage();
#ifdef _WIN32
    void updateProcessUsage(const HANDLE& processHandle, Process& processInfo);
#else
    void systemUpTime();
    void updateProcessUsage(ProcProcess& process);
    void scanProcessStats(const char* entryName, ProcProcess& process);
    void scanProcessMemoryStats(const char* entryName, ProcProcess& process);
    
    void scanCpuTime();
    void scanMemory();
#endif
    bool addProcess(const Process& processInfo);

#ifdef _WIN32
    bool addProcessModules(const std::string& processName, const HANDLE& processHandle);
#else
    bool addProcessModules(){return true;};
#endif
    void removeInactiveProcesses();

    std::map<uint32_t, Process> m_processMap;

    QElapsedTimer* m_elapsedTimer{nullptr};

    uint32_t m_cpuCoreCount{0};
    uint32_t m_currentFrameIdx{0};

#ifdef __linux__
    struct CPUData {
        unsigned long long int totalTime{0};
        unsigned long long int userTime{0};
        unsigned long long int systemTime{0};
        unsigned long long int systemAllTime{0};
        unsigned long long int idleAllTime{0};
        unsigned long long int idleTime{0};
        unsigned long long int niceTime{0};
        unsigned long long int ioWaitTime{0};
        unsigned long long int irqTime{0};
        unsigned long long int softIrqTime{0};
        unsigned long long int stealTime{0};
        unsigned long long int guestTime{0};

        unsigned long long int totalPeriod{0};
        unsigned long long int userPeriod{0};
        unsigned long long int systemPeriod{0};
        unsigned long long int systemAllPeriod{0};
        unsigned long long int idleAllPeriod{0};
        unsigned long long int idlePeriod{0};
        unsigned long long int nicePeriod{0};
        unsigned long long int ioWaitPeriod{0};
        unsigned long long int irqPeriod{0};
        unsigned long long int softIrqPeriod{0};
        unsigned long long int stealPeriod{0};
        unsigned long long int guestPeriod{0};

        double frequency{0.0};
    };

    std::vector<CPUData> m_cpuData;
    CPUData m_totalCpuData;

    double m_systemUptime{0.0};
    double m_systemDeltaTime{0.0};
    double m_currentPeriod{0.0};
    double m_deltaTime{0.0};
    double m_elapsedTime{0.0};

    std::map<uint32_t, ProcProcess> m_procProcessMap;

    unsigned long long int m_totalMem{0};
#endif
};

