#include "ProcessInfo.h"

#include "../Globals.h"

#include <cmath>

#ifdef _WIN32
#include <psapi.h>
#include <system_error>
#elif __linux__
#include <math.h>
#include <string>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctype.h>
#endif

#include <QDebug>

#ifdef __linux__

#define PROC_LINE_LENGTH 4096

static inline unsigned long long LinuxProcess_adjustTime(unsigned long long t) {
    static double jiffy = NAN;
    if(isnan(jiffy)) {
        errno = 0;
        long sc_jiffy = sysconf(_SC_CLK_TCK);
        if(errno || -1 == sc_jiffy) {
            jiffy = NAN;
            return t; // Assume 100Hz clock
        }
        jiffy = sc_jiffy;
    }
    double jiffytime = 1.0 / jiffy;
    return t * jiffytime * 100;
}
/*
 * String_startsWith gives better performance if strlen(match) can be computed
 * at compile time (e.g. when they are immutable string literals). :)
 */
static inline bool String_startsWith(const char* s, const char* match) {
    return strncmp(s, match, strlen(match)) == 0;
}

#endif

ProcessInfo::ProcessInfo() : BaseInfo("ProcessInfo", InfoType::Process)
{
    qDebug() << __FUNCTION__;

    m_elapsedTimer = new QElapsedTimer();
    m_elapsedTimer->start();
}

ProcessInfo::~ProcessInfo()
{
    qDebug() << __FUNCTION__;
}

const std::map<uint32_t, ProcessInfo::Process> &ProcessInfo::processMap() const
{
    return m_processMap;
}

void ProcessInfo::init()
{
    readCpuCount();
#ifdef __linux__
    m_cpuData.resize(m_cpuCoreCount);
    for (int i = 0; i < m_cpuCoreCount; i++) {
        m_cpuData[i].totalTime = 1;
        m_cpuData[i].totalPeriod = 1;
    }
    m_totalCpuData.totalTime = 1;
    m_totalCpuData.totalPeriod = 1;

#endif
}

void ProcessInfo::update()
{
    //m_elapsedTime = m_elapsedTimer->elapsed() * 0.001;
    //Enumerates all top-level windows on the screen by passing the handle to each window, in turn,
    //to an application-defined callback function. EnumWindows continues until the last top-level
    //window is enumerated or the callback function returns FALSE.
#ifdef _WIN32
    EnumWindows(StaticEnumWindowsProc, reinterpret_cast<LPARAM>(this));

    updateRunningProcesses();
#else
    scanMemory();
    systemUpTime();
    scanCpuTime();
    updateRunningProcesses();

    qDebug() << "number of processes: " << m_procProcessMap.size();
#endif

//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_Names, );
//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_Descriptions, );
//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_FilePaths, );
//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_States, );
//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_RamUsagesPercent, );
//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_CpuUsagesPercent, );
//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_GpuUsagesPercent, );
//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_VideoRamUsagesPercent, );
//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_RamUsagesSize, );
//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_VirtualRamUsagesSize, );
//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_VideoRamUsagesSize, );
//    setDynamicInfo(Globals::SysInfoAttr::Key_Process_Dynamic_TimeStamps, );
    //std::map<uint32_t, std::string> names;
    QVariant variant = QVariant::fromValue(m_processMap);
    setDynamicValue(Globals::SysInfoAttr::Key_Process_Dynamic_Info, variant);

    ++m_currentFrameIdx;
}

