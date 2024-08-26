#include "GpuInfoAmd.h"

#include <windows.h>

#include <amd_ags.h>

#include <QDebug>

//#pragma comment(lib, "amd_ags_x64.lib")
//#pragma comment(lib, "amd_ags_x64_2022_MDd.lib")

GpuInfoAmd::GpuInfoAmd()
{
    qDebug() << __FUNCTION__;

    m_adlxManager = new AdlxManager();
}

GpuInfoAmd::~GpuInfoAmd()
{
    qDebug() << __FUNCTION__;
}

bool GpuInfoAmd::init()
{
    return m_adlxManager->init();
    //m_adlManager->fetchInfo();
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

        m_staticInfo[Globals::SysInfoAttr::Key_Gpu_DriverInfo] = QString::fromStdString(gpuInfo.driverVersion);
        m_staticInfo[Globals::SysInfoAttr::Key_Gpu_DriverVersion] = QString::fromStdString(gpuInfo.radeonSoftwareVersion);

        if (agsDeInitialize(agsContext) != AGS_SUCCESS)
        {
            qDebug() << "Failed to cleanup AGS Library";
        }
    }
}
