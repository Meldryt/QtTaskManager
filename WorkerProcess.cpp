#include "WorkerProcess.h"
#include <QElapsedTimer>
#include <QDebug>

#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

WorkerProcess::WorkerProcess(QObject *parent)
    : QObject{parent}
{
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
//    QElapsedTimer elapsedTimer;
//    elapsedTimer.start();
//    while(elapsedTimer.elapsed() < 1000)
//    {

//    }
    EnumWindows(StaticEnumWindowsProc, reinterpret_cast<LPARAM>(this));
    listRunningProcesses();
    emit receivedList();

//    emit finished();
}

void WorkerProcess::listRunningProcesses()
{
//    HANDLE hProcess = GetCurrentProcess();
//    HMODULE hMods[1024];
//    DWORD count;

//    EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &count, LIST_MODULES_ALL);

//    for(auto& processInfo : processMap)
//    {
//        processInfo.second.active = false;
//    }

//    for (UINT i=0; i<(count / sizeof(HMODULE)); i++)
//    {
//        TCHAR szFileName[MAX_PATH];
//        TCHAR szBaseName[MAX_PATH];
//        if (GetModuleFileNameEx(hProcess, hMods[i], szFileName, _countof(szFileName)-1))
//        {
//            GetModuleBaseName(hProcess, hMods[i], szBaseName, _countof(szBaseName)-1);
//            QString processBaseName = QString::fromWCharArray(szBaseName);
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
//        }
//    }

//    auto process = std::begin(processMap);
//    while (process != std::end(processMap))
//    {
//        // Do some stuff
//        if (!process->second.active)
//        {
//            process = processMap.erase(process);
//            //emit processRemoved(process->second.baseName);
//        }
//        else
//        {
//            ++process;
//        }
//    }

    auto process = std::begin(processMap);
    while (process != std::end(processMap))
    {
        HANDLE processHandle = OpenProcess(SYNCHRONIZE, FALSE, process->second.processID);
        DWORD ret = WaitForSingleObject(processHandle, 0);
        CloseHandle(processHandle);

        if (ret != WAIT_TIMEOUT)
        {
            process = processMap.erase(process);
        }
        else
        {
            ++process;
        }
    }

    getTotalUsage();
    getProcessesUsage();
}

void WorkerProcess::getTotalUsage()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
    DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    totalUsageInfo.totalVirtualMemory = totalVirtualMem;
    totalUsageInfo.usedVirtualMemory = virtualMemUsed;

    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    totalUsageInfo.totalPhysicalMemory = totalPhysMem;
    totalUsageInfo.usedPhysicalMemory = physMemUsed;
}

void WorkerProcess::getProcessesUsage()
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
        SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
        SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

        process->second.usedVirtualMemory = virtualMemUsedByMe;
        process->second.usedPhysicalMemory = physMemUsedByMe;

        CloseHandle(processHandle);

        ++process;
    }
}

void WorkerProcess::getProcessList(std::vector<WorkerProcess::ProcessInfo>& processList)
{
    //std::vector<ProcessInfo> processList;
    auto process = std::begin(processMap);
    while (process != std::end(processMap))
    {
        processList.push_back(process->second);
        ++process;
    }
}

BOOL CALLBACK WorkerProcess::StaticEnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    WorkerProcess *pThis = reinterpret_cast<WorkerProcess*>(lParam);
    return pThis->EnumWindowsProc(hwnd);
}

BOOL WorkerProcess::EnumWindowsProc(HWND hwnd)
{
    WCHAR title[255];
    if(GetWindowText(hwnd, title, 255))
    {
        QString processWindowName = QString::fromWCharArray(title);

        DWORD processId;
        GetWindowThreadProcessId(hwnd, &processId);

        HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if(processHandle == NULL)
        {
            return false;
        }

        HMODULE hMod;
        DWORD cbNeeded;
        TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

        if ( EnumProcessModules( processHandle, &hMod, sizeof(hMod),
             &cbNeeded) )
        {
            GetModuleBaseName( processHandle, hMod, szProcessName,
                               sizeof(szProcessName)/sizeof(TCHAR) );
        }

        CloseHandle(processHandle);

        QString processBaseName = QString::fromWCharArray(szProcessName);

        std::map<uint32_t, ProcessInfo>::iterator itProcessMap = processMap.find(processId);
        if(itProcessMap == processMap.end() )
        {
            QString filePath = "";//QString::fromWCharArray(szFileName);
            ProcessInfo info;
            info.processID = processId;
            info.description = processWindowName;
            info.baseName = processBaseName;
            info.filePath = filePath;

            processMap.insert({processId,info});
        }

        //HMODULE hMods[1024];
        //DWORD count;

        //EnumProcessModulesEx(processHandle, hMods, sizeof(hMods), &count, LIST_MODULES_ALL);

    }

    return true;
}
