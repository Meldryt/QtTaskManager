#include "WorkerProcess.h"
#include <QElapsedTimer>
#include <QDebug>

#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
//#include <pdh.h>
#include <strsafe.h>

WorkerProcess::WorkerProcess(QObject *parent)
    : QObject{parent}
{
//    GetSystemTimeAsFileTime(&ftime);
//    memcpy(&lastCPU, &ftime, sizeof(FILETIME));

//    HANDLE self = GetCurrentProcess();
//    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
//    memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
//    memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

    PdhOpenQuery(NULL, 0, &totalCPUQuery);
    std::string text = "\\Processor(_Total)\\% Processor Time";
    std::wstring ws = std::wstring(text.begin(), text.end());
    LPCWSTR sw = ws.c_str();
    // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
    PdhAddEnglishCounter(totalCPUQuery, sw, 0, &totalCPUCounter);
    PdhCollectQueryData(totalCPUQuery);

    SYSTEM_INFO sysInfo;
    FILETIME ftime, fsys, fuser;

    GetSystemInfo(&sysInfo);
    numProcessors = sysInfo.dwNumberOfProcessors;

    for(uint8_t i=0;i<numProcessors;++i)
    {
        PDH_HQUERY singleCPUQuery;
        PDH_HCOUNTER singleCPUCounter;
        text = "\\Processor("+std::to_string(i)+")\\% Processor Time";
        ws = std::wstring(text.begin(), text.end());
        sw = ws.c_str();

        PdhOpenQuery(NULL, 0, &singleCPUQuery);
        // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
        PdhAddEnglishCounter(singleCPUQuery, sw, 0, &singleCPUCounter);
        PdhCollectQueryData(singleCPUQuery);

        singleCPUQueries.push_back(singleCPUQuery);
        singleCPUCounters.push_back(singleCPUCounter);
        totalInfo.singleCoreLoads.push_back(0.0);
    }

    timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, &QTimer::timeout, this, &WorkerProcess::process);
}

void WorkerProcess::start()
{
    timer->start();
}

void WorkerProcess::process()
{
    //Enumerates all top-level windows on the screen by passing the handle to each window, in turn,
    //to an application-defined callback function. EnumWindows continues until the last top-level
    //window is enumerated or the callback function returns FALSE.
    EnumWindows(StaticEnumWindowsProc, reinterpret_cast<LPARAM>(this));
    updateRunningProcesses();

    emit receivedProcessList();
    emit receivedTotalInfo();
    //emit finished();
}

void WorkerProcess::updateRunningProcesses()
{
    auto process = std::begin(processMap);
    while (process != std::end(processMap))
    {
        HANDLE processHandle = OpenProcess(SYNCHRONIZE, FALSE, process->second.processID);
        DWORD ret = WaitForSingleObject(processHandle, 0);
        CloseHandle(processHandle);

        if (ret != WAIT_TIMEOUT)
        {
            qDebug() << "remove process " << process->second.description;
            removedProcesses.push_back(process->second);
            process = processMap.erase(process);
        }
        else
        {
            ++process;
        }
    }

    updateTotalUsage();
    updateProcessesUsage();
}

void WorkerProcess::updateTotalUsage()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
    DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    totalInfo.totalVirtualMemory = totalVirtualMem;
    totalInfo.usedVirtualMemory = virtualMemUsed;

    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    totalInfo.totalPhysicalMemory = totalPhysMem;
    totalInfo.usedPhysicalMemory = physMemUsed;

//    PdhOpenQuery(NULL, NULL, &cpuQuery);
//    // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
//    PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
//    PdhCollectQueryData(cpuQuery);

    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(totalCPUQuery);
    PdhGetFormattedCounterValue(totalCPUCounter, PDH_FMT_DOUBLE, NULL, &counterVal);
    totalInfo.totalCPULoad = counterVal.doubleValue;

    for(uint8_t i=0;i<numProcessors;++i)
    {
        PdhCollectQueryData(singleCPUQueries[i]);
        PdhGetFormattedCounterValue(singleCPUCounters[i], PDH_FMT_DOUBLE, NULL, &counterVal);
        totalInfo.singleCoreLoads[i] = counterVal.doubleValue;
    }

}

void WorkerProcess::updateProcessesUsage()
{
    auto process = std::begin(processMap);
    while (process != std::end(processMap))
    {
        HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process->first);
        if(processHandle == NULL)
        {
            continue;
        }

        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(processHandle, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

        process->second.usedVirtualMemory = pmc.PrivateUsage;
        process->second.usedPhysicalMemory = pmc.WorkingSetSize;
        updateProcessUsage(processHandle, process->second);

        CloseHandle(processHandle);

        ++process;
    }
}

