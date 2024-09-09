#include "GpuInfoIntel.h"

#include "IgclHandler.h"
#include "ZeroLevelHandler.h"

#include "../../Globals.h"

GpuInfoIntel::GpuInfoIntel()
{
    qDebug() << __FUNCTION__;

    m_igclHandler = new IgclHandler();
    m_zeroLevelHandler = new ZeroLevelHandler();

    m_gpuChipDesigner = "Intel";
}

GpuInfoIntel::~GpuInfoIntel()
{
    qDebug() << __FUNCTION__;
}

bool GpuInfoIntel::init()
{
    m_igclHandler->init();
    m_zeroLevelHandler->init();

    return true;
}

void GpuInfoIntel::readStaticInfo()
{
    qDebug() << __FUNCTION__;

    m_igclHandler->readStaticInfo();
    m_zeroLevelHandler->readStaticInfo();

    //@todo: add missing info
    //m_staticInfo[Globals::SysInfoAttr::Key_Gpu_ChipDesigner] = QString::fromStdString(m_gpuChipDesigner);
    //m_staticInfo[Globals::SysInfoAttr::Key_Gpu_CardManufacturer] = QString::fromStdString(m_gpuCardManufacturer);
    //m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Model] = QString::fromStdString(m_nvapiHandler->gpuModel());
    //m_staticInfo[Globals::SysInfoAttr::Key_Gpu_MemorySize] = m_nvapiHandler->gpuMemorySize();
    //m_staticInfo[Globals::SysInfoAttr::Key_Gpu_MemoryType] = QString::fromStdString(m_nvapiHandler->gpuMemoryType());
    //m_staticInfo[Globals::SysInfoAttr::Key_Gpu_MemoryVendor] = QString::fromStdString(m_nvapiHandler->gpuMemoryVendor());
    //m_staticInfo[Globals::SysInfoAttr::Key_Gpu_DriverInfo] = QString::fromStdString(m_nvapiHandler->gpuDriverInfo());
    //m_staticInfo[Globals::SysInfoAttr::Key_Gpu_DriverVersion] = QString::fromStdString(m_nvapiHandler->gpuDriverVersion());
//
    //m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_StatusSupport_Nvapi] = QVariant::fromValue(m_nvapiHandler->functionsSupportStatus());
    //m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_ErrorMessage_Nvapi] = QVariant::fromValue(m_nvapiHandler->functionsStatusMessage());
//
    //m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_StatusSupport_Nvml] = QVariant::fromValue(m_nvmlHandler->functionsSupportStatus());
    //m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_ErrorMessage_Nvml] = QVariant::fromValue(m_nvmlHandler->functionsStatusMessage());
}

void GpuInfoIntel::readDynamicInfo()
{
    qDebug() << __FUNCTION__;

    m_igclHandler->readDynamicInfo();
    m_zeroLevelHandler->readDynamicInfo();

    //@todo: add missing info
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Usage] = m_nvapiHandler->gpuUsage();
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_ClockSpeed] = m_nvapiHandler->gpuClockSpeed();
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamUsage] = m_nvapiHandler->gpuVramUsage();
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamClockSpeed] = m_nvapiHandler->gpuVramClockSpeed();
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamUsed] = m_nvapiHandler->gpuVramUsed();
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Power] = m_nvmlHandler->gpuPower();
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_TotalBoardPower] = 0;
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Voltage] = 0;
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Temperature] = m_nvapiHandler->gpuTemperature();
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_HotSpotTemperature] = m_nvapiHandler->gpuHotspotTemperature();
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_FanSpeed] = m_nvapiHandler->gpuFanSpeed();
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_FanSpeedUsage] = m_nvapiHandler->gpuFanSpeedUsage();
}