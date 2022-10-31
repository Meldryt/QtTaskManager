#include "WorkerProcess.h"
#include <QElapsedTimer>
#include <QDebug>

#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <strsafe.h>

//#define NTDDI_VERSION NTDDI_WIN10
//#include <windows.h>
//#include <tchar.h>
//#include <stdio.h>
//#include <d3d9.h>
//#include <cfgmgr32.h>
//#include <ntddvdeo.h>
#include <d3d11.h>
#include <d3dcompiler.h>
//#include <d3dkmthk.h>

//#pragma comment(lib, "gdi32.lib")  // Doesn't do much, since it doesn't have the exports anyway...
//#pragma comment(lib, "advapi32.lib")
//#pragma comment(lib, "pdh.lib")
//typedef NTSTATUS (APIENTRY *PD3DKMTQueryStatistics)(_In_ CONST D3DKMT_QUERYSTATISTICS*);
//typedef NTSTATUS (APIENTRY *PD3DKMTOpenAdapterFromDeviceName)(_Inout_ D3DKMT_OPENADAPTERFROMDEVICENAME*);
//D3DKMT_OPENADAPTERFROMDEVICENAME openAdapterFromDeviceName;
//D3DKMT_QUERYSTATISTICS queryStatistics;

#ifndef NV_H
#define NV_H

#define NVAPI_MAX_THERMAL_SENSORS_PER_GPU 3
typedef enum
{
    NVAPI_THERMAL_TARGET_NONE          = 0,
    NVAPI_THERMAL_TARGET_GPU           = 1,     //!< GPU core temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_MEMORY        = 2,     //!< GPU memory temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_POWER_SUPPLY  = 4,     //!< GPU power supply temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_BOARD         = 8,     //!< GPU board ambient temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_VCD_BOARD     = 9,     //!< Visual Computing Device Board temperature requires NvVisualComputingDeviceHandle
    NVAPI_THERMAL_TARGET_VCD_INLET     = 10,    //!< Visual Computing Device Inlet temperature requires NvVisualComputingDeviceHandle
    NVAPI_THERMAL_TARGET_VCD_OUTLET    = 11,    //!< Visual Computing Device Outlet temperature requires NvVisualComputingDeviceHandle

    NVAPI_THERMAL_TARGET_ALL           = 15,
    NVAPI_THERMAL_TARGET_UNKNOWN       = -1,
} NV_THERMAL_TARGET;

typedef struct
{
    int   version;                //!< structure version
    int   count;                  //!< number of associated thermal sensors
    struct
    {
        int       controller;        //!< internal, ADM1032, MAX6649...
        int                       defaultMinTemp;    //!< The min default temperature value of the thermal sensor in degree Celsius
        int                       defaultMaxTemp;    //!< The max default temperature value of the thermal sensor in degree Celsius
        int                       currentTemp;       //!< The current temperature value of the thermal sensor in degree Celsius
        NV_THERMAL_TARGET           target;            //!< Thermal sensor targeted @ GPU, memory, chipset, powersupply, Visual Computing Device, etc.
    } sensor[NVAPI_MAX_THERMAL_SENSORS_PER_GPU];

} NV_GPU_THERMAL_SETTINGS;

//#include <nvapi.h>


/*
 * Name: NvGpuDetected
 * Desc: Returns true if an NVIDIA Gpu has been detected on this system.
 * NOTE: This function depends on whether a valid NVIDIA driver is installed
 *       on the target machine.  Since the Surface Hub does not include an
 *		 nvapi[64].dll in it's strippified driver, we need to load it directly
 *		 in order for the required APIs to work.
 */
BOOL NvGpuDetected();

/*
 * Name: NvGetGpuLoad
 * Desc: Returns the NVIDIA Gpu's current load percentage.
 */
int  NvGetGpuLoad();

/*
 * Name: NvGetGpuTemperature
 * Desc: Returns the current temperature of an NVIDIA Gpu.
 */
int  NvGetGpuTemperature();

#endif // NV_H


//#include "nvidia.h"

#if defined(_M_X64) || defined(__amd64__)
#define NVAPI_DLL "nvapi64.dll"
#else
#define NVAPI_DLL "nvapi.dll"
#endif


// magic numbers, do not change them
#define NVAPI_MAX_PHYSICAL_GPUS   64
#define NVAPI_MAX_USAGES_PER_GPU  34

