#include "CpuInfoAmd.h"

#ifdef _WIN32
#include "RyzenMasterSdkHandler.h"
#endif

#include "../../Globals.h"

#include <QDebug>

CpuInfoAmd::CpuInfoAmd()
{
    qDebug() << __FUNCTION__;

#ifdef _WIN32
    m_ryzenMasterSdkHandler = new RyzenMasterSdkHandler();
#endif

    for (uint8_t key = Globals::Key_Cpu_Static_Start + 1; key < Globals::Key_Cpu_Static_End; ++key)
    {
        m_staticInfo[key] = Globals::SysInfo_Uninitialized;
    }
    for (uint8_t key = Globals::Key_Cpu_Dynamic_Start + 1; key < Globals::Key_Cpu_Dynamic_End; ++key)
    {
        m_dynamicInfo[key] = Globals::SysInfo_Uninitialized;
    }
}

CpuInfoAmd::~CpuInfoAmd()
{
    qDebug() << __FUNCTION__;
}

const QMap<uint8_t,QVariant>& CpuInfoAmd::staticInfo() const
{
    return m_staticInfo;
}

const QMap<uint8_t,QVariant>& CpuInfoAmd::dynamicInfo() const
{
    return m_dynamicInfo;
}

void CpuInfoAmd::init()
{

    for (uint8_t i = Globals::Key_Cpu_Static_Start + 1; i < Globals::Key_Cpu_Static_End; ++i)
    {
        m_staticInfo[i] = Globals::SysInfo_Uninitialized;
    }

    for (uint8_t i = Globals::Key_Cpu_Dynamic_Start + 1; i < Globals::Key_Cpu_Dynamic_End; ++i)
    {
        m_dynamicInfo[i] = Globals::SysInfo_Uninitialized;
    }

#ifdef _WIN32
    if(!m_ryzenMasterSdkHandler)
    {
        return;
    }

    m_ryzenMasterSdkHandler->init();
#endif
}

void CpuInfoAmd::readStaticInfo()
{

#ifdef _WIN32
    if(!m_ryzenMasterSdkHandler)
    {
        return;
    }

    m_ryzenMasterSdkHandler->readStaticInfo();

    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_Brand] = QString::fromStdString(m_ryzenMasterSdkHandler->cpuBrand());
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_Socket] = QString::fromStdString(m_ryzenMasterSdkHandler->cpuSocket());
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_CoreCount] = m_ryzenMasterSdkHandler->cpuCoreCount();
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_ThreadCount] = Globals::SysInfo_Uninitialized;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_BaseFrequency] = m_ryzenMasterSdkHandler->cpuBaseFrequency();
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_L1CacheSize] = m_ryzenMasterSdkHandler->cpuL1CacheSize();
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_L2CacheSize] = m_ryzenMasterSdkHandler->cpuL2CacheSize();
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Static_L3CacheSize] = m_ryzenMasterSdkHandler->cpuL3CacheSize();
    m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_StatusSupport_Wmi] = QVariant::fromValue(m_ryzenMasterSdkHandler->functionsSupportStatus());
#endif
}

void CpuInfoAmd::readDynamicInfo()
{

#ifdef _WIN32
    if(!m_ryzenMasterSdkHandler)
    {
        return;
    }

    m_ryzenMasterSdkHandler->readDynamicInfo();

    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_CoreFrequencies] = QVariant::fromValue(m_ryzenMasterSdkHandler->cpuCoreFrequencies());
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_CurrentMaxFrequency] = m_ryzenMasterSdkHandler->cpuCurrentMaxFrequency();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_CoreVoltage] = m_ryzenMasterSdkHandler->cpuCoreVoltage();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_Power] = m_ryzenMasterSdkHandler->cpuPower();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_SocPower] = m_ryzenMasterSdkHandler->cpuSocPower();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_Temperature] = m_ryzenMasterSdkHandler->cpuTemperature();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Dynamic_Fanspeed] = Globals::SysInfo_Uninitialized;
#endif
}