#ifdef _WIN32
void ProcessInfo::updateRunningProcesses()
{
    auto process = std::begin(m_processMap);
    while (process != std::end(m_processMap))
    {
        HANDLE processHandle = OpenProcess(SYNCHRONIZE, FALSE, process->second.processID);
        DWORD ret = WaitForSingleObject(processHandle, 0);

        if (!processHandle || (ret != WAIT_TIMEOUT))
        {
            //qDebug() << "remove process " << process->second.description;
            process = m_processMap.erase(process);
        }
        else
        {
            ++process;
        }

        if(processHandle)
        {
            CloseHandle(processHandle);
        }
    }

    //updateTotalUsage();
    updateProcessesUsage();
}
#elif __linux__
void ProcessInfo::updateRunningProcesses()
{
    DIR* procdir = opendir("/proc");
    if (!procdir)
    {
        perror("opendir failed");
        return;
    }

    struct dirent *entry;
    int64_t currentTime = int64_t(m_elapsedTimer->elapsed() * 0.001);

    while ((entry = readdir(procdir)))
    {
        char exePath[256 + 10];
        char statPath[256 + 10];
        std::string processName;
        FILE* file;

        // Skip anything that is not a PID directory.
        if(!isdigit(*entry->d_name))
        {
            continue;
        }

        // Try to open /proc/<PID>/stat.
        ProcProcess procProcess;
        scanProcessStats(entry->d_name, procProcess);

        if(procProcess.pid <= 0 || procProcess.pid == procProcess.ppid)
        {
            continue;
        }

        scanProcessMemoryStats(entry->d_name, procProcess);

        processName = procProcess.comm;
        processName.erase(std::remove_if(processName.begin(), processName.end(), [](unsigned char x) { return (x == ')') || (x == '('); }), processName.end());

        bool invalidStates = (procProcess.state != 'R' && procProcess.state != 'S' && procProcess.state != 'D' && procProcess.state != 'I');

        //check if process is running
        if(invalidStates)
        {
            continue;
        }

        // Try to open /proc/<PID>/exe.
        snprintf(exePath, sizeof(exePath), "/proc/%s/exe", entry->d_name);
        file = fopen(exePath, "r");
        if (file) {
            fclose(file);
        }

        char buff[PATH_MAX];
        int len = ::readlink(exePath, buff, sizeof(buff)-1);
        std::string exeRealPath;
        if (len != -1) {
            buff[len] = '\0';
            exeRealPath = std::string(buff);
        }

        Process process;
        process.processID = procProcess.pid;
        process.processName = processName;
        process.description = procProcess.state;
        process.filePath = exeRealPath;
        process.frameIdx = m_currentFrameIdx;
        process.timestamp = currentTime;
        process.state = procProcess.state;

        updateProcessUsage(procProcess);

        process.ramUsagePercent = procProcess.percent_mem;
        process.cpuUsagePercent = procProcess.percent_cpu;

        process.ramUsageSize = procProcess.used_memory;
        process.virtualRamUsageSize = procProcess.virtualRam;

        std::map<uint32_t, Process>::iterator itProcessMap = m_processMap.find(process.processID);
        if(itProcessMap == m_processMap.end() )
        {
            m_processMap.insert({process.processID,process});
        }
        else
        {
            itProcessMap->second = process;
        }

        std::map<uint32_t, ProcProcess>::iterator itProcProcessMap = m_procProcessMap.find(procProcess.pid);
        if(itProcProcessMap == m_procProcessMap.end() )
        {
            m_procProcessMap.insert({procProcess.pid,procProcess});
        }
        else
        {
            itProcProcessMap->second = procProcess;
        }
    }

    closedir(procdir);

    removeInactiveProcesses();

    //updateTotalUsage();
    //updateProcessesUsage();
}
#endif

void ProcessInfo::readCpuCount()
{
#ifdef __linux__
    FILE* file;
    char statPath[256 + 10];
    snprintf(statPath, sizeof(statPath), "/proc/stat");
    file = fopen(statPath, "r");
    if (!file) {
        //perror(statPath);
        return;
    }

    int cpus = 0;
    char buffer[PROC_LINE_LENGTH + 1];
    while(fgets(buffer, sizeof(buffer), file)) {
        if (String_startsWith(buffer, "cpu"))
            cpus++;
    }

    fclose(file);

    /* subtract raw cpu entry */
    if (cpus > 0)
        cpus--;

    m_cpuCoreCount = cpus;
#elif _WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    m_cpuCoreCount = sysInfo.dwNumberOfProcessors;
#endif
}

