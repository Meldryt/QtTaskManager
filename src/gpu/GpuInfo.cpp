#include "GpuInfo.h"

#include "amd/GpuInfoAmd.h"
#include "intel/GpuInfoIntel.h"
#include "nvidia/GpuInfoNVidia.h"
#include "GlGlobals.h"

#ifdef _WIN32
#else
#include "linux/GpuInfoLinux.h"
#endif

#include <QDebug>

GpuInfo::GpuInfo() : BaseInfo("GpuInfo", InfoType::Gpu)
{
    qDebug() << __FUNCTION__;

#ifdef _WIN32
#else
    m_gpuInfoLinux = new GpuInfoLinux();
#endif
}

GpuInfo::~GpuInfo()
{
    qDebug() << __FUNCTION__;
}

void GpuInfo::init()
{
    detectGpu();

    if (m_gpuManufacturer == GpuManufacturer::AMD)
    {
        m_gpuInfoAmd = new GpuInfoAmd();
        m_gpuInfoAmd->init();
    }
    else if (m_gpuManufacturer == GpuManufacturer::INTEL)
    {
        m_gpuInfoIntel = new GpuInfoIntel();
        m_gpuInfoIntel->init();
    }
    else if (m_gpuManufacturer == GpuManufacturer::NVIDIA)
    {
        m_gpuInfoNVidia = new GpuInfoNVidia();
        m_gpuInfoNVidia->init();
    }
    else
    {
        m_gpuManufacturer = GpuManufacturer::UNKNOWN;
    }

#ifdef _WIN32
#else
    m_gpuInfoLinux->init();
#endif

    if(m_gpuDetected)
    {
        readStaticInfo();
    }
}

void GpuInfo::update()
{
    if(m_gpuDetected)
    {
        readDynamicInfo();
    }
}

void GpuInfo::detectGpu()
{
    const bool isAmd = GlGlobals::glRenderer.find("AMD") != std::string::npos || GlGlobals::glVendor.find("ATI") != std::string::npos;
    const bool isIntel = GlGlobals::glRenderer.find("INTEL") != std::string::npos || GlGlobals::glVendor.find("Intel") != std::string::npos;
    const bool isNVidia = GlGlobals::glRenderer.find("NVIDIA") != std::string::npos || GlGlobals::glVendor.find("Nvidia") != std::string::npos;

    if (isAmd)
    {
        m_gpuManufacturer = GpuManufacturer::AMD;
        m_gpuDetected = true;
        qDebug() << __FUNCTION__ << " : " << "AMD Gpu detected!";
    }
    else if (isIntel)
    {
        m_gpuManufacturer = GpuManufacturer::INTEL;
        m_gpuDetected = true;
        qDebug() << __FUNCTION__ << " : " <<  "INTEL Gpu detected!";
    }
    else if (isNVidia)
    {
        m_gpuManufacturer = GpuManufacturer::NVIDIA;
        m_gpuDetected = true;
        qDebug() << __FUNCTION__ << " : " <<  "NVIDIA Gpu detected!";
    }
    else
    {
        qDebug() << __FUNCTION__ << " : " <<  "No Gpu detected!";
    }
}

void GpuInfo::readStaticInfo()
{
    if (m_gpuManufacturer == GpuManufacturer::AMD)
    {
        if(!m_gpuInfoAmd)
        {
            return;
        }

        m_gpuInfoAmd->readStaticInfo();
        setStaticInfo(m_gpuInfoAmd->staticInfo());
    }
    else if (m_gpuManufacturer == GpuManufacturer::INTEL)
    {        
        if(!m_gpuInfoIntel)
        {
            return;
        }

        m_gpuInfoIntel->readStaticInfo();
        setStaticInfo(m_gpuInfoIntel->staticInfo());
    }
    else if (m_gpuManufacturer == GpuManufacturer::NVIDIA)
    {        
        if(!m_gpuInfoNVidia)
        {
            return;
        }

        m_gpuInfoNVidia->readStaticInfo();
        setStaticInfo(m_gpuInfoNVidia->staticInfo());
    }

#ifdef _WIN32
#else
    if(!m_gpuInfoLinux)
    {
        return;
    }

    m_gpuInfoLinux->readStaticInfo();
    setStaticInfo(m_gpuInfoLinux->staticInfo());
#endif
}

void GpuInfo::readDynamicInfo()
{
    if (m_gpuManufacturer == GpuManufacturer::AMD)
    {
        if(!m_gpuInfoAmd)
        {
            return;
        }

        m_gpuInfoAmd->readDynamicInfo();
        setDynamicInfo(m_gpuInfoAmd->dynamicInfo());
    }
    else if (m_gpuManufacturer == GpuManufacturer::INTEL)
    {
        if(!m_gpuInfoIntel)
        {
            return;
        }

        m_gpuInfoIntel->readDynamicInfo();
        setDynamicInfo(m_gpuInfoIntel->dynamicInfo());
    }
    else if (m_gpuManufacturer == GpuManufacturer::NVIDIA)
    {
        if(!m_gpuInfoNVidia)
        {
            return;
        }

        m_gpuInfoNVidia->readDynamicInfo();
        setDynamicInfo(m_gpuInfoNVidia->dynamicInfo());
    }

#ifdef _WIN32
#else
    if(!m_gpuInfoLinux)
    {
        return;
    }

    m_gpuInfoLinux->readDynamicInfo();
    setDynamicInfo(m_gpuInfoLinux->dynamicInfo());
#endif
}
