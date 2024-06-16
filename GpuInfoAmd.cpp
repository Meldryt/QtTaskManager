#include "GpuInfoAmd.h"

#include <windows.h>

GpuInfoAmd::GpuInfoAmd()
{
	m_adlManager = new AdlManager();
}

bool GpuInfoAmd::detectGpu()
{
    m_adlManager->init();
    //m_adlManager->fetchInfo();
    return true;
}

void GpuInfoAmd::fetchStaticInfo()
{
    m_adlManager->fetchStaticInfo();
}

void GpuInfoAmd::fetchDynamicInfo()
{
    m_adlManager->fetchDynamicInfo();
}
