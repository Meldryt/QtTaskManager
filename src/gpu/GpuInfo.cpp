#include "GpuInfo.h"
#include "GpuInfoNVidia.h"
#include "GpuInfoAmd.h"

#include "GlGlobals.h"

#include <QDebug>

GpuInfo::GpuInfo()
{
}

GpuInfo::~GpuInfo()
{
}

void GpuInfo::init()
{
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
        qDebug() << "GpuInfo::detectGpu(): " << "AMD Gpu detected!";
    }
    else if (isNVidia)
    {
        m_gpuManufacturer = GpuManufacturer::NVIDIA;
        m_gpuDetected = true;
        qDebug() << "GpuInfo::detectGpu(): " << "NVIDIA Gpu detected!";
    }
    else
    {
        m_gpuManufacturer = GpuManufacturer::UNKNOWN;
        m_gpuDetected = false;
        qDebug() << "GpuInfo::detectGpu(): " << "No Gpu detected!";
    }
}

void GpuInfo::readStaticInfo()
{
    if (m_gpuManufacturer == GpuManufacturer::AMD)
    {
        m_gpuInfoAmd->readStaticInfo();
        m_staticInfo = m_gpuInfoAmd->staticInfo();
    }
    else if (m_gpuManufacturer == GpuManufacturer::NVIDIA)
    {
        m_gpuInfoNVidia->readStaticInfo();
        m_staticInfo = m_gpuInfoNVidia->staticInfo();
    }
}

void GpuInfo::readDynamicInfo()
{
    if (m_gpuManufacturer == GpuManufacturer::AMD)
    {
        m_gpuInfoAmd->readDynamicInfo();
        m_dynamicInfo = m_gpuInfoAmd->dynamicInfo();
    }
    else if (m_gpuManufacturer == GpuManufacturer::NVIDIA)
    {
        m_gpuInfoNVidia->readDynamicInfo();
        m_dynamicInfo = m_gpuInfoNVidia->dynamicInfo();
    }
}