#ifdef __linux__
void ProcessInfo::scanProcessStats(const char* entryName, ProcProcess& process)
{
    FILE* file;
    char statPath[256 + 10];

    // Try to open /proc/<PID>/stat.
    snprintf(statPath, sizeof(statPath), "/proc/%s/stat", entryName);
    file = fopen(statPath, "r");
    if (!file) {
        //perror(statPath);
        return;
    }

    int pid, ppid, pgrp, session, tty_nr, tpgid, exit_signal, processor, exit_code;
    char comm[_POSIX_PATH_MAX];
    char state;
    unsigned int flags, rt_priority, policy;
    long unsigned int minflt, cminflt, majflt, cmajflt, cutime, cstime, cgtime, vsize, rlim = 0,
        startcode, endcode, startstack, kstkesp, kstkeip, signal, blocked, sigignore,
        sigcatch, wchan, nswap, cnswap, rsslim, guest_time, cguest_time, start_data,
        end_data, start_brk, arg_start, arg_end, env_start, env_end;
    long int priority, nice, unused, itrealvalue, rss, num_threads;
    unsigned long long utime, stime;
    unsigned long long starttime, delayacct_blkio_ticks;

    fscanf(file, "%d %s %s %d %d %d %d %d %u %lu %lu %lu %lu %llu %llu %ld %ld %ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu %d",
           &pid, (char*)&comm, (char*)&state, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags, &minflt,
           &cminflt, &majflt, &cmajflt, &utime, &stime, &cutime, &cstime, &priority, &nice, &num_threads,
           &itrealvalue, &starttime, &vsize, &rss, &rsslim, &startcode, &endcode, &startstack, &kstkesp,
           &kstkeip, &signal, &blocked, &sigignore, &sigcatch, &wchan, &nswap, &cnswap, &exit_signal, &processor,
           &rt_priority, &policy, &delayacct_blkio_ticks, &guest_time, &cguest_time, &start_data, &end_data, &start_brk,
           &arg_start, &arg_end, &env_start, &env_end, &exit_code);
    fclose(file);

    const int tgid = strtol(entryName, NULL, 10);
    process.pid = tgid; //pid
    process.ppid = ppid;
    process.comm = comm;
    process.state = state;
    process.processTime = LinuxProcess_adjustTime(utime+stime);
    process.starttime = starttime;
    process.timestamp = m_systemUptime;
}

void ProcessInfo::scanProcessMemoryStats(const char* entryName, ProcProcess& process)
{
    FILE* file;
    char statPath[256 + 10];

    // Try to open /proc/<PID>/stat.
    snprintf(statPath, sizeof(statPath), "/proc/%s/statm", entryName);
    file = fopen(statPath, "r");
    if (!file) {
        //perror(statPath);
        return;
    }

    long size, resident, shared, text, lib, data, dt;

    int result = fscanf(file, "%ld %ld %ld %ld %ld %ld %ld",
           &size, &resident, &shared, &text, &lib, &data, &dt);
    if(result != 7)
    {

    }
    fclose(file);

    process.resident = resident;
    process.virtualRam = size;
}

void ProcessInfo::systemUpTime()
{
    float uptime = 0;
    float idletime = 0;
    FILE* file = fopen("/proc/uptime", "r");
    if (file) {
        char line[256];
        float up_time, up_time2;

        int n = fscanf(file, "%f %f", &uptime, &idletime);
        fclose(file);
        if (n <= 0)
        {
            m_systemUptime = 0;
            return;
        }
    }

    const double oldSystemUptime = m_systemUptime;
    m_systemUptime = uptime;// / sysconf(_SC_CLK_TCK);
    m_systemDeltaTime = m_systemUptime - oldSystemUptime;
}

