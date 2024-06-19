#include "GpuInfoAmd.h"

#include <windows.h>

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
