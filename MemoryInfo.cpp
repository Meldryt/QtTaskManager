#include "MemoryInfo.h"

#include <windows.h>
#include <sysinfoapi.h>

MemoryInfo::MemoryInfo()
{

}

void MemoryInfo::init()
{
    fetchStaticInfo();
}

void MemoryInfo::update()
{
    fetchDynamicInfo();
}

void MemoryInfo::fetchStaticInfo()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
    staticInfo.totalVirtualMemory = totalVirtualMem;

    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    staticInfo.totalPhysicalMemory = totalPhysMem;
}

void MemoryInfo::fetchDynamicInfo()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    dynamicInfo.usedVirtualMemory = virtualMemUsed;

    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    dynamicInfo.usedPhysicalMemory = physMemUsed;
}

const MemoryInfo::MemoryStaticInfo &MemoryInfo::getStaticInfo() const
{
    return staticInfo;
}

const MemoryInfo::MemoryDynamicInfo &MemoryInfo::getDynamicInfo() const
{
    return dynamicInfo;
}
