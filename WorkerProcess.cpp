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

    gpuInfo = std::make_unique<GPUInfo>();
    cpuInfo = std::make_unique<CPUInfo>();
    memoryInfo = std::make_unique<MemoryInfo>();
    processInfo = std::make_unique<ProcessInfo>();
    devicesInfo = std::make_unique<DevicesInfo>();

    timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, &QTimer::timeout, this, &WorkerProcess::process);
}

void WorkerProcess::start()
{   
    gpuInfo->init();
    cpuInfo->init();
    memoryInfo->init();
    processInfo->setProcessorCount(cpuInfo->getStaticInfo().processorCount);
    //processInfo->init();
    //devicesInfo->init();

    emit signalStaticInfo_CPU(cpuInfo->getStaticInfo());
    emit signalStaticInfo_GPU(gpuInfo->getStaticInfo());
    emit signalStaticInfo_Memory(memoryInfo->getStaticInfo());

    timer->start();
}

void WorkerProcess::process()
{ 
    gpuInfo->update();
    cpuInfo->update();
    memoryInfo->update();
    processInfo->update();
    devicesInfo->update();

    emit signalDynamicInfo_CPU(cpuInfo->getDynamicInfo());
    emit signalDynamicInfo_GPU(gpuInfo->getDynamicInfo());
    emit signalDynamicInfo_Memory(memoryInfo->getDynamicInfo());
    emit signalDynamicInfo_Processes(processInfo->getProcessMap());
    //init gpuInfo
    //init cpuInfo

    //emit finished();
}
