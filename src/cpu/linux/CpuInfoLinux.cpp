#include "CpuInfoLinux.h"

#include "../../Globals.h"

#ifdef __linux__
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#endif

#include <QDebug>

CpuInfoLinux::CpuInfoLinux()
{
    qDebug() << __FUNCTION__;
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
    readCpuBrand();
}

void CpuInfoLinux::readStaticInfo()
{
#ifdef __linux__
    FILE *cpuinfo = fopen("/proc/cpuinfo", "rb");
    //char *arg = 0;
    size_t size = 0;
    while(getdelim(&arg, &size, 0, cpuinfo) != -1)
    {
        const std::string line = std::string(arg);
        if(line.find("Intel") != line.end())
        {
            m_cpuBrand = "Intel";
            break;
        }
        else if(line.find("AuthenticAMD") != line.end() || line.find("AMD") != line.end())
        {
            m_cpuBrand = "AMD";
            break;
        }
        //puts(arg);
    }
   //free(arg);
   fclose(cpuinfo);
#endif
}

void CpuInfoLinux::readDynamicInfo()
{
}

void CpuInfoLinux::readCpuBrand()
{
}