// function pointer types
typedef int *(*NvAPI_QueryInterface_t)(unsigned int offset);
typedef int (*NvAPI_Initialize_t)();
typedef int (*NvAPI_EnumPhysicalGPUs_t)(int **handles, int *count);
typedef int (*NvAPI_GPU_GetUsages_t)(int *handle, unsigned int *usages);
typedef int (*NvAPI_GPU_GetThermalSettings_t)(int *handle, int sensorIndex, NV_GPU_THERMAL_SETTINGS *temp);

// nvapi.dll internal function pointers
NvAPI_QueryInterface_t      NvAPI_QueryInterface     = NULL;
NvAPI_Initialize_t          NvAPI_Initialize         = NULL;
NvAPI_EnumPhysicalGPUs_t    NvAPI_EnumPhysicalGPUs   = NULL;
NvAPI_GPU_GetUsages_t       NvAPI_GPU_GetUsages      = NULL;
NvAPI_GPU_GetThermalSettings_t	NvAPI_GPU_GetThermalSettings = NULL;


/*
 * Name: NvGpuDetected
 * Desc: Returns true if an NVIDIA Gpu has been detected on this system.
 * NOTE: This function depends on whether a valid NVIDIA driver is installed
 *       on the target machine.  Since the Surface Hub does not include an
 *		 nvapi[64].dll in it's strippified driver, we need to load it directly
 *		 in order for the required APIs to work.
 */
BOOL NvGpuDetected()
{
    HMODULE hmod = LoadLibraryA( NVAPI_DLL );

    if( hmod == NULL )
    {
        std::cerr << "Couldn't find " << NVAPI_DLL << std::endl;
        return FALSE;
    }

    // nvapi_QueryInterface is a function used to retrieve other internal functions in nvapi.dll
    NvAPI_QueryInterface = (NvAPI_QueryInterface_t) GetProcAddress( hmod, "nvapi_QueryInterface" );

    // some useful internal functions that aren't exported by nvapi.dll
    NvAPI_Initialize = (NvAPI_Initialize_t) (*NvAPI_QueryInterface)(0x0150E828);
    NvAPI_EnumPhysicalGPUs = (NvAPI_EnumPhysicalGPUs_t) (*NvAPI_QueryInterface)(0xE5AC921F);
    NvAPI_GPU_GetUsages = (NvAPI_GPU_GetUsages_t) (*NvAPI_QueryInterface)(0x189A1FDF);
    NvAPI_GPU_GetThermalSettings = (NvAPI_GPU_GetThermalSettings_t) (*NvAPI_QueryInterface)(0xE3640A56);

    if( NvAPI_Initialize == NULL || NvAPI_EnumPhysicalGPUs == NULL ||
        NvAPI_EnumPhysicalGPUs == NULL || NvAPI_GPU_GetUsages == NULL )
    {
        std::cerr << "Couldn't get functions in nvapi.dll" << std::endl;
        return FALSE;
    }

    // initialize NvAPI library, call it once before calling any other NvAPI functions
    if( (*NvAPI_Initialize)() != 0 )
    {
        std::cerr << "Could not initialize nvapi!" << std::endl;
    }

    return TRUE;
}

/*
 * Name: NvGetGpuInfo
 * Desc: Returns the NVIDIA Gpu's current load percentage.
 */
void NvGetGpuInfo(WorkerProcess::TotalInfo& totalInfo)
{
    (*NvAPI_Initialize)();
    NV_GPU_THERMAL_SETTINGS 		 nvgts;
    int          gpuCount = 0;
    int         *gpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { NULL };
    unsigned int gpuUsages[NVAPI_MAX_USAGES_PER_GPU] = { 0 };

    // gpuUsages[0] must be this value, otherwise NvAPI_GPU_GetUsages won't work
    gpuUsages[0] = (NVAPI_MAX_USAGES_PER_GPU * 4) | 0x10000;

    (*NvAPI_EnumPhysicalGPUs)( gpuHandles, &gpuCount );
    nvgts.version = sizeof(NV_GPU_THERMAL_SETTINGS) | (1<<16);
    nvgts.count = 0;
    nvgts.sensor[0].controller = -1;
    nvgts.sensor[0].target = NVAPI_THERMAL_TARGET_GPU;

    (*NvAPI_GPU_GetThermalSettings)(gpuHandles[0], 0 ,&nvgts);
    totalInfo.gpuTemperature = nvgts.sensor[0].currentTemp;

    (*NvAPI_GPU_GetUsages)( gpuHandles[0], gpuUsages );
    totalInfo.totalGPULoad = gpuUsages[3];
}

