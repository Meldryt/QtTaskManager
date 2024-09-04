#include "GpuInfo.h"

#include "amd/GpuInfoAmd.h"
#include "nvidia/GpuInfoNVidia.h"
#include "GlGlobals.h"

#include <QDebug>

GpuInfo::GpuInfo()
{
    qDebug() << __FUNCTION__;
}

GpuInfo::~GpuInfo()
{
    qDebug() << __FUNCTION__;
}

void GpuInfo::init()
{
    for (uint8_t i = Globals::Key_Gpu_Static_Start + 1; i < Globals::Key_Gpu_Static_End; ++i)
    {
        m_staticInfo[i] = Globals::SysInfo_Uninitialized;
    }

    for (uint8_t i = Globals::Key_Gpu_Dynamic_Start + 1; i < Globals::Key_Gpu_Dynamic_End; ++i)
    {
        m_dynamicInfo[i] = Globals::SysInfo_Uninitialized;
    }

    detectGpu();

    if (m_gpuManufacturer == GpuManufacturer::AMD)
    {
        m_gpuInfoAmd = new GpuInfoAmd();
        m_gpuInfoAmd->init();
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
    const bool isNVidia = GlGlobals::glRenderer.find("NVIDIA") != std::string::npos || GlGlobals::glVendor.find("NVIDIA") != std::string::npos;

    if (isAmd)
    {
        m_gpuManufacturer = GpuManufacturer::AMD;
        m_gpuDetected = true;
        qDebug() << __FUNCTION__ << " : " << "AMD Gpu detected!";
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
        m_staticInfo = m_gpuInfoAmd->staticInfo();
    }
    else if (m_gpuManufacturer == GpuManufacturer::NVIDIA)
    {        
        if(!m_gpuInfoNVidia)
        {
            return;
        }

        m_gpuInfoNVidia->readStaticInfo();
        m_staticInfo = m_gpuInfoNVidia->staticInfo();
    }
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
        m_dynamicInfo = m_gpuInfoAmd->dynamicInfo();
    }
    else if (m_gpuManufacturer == GpuManufacturer::NVIDIA)
    {
        if(!m_gpuInfoNVidia)
        {
            return;
        }

        m_gpuInfoNVidia->readDynamicInfo();
        m_dynamicInfo = m_gpuInfoNVidia->dynamicInfo();
    }
}
