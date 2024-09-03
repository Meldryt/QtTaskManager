#include "CpuInfoAmd.h"

#include "RyzenMasterSdkHandler.h"
#include "../../Globals.h"

#include <QDebug>

CpuInfoAmd::CpuInfoAmd()
{
    qDebug() << __FUNCTION__;

    m_ryzenMasterSdkHandler = new RyzenMasterSdkHandler();
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
    if(!m_ryzenMasterSdkHandler)
    {
        return;
    }

    m_ryzenMasterSdkHandler->init();
}

void CpuInfoAmd::readStaticInfo()
{
    if(!m_ryzenMasterSdkHandler)
    {
        return;
    }

    m_ryzenMasterSdkHandler->readStaticInfo();

    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Brand] = QString::fromStdString(m_ryzenMasterSdkHandler->cpuBrand());
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Socket] = QString::fromStdString(m_ryzenMasterSdkHandler->cpuSocket());
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_ProcessorCount] = m_ryzenMasterSdkHandler->cpuProcessorCount();
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_ThreadCount] = Globals::SysInfo_Uninitialized;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_BaseFrequency] = m_ryzenMasterSdkHandler->cpuBaseFrequency();
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_MaxTurboFrequency] = Globals::SysInfo_Uninitialized;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_L1CacheSize] = m_ryzenMasterSdkHandler->cpuL1CacheSize();
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_L2CacheSize] = m_ryzenMasterSdkHandler->cpuL2CacheSize();
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_L3CacheSize] = m_ryzenMasterSdkHandler->cpuL3CacheSize();
    m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_StatusSupport_Wmi] = QVariant::fromValue(m_ryzenMasterSdkHandler->functionsSupportStatus());
}

void CpuInfoAmd::readDynamicInfo()
{
    if(!m_ryzenMasterSdkHandler)
    {
        return;
    }

    m_ryzenMasterSdkHandler->readDynamicInfo();

    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_TotalUsage] = Globals::SysInfo_Uninitialized;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CoreUsages] = Globals::SysInfo_Uninitialized;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CoreFrequencies] = QVariant::fromValue(m_ryzenMasterSdkHandler->cpuCoreFrequencies());
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CurrentMaxFrequency] = m_ryzenMasterSdkHandler->cpuCurrentMaxFrequency();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_ThreadFrequencies] = Globals::SysInfo_Uninitialized;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_ThreadUsages] = Globals::SysInfo_Uninitialized;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CoreVoltage] = m_ryzenMasterSdkHandler->cpuCoreVoltage();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Power] = m_ryzenMasterSdkHandler->cpuPower();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_SocPower] = m_ryzenMasterSdkHandler->cpuSocPower();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Temperature] = m_ryzenMasterSdkHandler->cpuTemperature();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Fanspeed] = Globals::SysInfo_Uninitialized;
}