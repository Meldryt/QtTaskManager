#include "GpuInfoAmd.h"

#include <windows.h>

#include <amd_ags.h>

//#pragma comment(lib, "amd_ags_x64.lib")
#pragma comment(lib, "amd_ags_x64_2022_MDd.lib")

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
        printf("\nAGS Library initialized: v%d.%d.%d\n", AMD_AGS_VERSION_MAJOR, AMD_AGS_VERSION_MINOR, AMD_AGS_VERSION_PATCH);
        printf("-----------------------------------------------------------------\n");

        printf("Radeon Software Version:   %s\n", gpuInfo.radeonSoftwareVersion);
        printf("Driver Version:            %s\n", gpuInfo.driverVersion);
        printf("-----------------------------------------------------------------\n");
        //PrintDisplayInfo(gpuInfo);
        printf("-----------------------------------------------------------------\n");

        if (0)
        {
            //printf("\n");
            //testDriver(gpuInfo.radeonSoftwareVersion, AGS_MAKE_VERSION(20, 1, 0));
            //testDriver("18.8.randombetadriver", AGS_MAKE_VERSION(18, 8, 2));
            //testDriver("18.8.123randomdriver", AGS_MAKE_VERSION(18, 8, 2));
            //testDriver("18.9.randomdriver", AGS_MAKE_VERSION(18, 8, 2));
            //testDriver("18.8.2", AGS_MAKE_VERSION(18, 8, 2));
            //testDriver("18.8.2", AGS_MAKE_VERSION(18, 8, 1));
            //testDriver("18.8.2", AGS_MAKE_VERSION(18, 8, 3));
            //printf("\n");
        }

        if (agsDeInitialize(agsContext) != AGS_SUCCESS)
        {
            printf("Failed to cleanup AGS Library\n");
        }
    }
    else
    {
        printf("Failed to initialize AGS Library\n");
    }
}
