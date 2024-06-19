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
    staticInfo.totalVirtualMemory = (totalVirtualMem / 1024) / 1024;

    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    staticInfo.totalPhysicalMemory = (totalPhysMem / 1024) / 1024;
}

void MemoryInfo::fetchDynamicInfo()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    dynamicInfo.usedVirtualMemory = (virtualMemUsed / 1024) / 1024;

    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    dynamicInfo.usedPhysicalMemory = (physMemUsed / 1024) / 1024;
}

const Globals::MemoryStaticInfo &MemoryInfo::getStaticInfo() const
{
    return staticInfo;
}

const Globals::MemoryDynamicInfo &MemoryInfo::getDynamicInfo() const
{
    return dynamicInfo;
}
