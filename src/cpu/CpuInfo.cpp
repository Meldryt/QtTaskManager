#include "CpuInfo.h"

#include "../Globals.h"

#ifdef _WIN32
#include "windows/CpuInfoWindows.h"
#elif __linux__
#include "linux/CpuInfoLinux.h"
#endif
#include "amd/CpuInfoAmd.h"
#include "intel/CpuInfoIntel.h"

CpuInfo::CpuInfo()
{
    qDebug() << __FUNCTION__;
}

CpuInfo::~CpuInfo()
{
    qDebug() << __FUNCTION__;
}

void CpuInfo::init()
{
    for (uint8_t i = Globals::Key_Cpu_Static_Start + 1; i < Globals::Key_Cpu_Static_End; ++i)
    {
        m_staticInfo[i] = Globals::SysInfo_Uninitialized;
    }

    for (uint8_t i = Globals::Key_Cpu_Dynamic_Start + 1; i < Globals::Key_Cpu_Dynamic_End; ++i)
    {
        m_dynamicInfo[i] = Globals::SysInfo_Uninitialized;
    }

#ifdef _WIN32
    m_cpuInfoWindows = new CpuInfoWindows();
    m_cpuInfoWindows->init();
#elif __linux__
    m_cpuInfoLinux = new CpuInfoLinux();
    m_cpuInfoLinux->init();
#endif

    detectCpu();

    if (m_cpuManufacturer == CpuManufacturer::AMD)
    {
        m_cpuInfoAmd = new CpuInfoAmd();
        m_cpuInfoAmd->init();
    }
    else if (m_cpuManufacturer == CpuManufacturer::INTEL)
    {
        m_cpuInfoIntel = new CpuInfoIntel();
        m_cpuInfoIntel->init();
    }

    if(m_cpuDetected)
    {
        readStaticInfo();
    }
}

void CpuInfo::update()
{
    if(m_cpuDetected)
    {
        readDynamicInfo();
    }
}

void CpuInfo::detectCpu()
{
    std::string cpuBrand;
#ifdef _WIN32
    if(m_cpuInfoWindows)
    {
        cpuBrand = m_cpuInfoWindows->cpuBrand();
    }
#elif __linux__
    if(m_cpuInfoLinux)
    {
        cpuBrand = m_cpuInfoLinux->cpuBrand();
    }
#endif

    const bool isAmd = cpuBrand.find("AMD") != std::string::npos || cpuBrand.find("Amd") != std::string::npos;
    const bool isIntel = cpuBrand.find("INTEL") != std::string::npos || cpuBrand.find("Intel") != std::string::npos;

    if (isAmd)
    {
        m_cpuManufacturer = CpuManufacturer::AMD;
        m_cpuDetected = true;
        qDebug() << __FUNCTION__ << " : " << "AMD Cpu detected!";
    }
    else if (isIntel)
    {
        m_cpuManufacturer = CpuManufacturer::INTEL;
        m_cpuDetected = true;
        qDebug() << __FUNCTION__ << " : " <<  "INTEL Cpu detected!";
    }
    else
    {
        qDebug() << __FUNCTION__ << " : " <<  "No Cpu detected!";
    }
}

void CpuInfo::readStaticInfo()
{
    QMap<uint8_t, QVariant> staticInfoOs;
#ifdef _WIN32
    if(m_cpuInfoWindows)
    {
        m_cpuInfoWindows->readStaticInfo();
        staticInfoOs = m_cpuInfoWindows->staticInfo();
    }
#elif __linux__
    if(m_cpuInfoLinux)
    {
        m_cpuInfoLinux->readStaticInfo();
        staticInfoOs = m_cpuInfoLinux->staticInfo();
    }
#endif

    if (m_cpuManufacturer == CpuManufacturer::AMD)
    {
        if(!m_cpuInfoAmd)
        {
            return;
        }

        m_cpuInfoAmd->readStaticInfo();
        m_staticInfo = m_cpuInfoAmd->staticInfo();
    }
    else if (m_cpuManufacturer == CpuManufacturer::INTEL)
    {        
        if(!m_cpuInfoIntel)
        {
            return;
        }

        m_cpuInfoIntel->readStaticInfo();
        m_staticInfo = m_cpuInfoIntel->staticInfo();
    }

    for (uint8_t i = Globals::Key_Cpu_Static_Start + 1; i < Globals::Key_Cpu_Static_End; ++i)
    {
        if (m_staticInfo[i].isValid() && m_staticInfo[i].canConvert<int8_t>() &&
            m_staticInfo[i].value<int8_t>() == Globals::SysInfo_Uninitialized &&
            staticInfoOs[i] != m_staticInfo[i])
        {
            m_staticInfo[i] = staticInfoOs[i];
        }
    }
}

void CpuInfo::readDynamicInfo()
{
    QMap<uint8_t, QVariant> dynamicInfoOs;
#ifdef _WIN32
    if(m_cpuInfoWindows)
    {
        m_cpuInfoWindows->readDynamicInfo();
        dynamicInfoOs = m_cpuInfoWindows->dynamicInfo();
    }
#elif __linux__
    if(m_cpuInfoLinux)
    {
        m_cpuInfoLinux->readDynamicInfo();
        dynamicInfoOs = m_cpuInfoLinux->dynamicInfo();
    }
#endif

    if (m_cpuManufacturer == CpuManufacturer::AMD)
    {
        if(!m_cpuInfoAmd)
        {
            return;
        }

        m_cpuInfoAmd->readDynamicInfo();
        m_dynamicInfo = m_cpuInfoAmd->dynamicInfo();
    }
    else if (m_cpuManufacturer == CpuManufacturer::INTEL)
    {
        if(!m_cpuInfoIntel)
        {
            return;
        }

        m_cpuInfoIntel->readDynamicInfo();
        m_dynamicInfo = m_cpuInfoIntel->dynamicInfo();
    }

    for (uint8_t i = Globals::Key_Cpu_Dynamic_Start + 1; i < Globals::Key_Cpu_Dynamic_End; ++i)
    {
        if (m_dynamicInfo[i].isValid() && m_dynamicInfo[i].canConvert<int8_t>() &&
            m_dynamicInfo[i].value<int8_t>() == Globals::SysInfo_Uninitialized &&
            dynamicInfoOs[i] != m_dynamicInfo[i])
        {
            m_dynamicInfo[i] = dynamicInfoOs[i];
        }
    }
}
