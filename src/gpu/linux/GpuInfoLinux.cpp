#include "GpuInfoLinux.h"

#include "../../Globals.h"
#include "../../Utils.h"

GpuInfoLinux::GpuInfoLinux()
{
    qDebug() << __FUNCTION__;

    m_gpuChipDesigner = "Intel";

    for (uint8_t key = Globals::Key_Gpu_Static_Start + 1; key < Globals::Key_Gpu_Static_End; ++key)
    {
        m_staticInfo[key] = Globals::SysInfo_Uninitialized;
    }
    for (uint8_t key = Globals::Key_Gpu_Dynamic_Start + 1; key < Globals::Key_Gpu_Dynamic_End; ++key)
    {
        m_dynamicInfo[key] = Globals::SysInfo_Uninitialized;
    }
}

GpuInfoLinux::~GpuInfoLinux()
{
    qDebug() << __FUNCTION__;
}

bool GpuInfoLinux::init()
{
    const std::string findDevices = "find /sys/devices/pci* -type f -name ";
    m_gpuInfoPathMap["gt_min_freq_mhz"] = Utils::exec(findDevices + "gt_min_freq_mhz");
    m_gpuInfoPathMap["gt_max_freq_mhz"] = Utils::exec(findDevices + "gt_max_freq_mhz");
    m_gpuInfoPathMap["gt_act_freq_mhz"] = Utils::exec(findDevices + "gt_act_freq_mhz");
    m_gpuInfoPathMap["gt_cur_freq_mhz"] = Utils::exec(findDevices + "gt_cur_freq_mhz");
    m_gpuInfoPathMap["gt_boost_freq_mhz"] = Utils::exec(findDevices + "gt_boost_freq_mhz");
    m_gpuInfoPathMap["gt_RP0_freq_mhz"] = Utils::exec(findDevices + "gt_RP0_freq_mhz");
    m_gpuInfoPathMap["gt_RP1_freq_mhz"] = Utils::exec(findDevices + "gt_RP1_freq_mhz");

    return true;
}

void GpuInfoLinux::readStaticInfo()
{
    //@todo: add missing info
    m_gpuModel = Utils::exec("lspci | grep VGA | cut -d ':' -f3");
    const std::string strBaseClock = Utils::exec("cat " + m_gpuInfoPathMap["gt_RP1_freq_mhz"]);
    if(!strBaseClock.empty())
    {
        m_gpuBaseClock= std::stoi(strBaseClock);
    }

    const std::string strBoostClock = Utils::exec("cat " + m_gpuInfoPathMap["gt_boost_freq_mhz"]);
    if(!strBoostClock.empty())
    {
        m_gpuBoostClock = std::stoi(strBoostClock);
    }

    //m_staticInfo[Globals::SysInfoAttr::Key_Gpu_ChipDesigner] = QString::fromStdString(m_gpuChipDesigner);
    //m_staticInfo[Globals::SysInfoAttr::Key_Gpu_CardManufacturer] = QString::fromStdString(m_gpuCardManufacturer);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_Model] = QString::fromStdString(m_gpuModel);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_BaseClock] = m_gpuBaseClock;
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_BoostClock] = m_gpuBoostClock;
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

void GpuInfoLinux::readDynamicInfo()
{
    const std::string strCurrentFrequency = Utils::exec("cat " + m_gpuInfoPathMap["gt_cur_freq_mhz"]);
    if(!strCurrentFrequency.empty())
    {
        m_gpuClockSpeed = std::stoi(strCurrentFrequency);
    }

    //@todo: add missing info
    //m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Usage] = m_nvapiHandler->gpuUsage();
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Dynamic_ClockSpeed] = m_gpuClockSpeed;
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

void GpuInfoLinux::readGpuModel()
{

}