void ProcessInfo::updateProcessUsage(ProcProcess& process)
{
    process.percent_cpu = 0.0;

    std::map<uint32_t, ProcProcess>::iterator itProcProcessMap = m_procProcessMap.find(process.pid);
    if(itProcProcessMap != m_procProcessMap.end() )
    {
        const unsigned long long lasttimes = itProcProcessMap->second.processTime;
        const unsigned long long processTime = process.processTime;
        const unsigned long long deltaProcessTime = (processTime - lasttimes);
        if(lasttimes > 0 && processTime > lasttimes)
        {
            double percent_cpu = (100.0 * deltaProcessTime) / m_currentPeriod;
            process.percent_cpu = std::clamp(percent_cpu, 0.0, m_cpuCoreCount * 100.0);
        }
    }

    long CRT_pageSize = -1;
    long CRT_pageSizeKB = -1;
    CRT_pageSize = sysconf(_SC_PAGESIZE);
    if (CRT_pageSize == -1)
    {
        qWarning() << __FUNCTION__ << " Fatal error: Can not get PAGE_SIZE by sysconf(_SC_PAGESIZE)";
    }
    CRT_pageSizeKB = CRT_pageSize / 1024;

    process.used_memory = process.resident * CRT_pageSizeKB;
    process.virtualRam = process.virtualRam * CRT_pageSizeKB;
    process.percent_mem = process.used_memory / (double)(m_totalMem) * 100.0;
}

