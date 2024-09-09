#include "CpuInfoIntel.h"

#include "PcmHandler.h"
#include "../../Globals.h"

CpuInfoIntel::CpuInfoIntel()
{
    qDebug() << __FUNCTION__;

    m_pcmHandler = std::make_unique<PcmHandler>();

    for (uint8_t key = Globals::Key_Cpu_Static_Start + 1; key < Globals::Key_Cpu_Static_End; ++key)
    {
        m_staticInfo[key] = Globals::SysInfo_Uninitialized;
    }
    for (uint8_t key = Globals::Key_Cpu_Dynamic_Start + 1; key < Globals::Key_Cpu_Dynamic_End; ++key)
    {
        m_dynamicInfo[key] = Globals::SysInfo_Uninitialized;
    }
}

CpuInfoIntel::~CpuInfoIntel()
{
    qDebug() << __FUNCTION__;
}

const QMap<uint8_t, QVariant>& CpuInfoIntel::staticInfo() const
{
    return m_staticInfo;
}

const QMap<uint8_t, QVariant>& CpuInfoIntel::dynamicInfo() const
{
    return m_dynamicInfo;
}

void CpuInfoIntel::init()
{
}

void CpuInfoIntel::readStaticInfo()
{
    m_pcmHandler->init();

    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Brand] = QString::fromStdString(m_pcmHandler->cpuBrand());
    //m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Socket] = m_pcmHandler->cpuThreadCount();
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_CoreCount] = m_pcmHandler->cpuCoreCount();
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_ThreadCount] = m_pcmHandler->cpuThreadCount();
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_ThermalDesignPower] = m_pcmHandler->cpuThermalDesignPower();
}

void CpuInfoIntel::readDynamicInfo()
{
    m_pcmHandler->update();


    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CoreUsages] = QVariant::fromValue(m_pcmHandler->cpuCoreUsages());
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CoreFrequencies] = QVariant::fromValue(m_pcmHandler->cpuCoreFrequencies());
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Power] = m_pcmHandler->cpuPackagePower();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Temperature] = m_pcmHandler->cpuTemperature();
}