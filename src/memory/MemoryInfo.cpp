#include "MemoryInfo.h"

#include "../Globals.h"

#ifdef _WIN32
#include <windows.h>
#include <sysinfoapi.h>
#endif

MemoryInfo::MemoryInfo()
{
    qDebug() << __FUNCTION__;
}

MemoryInfo::~MemoryInfo()
{
    qDebug() << __FUNCTION__;
}

void MemoryInfo::init()
{
    readStaticInfo();
}

void MemoryInfo::update()
{
    readDynamicInfo();
}

void MemoryInfo::readStaticInfo()
{
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
    m_totalVirtualMemory = (totalVirtualMem / 1024) / 1024;

    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    m_totalPhysicalMemory = (totalPhysMem / 1024) / 1024;
#endif
    m_staticInfo[Globals::SysInfoAttr::Key_Memory_TotalVirtualMemory] = m_totalVirtualMemory;
    m_staticInfo[Globals::SysInfoAttr::Key_Memory_TotalPhysicalMemory] = m_totalPhysicalMemory;
}

void MemoryInfo::readDynamicInfo()
{
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    m_usedVirtualMemory = (virtualMemUsed / 1024) / 1024;

    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    m_usedPhysicalMemory = (physMemUsed / 1024) / 1024;
#endif
    m_dynamicInfo[Globals::SysInfoAttr::Key_Memory_UsedVirtualMemory] = m_usedVirtualMemory;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Memory_UsedPhysicalMemory] = m_usedPhysicalMemory;
}