void ProcessInfo::scanCpuTime()
{
    FILE* file;
    char statPath[256 + 10];
    snprintf(statPath, sizeof(statPath), "/proc/stat");
    file = fopen(statPath, "r");
    if (!file) {
        //perror(statPath);
        return;
    }

    for (int i = 0; i <= m_cpuCoreCount; ++i) {
        char buffer[PROC_LINE_LENGTH + 1];
        unsigned long long int usertime, nicetime, systemtime, idletime;
        unsigned long long int ioWait, irq, softIrq, steal, guest, guestnice;
        ioWait = irq = softIrq = steal = guest = guestnice = 0;
        // Depending on your kernel version,
        // 5, 7, 8 or 9 of these fields will be set.
        // The rest will remain at zero.
        char* ok = fgets(buffer, PROC_LINE_LENGTH, file);
        int cpuid{-1};
        if (!ok) buffer[0] = '\0';
        if (i == 0)
            (void) sscanf(buffer,   "cpu  %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu",         &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
        else {
            (void) sscanf(buffer, "cpu%4d %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu %16llu", &cpuid, &usertime, &nicetime, &systemtime, &idletime, &ioWait, &irq, &softIrq, &steal, &guest, &guestnice);
            assert(cpuid == i - 1);
        }
        // Guest time is already accounted in usertime
        usertime = usertime - guest;
        nicetime = nicetime - guestnice;
        // Fields existing on kernels >= 2.6
        // (and RHEL's patched kernel 2.4...)
        unsigned long long int idlealltime = idletime + ioWait;
        unsigned long long int systemalltime = systemtime + irq + softIrq;
        unsigned long long int virtalltime = guest + guestnice;
        unsigned long long int totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;

        CPUData& cpuData = cpuid == -1? m_totalCpuData : m_cpuData[cpuid];
// Since we do a subtraction (usertime - guest) and cputime64_to_clock_t()
// used in /proc/stat rounds down numbers, it can lead to a case where the
// integer overflow.
#define WRAP_SUBTRACT(a,b) (((a) > (b)) ? (a) - (b) : 0)
        cpuData.userPeriod = WRAP_SUBTRACT(usertime,cpuData.userTime);
        cpuData.nicePeriod = WRAP_SUBTRACT(nicetime, cpuData.niceTime);
        cpuData.systemPeriod = WRAP_SUBTRACT(systemtime, cpuData.systemTime);
        cpuData.systemAllPeriod = WRAP_SUBTRACT(systemalltime, cpuData.systemAllTime);
        cpuData.idleAllPeriod = WRAP_SUBTRACT(idlealltime, cpuData.idleAllTime);
        cpuData.idlePeriod = WRAP_SUBTRACT(idletime, cpuData.idleTime);
        cpuData.ioWaitPeriod = WRAP_SUBTRACT(ioWait, cpuData.ioWaitTime);
        cpuData.irqPeriod = WRAP_SUBTRACT(irq, cpuData.irqTime);
        cpuData.softIrqPeriod = WRAP_SUBTRACT(softIrq, cpuData.softIrqTime);
        cpuData.stealPeriod = WRAP_SUBTRACT(steal, cpuData.stealTime);
        cpuData.guestPeriod = WRAP_SUBTRACT(virtalltime, cpuData.guestTime);
        cpuData.totalPeriod = WRAP_SUBTRACT(totaltime, cpuData.totalTime);
#undef WRAP_SUBTRACT
        cpuData.userTime = usertime;
        cpuData.niceTime = nicetime;
        cpuData.systemTime = systemtime;
        cpuData.systemAllTime = systemalltime;
        cpuData.idleAllTime = idlealltime;
        cpuData.idleTime = idletime;
        cpuData.ioWaitTime = ioWait;
        cpuData.irqTime = irq;
        cpuData.softIrqTime = softIrq;
        cpuData.stealTime = steal;
        cpuData.guestTime = virtalltime;
        cpuData.totalTime = totaltime;
    }
    fclose(file);

    m_currentPeriod = m_totalCpuData.totalPeriod / (double)m_cpuCoreCount;
}

void ProcessInfo::scanMemory()
{
    unsigned long long int swapFree = 0;
    unsigned long long int shmem = 0;
    unsigned long long int sreclaimable = 0;

    unsigned long long int totalMem;
    unsigned long long int usedMem;
    unsigned long long int freeMem;
    unsigned long long int sharedMem;
    unsigned long long int buffersMem;
    unsigned long long int cachedMem;
    unsigned long long int totalSwap;
    unsigned long long int usedSwap;
    unsigned long long int freeSwap;

    FILE* file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        return;
        //CRT_fatalError("Cannot open " "/proc/meminfo");
    }
    char buffer[128];
    while (fgets(buffer, 128, file)) {
        if(String_startsWith(buffer, "MemTotal:"))
        {
            sscanf(buffer + strlen("MemTotal:"), " %32llu kB", &totalMem);
        }
        else if(String_startsWith(buffer, "MemFree:"))
        {
            sscanf(buffer + strlen("MemFree:"), " %32llu kB", &freeMem);
        }
        else if(String_startsWith(buffer, "MemShared:"))
        {
            sscanf(buffer + strlen("MemShared:"), " %32llu kB", &sharedMem);
        }
        else if(String_startsWith(buffer, "Buffers:"))
        {
            sscanf(buffer + strlen("Buffers:"), " %32llu kB", &buffersMem);
        }
        else if(String_startsWith(buffer, "Cached:"))
        {
            sscanf(buffer + strlen("Cached:"), " %32llu kB", &cachedMem);
        }
        else if(String_startsWith(buffer, "SwapTotal:"))
        {
            sscanf(buffer + strlen("SwapTotal:"), " %32llu kB", &totalSwap);
        }
        else if(String_startsWith(buffer, "SwapFree:"))
        {
            sscanf(buffer + strlen("SwapFree:"), " %32llu kB", &swapFree);
        }
        else if(String_startsWith(buffer, "Shmem:"))
        {
            sscanf(buffer + strlen("Shmem:"), " %32llu kB", &shmem);
        }
        else if(String_startsWith(buffer, "SReclaimable:"))
        {
            sscanf(buffer + strlen("SReclaimable:"), " %32llu kB", &sreclaimable);
        }
    }
    fclose(file);

    usedMem = totalMem - freeMem;
    cachedMem = cachedMem + sreclaimable - shmem;
    usedSwap = totalSwap - swapFree;

    m_totalMem = totalMem;
}