void WorkerProcess::updateProcessUsage(const HANDLE& processHandle, ProcessInfo& processInfo)
{
    FILETIME ftime, fsys, fuser;
    ULARGE_INTEGER now, sys, user;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    memcpy(&now, &ftime, sizeof(FILETIME));

    GetProcessTimes(processHandle, &ftime, &ftime, &fsys, &fuser);

    memcpy(&sys, &fsys, sizeof(FILETIME));
    memcpy(&user, &fuser, sizeof(FILETIME));
    percent = (sys.QuadPart - processInfo.lastSysCPU.QuadPart) +
        (user.QuadPart - processInfo.lastUserCPU.QuadPart);
    percent /= (now.QuadPart - processInfo.lastCPU.QuadPart);
    percent /= numProcessors;
    processInfo.lastCPU = now;
    processInfo.lastUserCPU = user;
    processInfo.lastSysCPU = sys;
    double currentUsedCPULoad = std::ceil( percent * 10000.0) / 100.0;
    processInfo.usedCPULoadSum += currentUsedCPULoad;
    ++processInfo.usedCPULoadCounter;
    //processInfo.usedCPULoad = processInfo.usedCPULoadSum/processInfo.usedCPULoadCounter;
    if(processInfo.usedCPULoadCounter == 3)
    {
        processInfo.usedCPULoad = processInfo.usedCPULoadSum/processInfo.usedCPULoadCounter;
        processInfo.usedCPULoadSum = processInfo.usedCPULoad;
        processInfo.usedCPULoadCounter = 1;
    }
}


bool WorkerProcess::fillProcessList(std::vector<WorkerProcess::ProcessInfo>& processList)
{
    bool changed{false};
    if(!processList.empty())
    {
        auto removedProcess = std::begin(removedProcesses);
        while (removedProcess != std::end(removedProcesses))
        {
            auto it = std::find_if(processList.begin(), processList.end(), [removedProcess](const WorkerProcess::ProcessInfo& processInfo)
            {
                return (removedProcess->processID == processInfo.processID);
            });

            if(it != processList.end())
            {
                qDebug() << "erase " << it->description;
                processList.erase(it);
                changed = true;
            }
            ++removedProcess;
        }
        removedProcesses.clear();
    }

    uint8_t index = 0;
    auto process = std::begin(processMap);
    while (process != std::end(processMap))
    {
        auto it = std::find_if(processList.begin(), processList.end(), [process](const WorkerProcess::ProcessInfo& processInfo)
        {
            return (process->second.processID == processInfo.processID);
        });

        if(it == processList.end())
        {
            processList.push_back(process->second);
            changed = true;
        }
        else
        {
            it->usedPhysicalMemory = process->second.usedPhysicalMemory;
            it->usedVirtualMemory = process->second.usedVirtualMemory;
            it->usedCPULoad = process->second.usedCPULoad;
        }

        ++process;
        ++index;
    }
    return changed;
}

bool WorkerProcess::fillTotalInfo(TotalInfo& t)
{
    t = totalInfo;
    return true;
}

BOOL CALLBACK WorkerProcess::StaticEnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    //qDebug() << "WorkerProcess::StaticEnumWindowsProc()";

    WorkerProcess *pThis = reinterpret_cast<WorkerProcess*>(lParam);
    return pThis->EnumWindowsProc(hwnd);
}

BOOL WorkerProcess::EnumWindowsProc(HWND hwnd)
{
    WCHAR title[255];

    SetLastError(ERROR_SUCCESS);
    int result = GetWindowText(hwnd, title, 255);
    if(result == 0)
    {
        DWORD error = GetLastError();
//        LPVOID lpMsgBuf;
//        LPVOID lpDisplayBuf;
//        std::string message = std::system_category().message(error);
//        qDebug() << "GetLastError: " << message.c_str();
    }
    else
    {
        QString processWindowName = QString::fromWCharArray(title);

        DWORD processId;
        GetWindowThreadProcessId(hwnd, &processId);

        HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if(processHandle == NULL)
        {
            return false;
        }

        TCHAR moduleBaseName[MAX_PATH] = TEXT("<unknown>");
        if (GetModuleBaseName( processHandle, 0, moduleBaseName, sizeof(moduleBaseName)/sizeof(TCHAR)))
        {
        }
        else
        {
            DWORD error = GetLastError();
        }

        TCHAR moduleFileName[MAX_PATH] = TEXT("<unknown>");
        if (GetModuleFileNameEx(processHandle, 0, moduleFileName, sizeof(moduleFileName)/sizeof(TCHAR)))
        {
        }
        else
        {
            DWORD error = GetLastError();
        }

        QString processBaseName = QString::fromWCharArray(moduleBaseName);
        QString processFilePath = QString::fromWCharArray(moduleFileName);

        ProcessInfo processInfo;
        processInfo.processID = processId;
        processInfo.description = processWindowName;
        processInfo.baseName = processBaseName;
        processInfo.filePath = processFilePath;

        bool newProcess = addProcess(processInfo);
        if(newProcess)
        {
            addProcessModules(processInfo.description, processHandle);
        }

        CloseHandle(processHandle);
    }

    return true;
}

bool WorkerProcess::addProcess(const ProcessInfo& processInfo)
{
    std::map<uint32_t, ProcessInfo>::iterator itProcessMap = processMap.find(processInfo.processID);
    if(itProcessMap == processMap.end() )
    {
        ProcessInfo info;
        info.processID = processInfo.processID;
        info.description = processInfo.description;
        info.baseName = processInfo.baseName;
        info.filePath = processInfo.filePath;

        processMap.insert({processInfo.processID,info});

        //qDebug() << "WorkerProcess::receivedProcess new process " << info.description;
        return true;
    }
    else
    {
        return false;
    }
}

bool WorkerProcess::addProcessModules(const QString& processName, const HANDLE& processHandle)
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
            QString moduleBaseName = QString::fromWCharArray(szBaseName);
            //if(moduleBaseName.contains(".exe"))
            {
                qDebug() << "WorkerProcess::getProcessModules" << processName << " module " << moduleBaseName;
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
