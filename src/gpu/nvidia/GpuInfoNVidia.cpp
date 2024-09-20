#include "GpuInfoNVidia.h"

#ifdef _WIN32
#include "NvapiHandler.h"
#include "NvmlHandler.h"
#endif

#include "../../Globals.h"

GpuInfoNVidia::GpuInfoNVidia()
{
    qDebug() << __FUNCTION__;

#ifdef _WIN32
    m_nvapiHandler = new NvapiHandler();
    m_nvmlHandler = new NvmlHandler();
#endif
    m_gpuChipDesigner = "NVIDIA";
}

GpuInfoNVidia::~GpuInfoNVidia()
{
    qDebug() << __FUNCTION__;
}

bool GpuInfoNVidia::init()
{
    for (uint8_t i = Globals::Key_Gpu_Static_Start + 1; i < Globals::Key_Gpu_Static_End; ++i)
    {
        m_staticInfo[i] = Globals::SysInfo_Uninitialized;
    }

    for (uint8_t i = Globals::Key_Gpu_Dynamic_Start + 1; i < Globals::Key_Gpu_Dynamic_End; ++i)
    {
        m_dynamicInfo[i] = Globals::SysInfo_Uninitialized;
    }

#ifdef _WIN32
    m_nvapiHandler->init();
    m_nvmlHandler->init();
#endif
    return true;
}

void GpuInfoNVidia::readStaticInfo()
{
    qDebug() << __FUNCTION__;

#ifdef _WIN32
    m_nvapiHandler->readStaticInfo();
    m_nvmlHandler->readStaticInfo();

    //@todo: add missing info
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_ChipDesigner] = QString::fromStdString(m_gpuChipDesigner);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_CardManufacturer] = QString::fromStdString(m_gpuCardManufacturer);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_Model] = QString::fromStdString(m_nvapiHandler->gpuModel());
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_MemorySize] = m_nvapiHandler->gpuMemorySize();
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_MemoryType] = QString::fromStdString(m_nvapiHandler->gpuMemoryType());
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_MemoryVendor] = QString::fromStdString(m_nvapiHandler->gpuMemoryVendor());
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_DriverInfo] = QString::fromStdString(m_nvapiHandler->gpuDriverInfo());
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_DriverVersion] = QString::fromStdString(m_nvapiHandler->gpuDriverVersion());

    m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_StatusSupport_Nvapi] = QVariant::fromValue(m_nvapiHandler->functionsSupportStatus());
    m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_ErrorMessage_Nvapi] = QVariant::fromValue(m_nvapiHandler->functionsStatusMessage());

    m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_StatusSupport_Nvml] = QVariant::fromValue(m_nvmlHandler->functionsSupportStatus());
    m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_ErrorMessage_Nvml] = QVariant::fromValue(m_nvmlHandler->functionsStatusMessage());
#endif
}

void GpuInfoNVidia::readDynamicInfo()
{
    qDebug() << __FUNCTION__;


#ifdef _WIN32
    m_nvapiHandler->readDynamicInfo();
    m_nvmlHandler->readDynamicInfo();

    //@todo: add missing info
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_Usage] = m_nvapiHandler->gpuUsage();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_ClockSpeed] = m_nvapiHandler->gpuClockSpeed();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_VRamUsage] = m_nvapiHandler->gpuVramUsage();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_VRamClockSpeed] = m_nvapiHandler->gpuVramClockSpeed();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_VRamUsed] = m_nvapiHandler->gpuVramUsed();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_Power] = m_nvmlHandler->gpuPower();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_TotalBoardPower] = 0;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_Voltage] = 0;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_Temperature] = m_nvapiHandler->gpuTemperature();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_HotSpotTemperature] = m_nvapiHandler->gpuHotspotTemperature();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_FanSpeed] = m_nvapiHandler->gpuFanSpeed();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_FanSpeedUsage] = m_nvapiHandler->gpuFanSpeedUsage();
#endif
}
