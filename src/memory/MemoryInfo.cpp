#include "MemoryInfo.h"

#include "../Globals.h"

#ifdef _WIN32
#include <windows.h>
#include <sysinfoapi.h>
#elif __linux__
#include "sys/types.h"
#include "sys/sysinfo.h"
#endif

MemoryInfo::MemoryInfo() : BaseInfo()
{
    qDebug() << __FUNCTION__;
}

MemoryInfo::~MemoryInfo()
{
    qDebug() << __FUNCTION__;
}

void MemoryInfo::init()
{
    readTotalMemory();

    setStaticInfo(Globals::SysInfoAttr::Key_Memory_Static_TotalVirtualMemory,m_totalVirtualMemory);
    setStaticInfo(Globals::SysInfoAttr::Key_Memory_Static_TotalPhysicalMemory,m_totalPhysicalMemory);
}

void MemoryInfo::update()
{
    readUsedMemory();

    setDynamicInfo(Globals::SysInfoAttr::Key_Memory_Dynamic_UsedVirtualMemory, m_usedVirtualMemory);
    setDynamicInfo(Globals::SysInfoAttr::Key_Memory_Dynamic_UsedPhysicalMemory, m_usedPhysicalMemory);
}

void MemoryInfo::readTotalMemory()
{
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
    m_totalVirtualMemory = (totalVirtualMem / 1024) / 1024;

    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    m_totalPhysicalMemory = (totalPhysMem / 1024) / 1024;
#elif __linux__
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    long long totalVirtualMem = memInfo.totalram;
    //Add other values in next statement to avoid int overflow on right hand side...
    totalVirtualMem += memInfo.totalswap;
    totalVirtualMem *= memInfo.mem_unit;
    m_totalVirtualMemory = (totalVirtualMem / 1024) / 1024;

    long long totalPhysMem = memInfo.totalram;
    //Multiply in next statement to avoid int overflow on right hand side...
    totalPhysMem *= memInfo.mem_unit;
    m_totalPhysicalMemory = (totalPhysMem / 1024) / 1024;
#endif
}

void MemoryInfo::readUsedMemory()
{
#ifdef _WIN32
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    m_usedVirtualMemory = (virtualMemUsed / 1024) / 1024;

    DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
    m_usedPhysicalMemory = (physMemUsed / 1024) / 1024;
#elif __linux__
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    long long virtualMemUsed = memInfo.totalram - memInfo.freeram;
    //Add other values in next statement to avoid int overflow on right hand side...
    virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
    virtualMemUsed *= memInfo.mem_unit;
    m_usedVirtualMemory = (virtualMemUsed / 1024) / 1024;

    long long physMemUsed = memInfo.totalram - memInfo.freeram;
    //Multiply in next statement to avoid int overflow on right hand side...
    physMemUsed *= memInfo.mem_unit;
    m_usedPhysicalMemory = (physMemUsed / 1024) / 1024;
#endif
}