void ProcessInfo::removeInactiveProcesses()
{
    int64_t currentTime = int64_t(m_elapsedTimer->elapsed() * 0.001);

    auto process = std::begin(m_processMap);
    while (process != std::end(m_processMap))
    {
        if (process->second.timestamp + 10 < currentTime)
        {
            process = m_processMap.erase(process);
        }
        else
        {
            ++process;
        }
    }

    auto procProcess = std::begin(m_procProcessMap);
    while (procProcess != std::end(m_procProcessMap))
    {
        if (procProcess->second.timestamp + 10 < currentTime)
        {
            procProcess = m_procProcessMap.erase(procProcess);
        }
        else
        {
            ++procProcess;
        }
    }
}
#endif


#ifdef _WIN32
void ProcessInfo::updateProcessesUsage()
{
    auto process = std::begin(m_processMap);
    while (process != std::end(m_processMap))
    {
        HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process->first);
        if(processHandle == NULL)
        {
            continue;
        }

        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(processHandle, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

        process->second.virtualRamUsageSize = pmc.PrivateUsage;
        process->second.ramUsageSize = pmc.WorkingSetSize;
        process->second.timestamp = int64_t(m_elapsedTimer->elapsed() * 0.001);
        updateProcessUsage(processHandle, process->second);

        CloseHandle(processHandle);

        ++process;
    }
}

void ProcessInfo::updateProcessUsage(const HANDLE& processHandle, Process& processInfo)
{
    if (m_cpuCoreCount == 0)
    {
        return;
    }

    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(processHandle, &ftime, &ftime, &fsys, &fuser);

    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));
    percent = (sys.QuadPart - processInfo.lastSysCpu.QuadPart) +
        (user.QuadPart - processInfo.lastUserCpu.QuadPart);
    percent /= (now.QuadPart - processInfo.lastCpu.QuadPart);
    percent /= m_cpuCoreCount;
    processInfo.lastCpu = now;
    processInfo.lastUserCpu = user;
    processInfo.lastSysCpu = sys;
    double currentUsedCpuLoad = std::ceil( percent * 10000.0) / 100.0;
    processInfo.cpuUsagePercent = currentUsedCpuLoad;
    processInfo.cpuAverageUsagePercentSum += currentUsedCpuLoad;
    ++processInfo.cpuAverageUsageCounter;
    //processInfo.usedCpuLoad = processInfo.usedCpuLoadSum/processInfo.usedCpuLoadCounter;
    if(processInfo.cpuAverageUsageCounter == 3)
    {
        processInfo.cpuAverageUsagePercent = processInfo.cpuAverageUsagePercentSum /processInfo.cpuAverageUsageCounter;
        processInfo.cpuAverageUsagePercentSum = processInfo.cpuAverageUsagePercent;
        processInfo.cpuAverageUsageCounter = 1;
    }
}

BOOL CALLBACK ProcessInfo::StaticEnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    //qDebug() << "WorkerProcess::StaticEnumWindowsProc()";

    ProcessInfo *pThis = reinterpret_cast<ProcessInfo*>(lParam);
    return pThis->EnumWindowsProc(hwnd);
}

