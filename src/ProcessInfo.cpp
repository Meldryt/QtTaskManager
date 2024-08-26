#include "ProcessInfo.h"

#include <cmath>

#ifdef _WIN32
#include <psapi.h>
#else
#endif

#include <QDebug>

ProcessInfo::ProcessInfo()
{
    qDebug() << __FUNCTION__;

    m_elapsedTimer = new QElapsedTimer();
    m_elapsedTimer->start();
}

ProcessInfo::~ProcessInfo()
{
    qDebug() << __FUNCTION__;
}

void ProcessInfo::setProcessorCount(uint8_t newProcessorCount)
{
    m_processorCount = newProcessorCount;
}

const std::map<uint32_t, ProcessInfo::Process> &ProcessInfo::getProcessMap() const
{
    return m_processMap;
}

void ProcessInfo::update()
{
    //Enumerates all top-level windows on the screen by passing the handle to each window, in turn,
    //to an application-defined callback function. EnumWindows continues until the last top-level
    //window is enumerated or the callback function returns FALSE.
#ifdef _WIN32
EnumWindows(StaticEnumWindowsProc, reinterpret_cast<LPARAM>(this));
#else
#endif

    updateRunningProcesses();
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

        process->second.usedVirtualMemory = pmc.PrivateUsage;
        process->second.usedPhysicalMemory = pmc.WorkingSetSize;
        process->second.timestamp = int64_t(m_elapsedTimer->elapsed() * 1000);
        updateProcessUsage(processHandle, process->second);

        CloseHandle(processHandle);

        ++process;
    }
}

void ProcessInfo::updateProcessUsage(const HANDLE& processHandle, Process& processInfo)
{
    if (m_processorCount == 0)
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
    percent /= m_processorCount;
    processInfo.lastCpu = now;
    processInfo.lastUserCpu = user;
    processInfo.lastSysCpu = sys;
    double currentUsedCpuLoad = std::ceil( percent * 10000.0) / 100.0;
    processInfo.usedCpuLoadSum += currentUsedCpuLoad;
    ++processInfo.usedCpuLoadCounter;
    //processInfo.usedCpuLoad = processInfo.usedCpuLoadSum/processInfo.usedCpuLoadCounter;
    if(processInfo.usedCpuLoadCounter == 3)
    {
        processInfo.usedCpuLoad = processInfo.usedCpuLoadSum/processInfo.usedCpuLoadCounter;
        processInfo.usedCpuLoadSum = processInfo.usedCpuLoad;
        processInfo.usedCpuLoadCounter = 1;
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
        DWORD error = GetLastError();
//        LPVOID lpMsgBuf;
//        LPVOID lpDisplayBuf;
//        std::string message = std::system_category().message(error);
//        qDebug() << "GetLastError: " << message.c_str();
    }
    else
    {
        //std::string processWindowName = QString::fromWCharArray(title);
        std::wstring wsTitle( title );
        std::string processWindowName( wsTitle.begin(), wsTitle.end() );

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

        //std::string processBaseName = QString::fromWCharArray(moduleBaseName);
        //std::string processFilePath = QString::fromWCharArray(moduleFileName);
        std::wstring wsModuleBaseName( moduleBaseName );
        std::string processBaseName( wsModuleBaseName.begin(), wsModuleBaseName.end() );
        std::wstring wsModuleFileName( moduleFileName );
        std::string processFilePath( wsModuleFileName.begin(), wsModuleFileName.end() );

        Process process;
        process.processID = processId;
        process.description = processWindowName;
        process.baseName = processBaseName;
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

bool ProcessInfo::addProcess(const Process& processInfo)
{
    std::map<uint32_t, Process>::iterator itProcessMap = m_processMap.find(processInfo.processID);
    if(itProcessMap == m_processMap.end() )
    {
        Process process;
        process.processID = processInfo.processID;
        process.description = processInfo.description;
        process.baseName = processInfo.baseName;
        process.filePath = processInfo.filePath;

        m_processMap.insert({processInfo.processID,process});

        //qDebug() << "WorkerProcess::receivedProcess new process " << process.description.c_str();
        return true;
    }
    else
    {
        return false;
    }
}

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
