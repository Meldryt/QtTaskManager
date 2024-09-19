#include "CpuInfoLinux.h"

#include "../../Globals.h"

#ifdef __linux__
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

#include <QDebug>

CpuInfoLinux::CpuInfoLinux()
{
    qDebug() << __FUNCTION__;

    for (uint8_t key = Globals::Key_Cpu_Static_Start + 1; key < Globals::Key_Cpu_Static_End; ++key)
    {
        m_staticInfo[key] = Globals::SysInfo_Uninitialized;
    }
    for (uint8_t key = Globals::Key_Cpu_Dynamic_Start + 1; key < Globals::Key_Cpu_Dynamic_End; ++key)
    {
        m_dynamicInfo[key] = Globals::SysInfo_Uninitialized;
    }
}

CpuInfoLinux::~CpuInfoLinux()
{
    qDebug() << __FUNCTION__;
}

const QMap<uint8_t, QVariant>& CpuInfoLinux::staticInfo() const
{
    return m_staticInfo;
}

const QMap<uint8_t, QVariant>& CpuInfoLinux::dynamicInfo() const
{
    return m_dynamicInfo;
}

void CpuInfoLinux::init()
{
    readCpuVendor();
}

void CpuInfoLinux::readStaticInfo()
{
    readCpuBrand();
    readCpuCoreCount();
    readCpuBaseFrequency();
    readCpuMaxFrequency();

#ifdef __linux__
    m_cpuThreadCount = sysconf( _SC_NPROCESSORS_ONLN );
    m_cpuThreadFrequencies.resize(m_cpuThreadCount);
    m_cpuThreadUsages.resize(m_cpuThreadCount);
    m_cpuL1CacheSize = m_cpuCoreCount * (sysconf( _SC_LEVEL1_DCACHE_SIZE ) + sysconf( _SC_LEVEL1_ICACHE_SIZE )) / 1024;
    m_cpuL2CacheSize = m_cpuCoreCount * sysconf( _SC_LEVEL2_CACHE_SIZE ) / 1024;
    m_cpuL3CacheSize = sysconf( _SC_LEVEL3_CACHE_SIZE ) / 1024;

#endif

    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_Brand] = QString::fromStdString(m_cpuBrand);
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_Socket] = QString::fromStdString(m_cpuSocket);
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_CoreCount] = m_cpuCoreCount;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_ThreadCount] = m_cpuThreadCount;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_BaseFrequency] = m_cpuBaseFrequency;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_MaxTurboFrequency] = m_cpuMaxTurboFrequency;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_L1CacheSize] = m_cpuL1CacheSize;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_L2CacheSize] = m_cpuL2CacheSize;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_L3CacheSize] = m_cpuL3CacheSize;
    //m_staticInfo[Globals::SysInfoAttr::.-Key_Api_Functions_StatusSupport_Wmi] = QVariant::fromValue(m_functionsSupportStatus);
}

void CpuInfoLinux::readDynamicInfo()
{
    readCpuCoreFrequencies();
    readCpuTemperature();

    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_TotalUsage] = m_cpuTotalUsage;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_CoreUsages] = QVariant::fromValue(m_cpuCoreUsages);
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_CoreFrequencies] = QVariant::fromValue(m_cpuCoreFrequencies);
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_CurrentMaxFrequency] = m_cpuCurrentMaxFrequency;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadFrequencies] = QVariant::fromValue(m_cpuThreadFrequencies);
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadUsages] = QVariant::fromValue(m_cpuThreadUsages);
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_CoreVoltage] = m_cpuCoreVoltage;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_Power] = m_cpuPower;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_SocPower] = m_cpuSocPower;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_Temperature] = m_cpuTemperature;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_Fanspeed] = Globals::SysInfo_Uninitialized;
}

void CpuInfoLinux::readCpuVendor()
{
#ifdef __linux__
    FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
    char *arg = 0;
    size_t size = 0;
    while(getdelim(&arg, &size, '\n', cpuinfo) != -1)
    {
        const std::string line = std::string(arg);
        if(line.find("vendor_id") != std::string::npos)
        {
            if(line.find("Intel") != std::string::npos) //normally its "GenuineIntel"
            {
                m_cpuBrand = "Intel";
            }
            else if(line.find("AMD") != std::string::npos) //normally its "AuthenticAMD"
            {
                m_cpuBrand = "AMD";
            }
            break;
        }
    }
    free(arg);
    fclose(cpuinfo);

#endif
}

