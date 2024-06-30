#include "GpuInfoAmd.h"

#include <windows.h>

#include <amd_ags.h>

#include <QDebug>

//#pragma comment(lib, "amd_ags_x64.lib")
//#pragma comment(lib, "amd_ags_x64_2022_MDd.lib")

GpuInfoAmd::GpuInfoAmd()
{
    m_adlxManager = new AdlxManager();
}

bool GpuInfoAmd::detectGpu()
{
    m_adlxManager->init();
    //m_adlManager->fetchInfo();
    return true;
}

void GpuInfoAmd::fetchStaticInfo()
{
    m_adlxManager->fetchStaticInfo();
    m_staticInfo = m_adlxManager->staticInfo();

    initAgs();
}

void GpuInfoAmd::fetchDynamicInfo()
{
    m_adlxManager->fetchDynamicInfo();
}

void GpuInfoAmd::initAgs()
{
    AGSContext* agsContext = nullptr;
    AGSGPUInfo gpuInfo = {};
    AGSConfiguration config = {};

    if (agsInitialize(AGS_CURRENT_VERSION, &config, &agsContext, &gpuInfo) == AGS_SUCCESS)
    {
        qDebug() << "Radeon Software Version: " << gpuInfo.radeonSoftwareVersion;
        qDebug() << "Driver Version:          " << gpuInfo.driverVersion;

        m_staticInfo.driverInfo = gpuInfo.driverVersion;
        m_staticInfo.driverVersion = gpuInfo.radeonSoftwareVersion;

        if (agsDeInitialize(agsContext) != AGS_SUCCESS)
        {
            qDebug() << "Failed to cleanup AGS Library";
        }
    }
}