/*
 * Name: NvGetGpuTemperature
 * Desc: Returns the current temperature of an NVIDIA Gpu.
 */
int  NvGetGpuTemperature()
{
    return 0;	// TODO
}


#if 0
int main()
{
#if defined(_M_X64) || defined(__amd64__)
    HMODULE hmod = LoadLibraryA("nvapi64.dll");
#else
    HMODULE hmod = LoadLibraryA("nvapi.dll");
#endif
    if (hmod == NULL)
    {
        std::cerr << "Couldn't find nvapi.dll" << std::endl;
        return 1;
    }

    // nvapi.dll internal function pointers
    NvAPI_QueryInterface_t      NvAPI_QueryInterface     = NULL;
    NvAPI_Initialize_t          NvAPI_Initialize         = NULL;
    NvAPI_EnumPhysicalGPUs_t    NvAPI_EnumPhysicalGPUs   = NULL;
    NvAPI_GPU_GetUsages_t       NvAPI_GPU_GetUsages      = NULL;

    // nvapi_QueryInterface is a function used to retrieve other internal functions in nvapi.dll
    NvAPI_QueryInterface = (NvAPI_QueryInterface_t) GetProcAddress(hmod, "nvapi_QueryInterface");

    // some useful internal functions that aren't exported by nvapi.dll
    NvAPI_Initialize = (NvAPI_Initialize_t) (*NvAPI_QueryInterface)(0x0150E828);
    NvAPI_EnumPhysicalGPUs = (NvAPI_EnumPhysicalGPUs_t) (*NvAPI_QueryInterface)(0xE5AC921F);
    NvAPI_GPU_GetUsages = (NvAPI_GPU_GetUsages_t) (*NvAPI_QueryInterface)(0x189A1FDF);

    if (NvAPI_Initialize == NULL || NvAPI_EnumPhysicalGPUs == NULL ||
        NvAPI_EnumPhysicalGPUs == NULL || NvAPI_GPU_GetUsages == NULL)
    {
        std::cerr << "Couldn't get functions in nvapi.dll" << std::endl;
        return 2;
    }

    // initialize NvAPI library, call it once before calling any other NvAPI functions
    (*NvAPI_Initialize)();

    int          gpuCount = 0;
    int         *gpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { NULL };
    unsigned int gpuUsages[NVAPI_MAX_USAGES_PER_GPU] = { 0 };

    // gpuUsages[0] must be this value, otherwise NvAPI_GPU_GetUsages won't work
    gpuUsages[0] = (NVAPI_MAX_USAGES_PER_GPU * 4) | 0x10000;

    (*NvAPI_EnumPhysicalGPUs)(gpuHandles, &gpuCount);

    // print GPU usage every second
    for (int i = 0; i < 100; i++)
    {
        (*NvAPI_GPU_GetUsages)(gpuHandles[0], gpuUsages);
        int usage = gpuUsages[3];
        std::cout << "GPU Usage: " << usage << std::endl;
        Sleep(1000);
    }

    return 0;
}
#endif

WorkerProcess::WorkerProcess(QObject *parent)
    : QObject{parent}
{
//    if (NT_SUCCESS(D3DKMTQueryStatistics(&queryStatistics)))
//    {

//    }
//    GetSystemTimeAsFileTime(&ftime);
//    memcpy(&lastCPU, &ftime, sizeof(FILETIME));

//    HANDLE self = GetCurrentProcess();
//    GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
//    memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
//    memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));

    qDebug() << "GPU Detected: " << NvGpuDetected();

    PdhOpenQuery(NULL, 0, &totalCPUQuery);
    std::string text = "\\Processor(_Total)\\% Processor Time";
    std::wstring ws = std::wstring(text.begin(), text.end());
    LPCWSTR sw = ws.c_str();
    // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
    PdhAddEnglishCounter(totalCPUQuery, sw, 0, &totalCPUCounter);
    PdhCollectQueryData(totalCPUQuery);

    SYSTEM_INFO sysInfo;

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

    NvGetGpuInfo(totalInfo);
    //qDebug() << "GPU Detected: " << NvGpuDetected();
    //qDebug() << "GPU Load: " << NvGetGpuLoad();

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