void CpuInfoLinux::readCpuBrand()
{
#ifdef __linux__
    FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
    char *arg = 0;
    size_t size = 0;
    while(getdelim(&arg, &size, '\n', cpuinfo) != -1)
    {
        const std::string line = std::string(arg);
        if(line.find("model name") != std::string::npos)
        {
            const uint32_t offset = std::string(": ").length();
            const uint32_t first = line.find(": ");
            const uint32_t last = line.find("\n");
            const std::string subString = line.substr(first+offset,last-(offset+first));
            m_cpuBrand = subString;
            break;
        }
    }
    free(arg);
    fclose(cpuinfo);
#endif
}

void CpuInfoLinux::readCpuBaseFrequency()
{
#ifdef __linux__
    FILE *cpuinfo = fopen("/sys/devices/system/cpu/cpu0/cpufreq/base_frequency", "rb");
    if(cpuinfo != NULL)
    {
        uint32_t frequency;
        fscanf(cpuinfo, "%d", &frequency);
        fclose(cpuinfo);
        m_cpuBaseFrequency = frequency / 1000;
    }

#endif
}

void CpuInfoLinux::readCpuMaxFrequency()
{
#ifdef __linux__
    FILE *cpuinfo = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "rb");
    if(cpuinfo != NULL)
    {
        uint32_t frequency;
        fscanf(cpuinfo, "%d", &frequency);
        fclose(cpuinfo);
        m_cpuMaxTurboFrequency = frequency / 1000;
    }

#endif
}

void CpuInfoLinux::readCpuCoreCount()
{
#ifdef __linux__
    FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
    char *arg = 0;
    size_t size = 0;
    while(getdelim(&arg, &size, '\n', cpuinfo) != -1)
    {
        const std::string line = std::string(arg);
        if(line.find("cpu cores") != std::string::npos)
        {
            const uint32_t offset = std::string(": ").length();
            const uint32_t first = line.find(": ");
            const uint32_t last = line.find("\n");
            const std::string subString = line.substr(first+offset,last-(offset+first));
            m_cpuCoreCount = std::stoi(subString);
            m_cpuCoreFrequencies.resize(m_cpuCoreCount);
            m_cpuCoreUsages.resize(m_cpuCoreCount);
            break;
        }
    }
    free(arg);
    fclose(cpuinfo);
#endif
}

void CpuInfoLinux::readCpuCoreFrequencies()
{
#ifdef __linux__
    FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
    char *arg = 0;
    size_t size = 0;
    uint32_t currentCoreId = 0;
    const uint32_t offset = std::string(": ").length();

    m_cpuCurrentMaxFrequency = 0.0;
    m_cpuTotalUsage = 0.0;

    while(getdelim(&arg, &size, '\n', cpuinfo) != -1)
    {
        const std::string line = std::string(arg);
        if(line.find("cpu MHz") != std::string::npos)
        {
            const uint32_t first = line.find(": ");
            const uint32_t last = line.find("\n");
            const std::string subString = line.substr(first+offset,last-(offset+first));
            const double threadFrequency = std::stod(subString);
            const uint32_t currentPhysicalId = currentCoreId / 2;

            m_cpuThreadFrequencies.at(currentCoreId)=threadFrequency;
            m_cpuCoreFrequencies.at(currentPhysicalId)=threadFrequency;
            m_cpuThreadUsages.at(currentCoreId)=(threadFrequency/m_cpuMaxTurboFrequency)*100.0;
            m_cpuCoreUsages.at(currentPhysicalId)=(threadFrequency/m_cpuMaxTurboFrequency)*100.0;

            if(threadFrequency > m_cpuCurrentMaxFrequency)
            {
                m_cpuCurrentMaxFrequency = threadFrequency;
            }

            ++currentCoreId;
        }
    }
    free(arg);
    fclose(cpuinfo);

    for(auto&& coreUsage : m_cpuCoreUsages)
    {
        m_cpuTotalUsage += coreUsage;
    }
    m_cpuTotalUsage /= m_cpuCoreUsages.size();
#endif
}

void CpuInfoLinux::readCpuTemperature()
{
#ifdef __linux__
    FILE *cpuinfo = fopen("/sys/class/thermal/thermal_zone0/temp", "rb");
    if(cpuinfo != NULL)
    {
        uint32_t temperature;
        fscanf(cpuinfo, "%d", &temperature);
        fclose(cpuinfo);
        m_cpuTemperature = temperature / 1000;
    }
#endif
}
