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
    //m_model = m_adlManager->readModel();
    //m_manufacturer = m_adlManager->readManufacturer();
}

void GpuInfoAmd::fetchDynamicInfo()
{
    m_temperature = 0;
    m_totalLoad = 0;
}