BOOL ProcessInfo::EnumWindowsProc(HWND hwnd)
{
    WCHAR title[255];

    SetLastError(ERROR_SUCCESS);
    int result = GetWindowText(hwnd, title, 255);
    if(result == 0)
    {
        //DWORD error = GetLastError();
        //std::string message = std::system_category().message(error);
        //qWarning() << __FUNCTION__ << " GetWindowText failed with error " << message.c_str();
    }
    else
    {
        //std::string processWindowName = QString::fromWCharArray(title);
        std::wstring wsTitle( title );
        std::string processWindowName( wsTitle.begin(), wsTitle.end() );

        DWORD processId;
        GetWindowThreadProcessId(hwnd, &processId);

        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if(processHandle == NULL)
        {
            DWORD error = GetLastError();
            std::string message = std::system_category().message(error);
            qWarning() << __FUNCTION__ << " OpenProcess failed with error " << message.c_str();
            return false;
        }

        TCHAR moduleBaseName[MAX_PATH] = TEXT("<unknown>");
        if (GetModuleBaseName( processHandle, 0, moduleBaseName, sizeof(moduleBaseName)/sizeof(TCHAR)))
        {
        }
        else
        {
            DWORD error = GetLastError();
            std::string message = std::system_category().message(error);
            qWarning() << __FUNCTION__ << " GetModuleBaseName failed with error " << message.c_str();
        }

        TCHAR moduleFileName[MAX_PATH] = TEXT("<unknown>");
        if (GetModuleFileNameEx(processHandle, 0, moduleFileName, sizeof(moduleFileName)/sizeof(TCHAR)))
        {
        }
        else
        {
            DWORD error = GetLastError();
            std::string message = std::system_category().message(error);
            qWarning() << __FUNCTION__ << " GetModuleFileNameEx failed with error " << message.c_str();
        }

        //std::string processBaseName = QString::fromWCharArray(moduleBaseName);
        //std::string processFilePath = QString::fromWCharArray(moduleFileName);
        std::wstring wsModuleBaseName( moduleBaseName );
        std::string processBaseName( wsModuleBaseName.begin(), wsModuleBaseName.end() );
        std::wstring wsModuleFileName( moduleFileName );
        std::string processFilePath( wsModuleFileName.begin(), wsModuleFileName.end() );

        Process process;
        process.processID = processId;
        process.description = processWindowName;
        process.processName = processBaseName;
        process.filePath = processFilePath;

        bool newProcess = addProcess(process);
        if(newProcess)
        {
            addProcessModules(process.description, processHandle);
        }

        CloseHandle(processHandle);
    }

    return true;
}
#endif
bool ProcessInfo::addProcess(const Process& processInfo)
{
    std::map<uint32_t, Process>::iterator itProcessMap = m_processMap.find(processInfo.processID);
    if(itProcessMap == m_processMap.end() )
    {
        Process process = processInfo;;
        m_processMap.insert({process.processID,process});

        return true;
    }
    else
    {
        return false;
    }
}
#ifdef _WIN32
bool ProcessInfo::addProcessModules(const std::string& processName, const HANDLE& processHandle)
{
    HMODULE hMods[1024];
    DWORD count;

    EnumProcessModulesEx(processHandle, hMods, sizeof(hMods), &count, LIST_MODULES_ALL);

    for (UINT i=0; i<(count / sizeof(HMODULE)); i++)
    {
        TCHAR szFileName[MAX_PATH];
        TCHAR szBaseName[MAX_PATH];
        if (GetModuleFileNameEx(processHandle, hMods[i], szFileName, _countof(szFileName)-1))
        {
            GetModuleBaseName(processHandle, hMods[i], szBaseName, _countof(szBaseName)-1);
            //std::string moduleBaseName = QString::fromWCharArray(szBaseName);
            std::wstring wsBaseName( szBaseName );
            std::string moduleBaseName( wsBaseName.begin(), wsBaseName.end() );
            //if(moduleBaseName.contains(".exe"))
            {
                //qDebug() << "WorkerProcess::getProcessModules" << processName << " module " << moduleBaseName;
            }
//            std::map<QString, ProcessInfo>::iterator itProcessMap = processMap.find(processBaseName);
//            if(itProcessMap == processMap.end() )
//            {
//                QString filePath = QString::fromWCharArray(szFileName);
//                ProcessInfo info;
//                info.baseName = processBaseName;
//                info.filePath = filePath;
//                info.active = true;

//                processMap.insert({info.baseName,info});
//                //emit processAdded(info.baseName, info.filePath);
//            }
//            else
//            {
//                itProcessMap->second.active = true;
//            }
        }
        return true;
    }
    return false;
}

#else
#endif
