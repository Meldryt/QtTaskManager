#include "WorkerProcess.h"
#include <QElapsedTimer>
#include <QDebug>

//#include <iostream>
//#include <stdio.h>
//#include <tchar.h>
//#include <psapi.h>
//#include <strsafe.h>

////#include <cpuid.h>
//#include <d3d11.h>
//#include <d3dcompiler.h>
////#include <d3dkmthk.h>

//#include <setupapi.h>
//#include <initguid.h>
//#include <cfgmgr32.h>
////#include <portabledeviceapi.h>
////#include <wrl.h>
////#include <sysinfoapi.h>

//#include <intsafe.h>
//#include <string>

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

    m_gpuInfo = std::make_unique<GpuInfo>();
    m_cpuInfo = std::make_unique<CpuInfo>();
    memoryInfo = std::make_unique<MemoryInfo>();
    processInfo = std::make_unique<ProcessInfo>();
    devicesInfo = std::make_unique<DevicesInfo>();

    timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, &QTimer::timeout, this, &WorkerProcess::process);
}

void WorkerProcess::start()
{   
    m_gpuInfo->init();
    m_cpuInfo->init();
    memoryInfo->init();
    processInfo->setProcessorCount(m_cpuInfo->getStaticInfo().processorCount);
    //processInfo->init();
    //devicesInfo->init();

    emit signalStaticInfoCpu(m_cpuInfo->getStaticInfo());
    emit signalStaticInfoGpu(m_gpuInfo->getStaticInfo());
    emit signalStaticInfoMemory(memoryInfo->getStaticInfo());

    timer->start();
}

void WorkerProcess::process()
{ 
    m_gpuInfo->update();
    m_cpuInfo->update();
    memoryInfo->update();
    processInfo->update();
    devicesInfo->update();

    emit signalDynamicInfoCpu(m_cpuInfo->getDynamicInfo());
    emit signalDynamicInfoGpu(m_gpuInfo->getDynamicInfo());
    emit signalDynamicInfoMemory(memoryInfo->getDynamicInfo());
    emit signalDynamicInfoProcesses(processInfo->getProcessMap());
    //init GpuInfo
    //init CpuInfo

    //emit finished();
}
