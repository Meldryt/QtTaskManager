#pragma once

#include <stdint.h>

class Globals
{
public:

    //enum used for static and dynamic system info map QMap<uint8_t, QVariant>
    static const int8_t SysInfo_Uninitialized{-1};

    enum SysInfoAttr : uint8_t
    {
        Key_Start = 0,

        Key_Cpu_Static_Start,
        Key_Cpu_Static_Brand,
        Key_Cpu_Static_Socket,
        Key_Cpu_Static_CoreCount,
        Key_Cpu_Static_ThreadCount,
        Key_Cpu_Static_BaseFrequency,
        Key_Cpu_Static_MaxTurboFrequency,
        Key_Cpu_Static_L1CacheSize,
        Key_Cpu_Static_L2CacheSize,
        Key_Cpu_Static_L3CacheSize,
        Key_Cpu_Static_ThermalDesignPower,
        Key_Cpu_Static_End,

        Key_Cpu_Dynamic_Start,
        Key_Cpu_Dynamic_TotalUsage,
        Key_Cpu_Dynamic_CoreUsages,
        Key_Cpu_Dynamic_CoreFrequencies,
        Key_Cpu_Dynamic_CurrentMaxFrequency,
        Key_Cpu_Dynamic_ThreadFrequencies,
        Key_Cpu_Dynamic_ThreadUsages,
        Key_Cpu_Dynamic_CoreVoltage,
        Key_Cpu_Dynamic_Power,
        Key_Cpu_Dynamic_SocPower,
        Key_Cpu_Dynamic_Temperature,
        Key_Cpu_Dynamic_Fanspeed,
        Key_Cpu_Dynamic_End,

        Key_Gpu_Static_Start,
        Key_Gpu_Static_ChipDesigner,
        Key_Gpu_Static_CardManufacturer,
        Key_Gpu_Static_Model,
        Key_Gpu_Static_BaseClock,
        Key_Gpu_Static_BoostClock,
        Key_Gpu_Static_MemoryVendor,
        Key_Gpu_Static_MemorySize,
        Key_Gpu_Static_MemoryType,
        Key_Gpu_Static_MemoryBandwidth,
        Key_Gpu_Static_DriverInfo,
        Key_Gpu_Static_DriverVersion,
        Key_Gpu_Static_PnpString,
        Key_Gpu_Static_End,

        Key_Gpu_Dynamic_Start,
        Key_Gpu_Dynamic_Usage,
        Key_Gpu_Dynamic_ClockSpeed,
        Key_Gpu_Dynamic_VRamUsage,
        Key_Gpu_Dynamic_VRamClockSpeed,
        Key_Gpu_Dynamic_VRamUsed,
        Key_Gpu_Dynamic_Power,
        Key_Gpu_Dynamic_TotalBoardPower,
        Key_Gpu_Dynamic_Voltage,
        Key_Gpu_Dynamic_Temperature,
        Key_Gpu_Dynamic_HotSpotTemperature,
        Key_Gpu_Dynamic_FanSpeed,
        Key_Gpu_Dynamic_FanSpeedUsage,
        Key_Gpu_Dynamic_End,

        Key_Memory_Static_Start,
        Key_Memory_Static_TotalVirtualMemory,
        Key_Memory_Static_TotalPhysicalMemory,
        Key_Memory_Static_End,

        Key_Memory_Dynamic_Start,
        Key_Memory_Dynamic_UsedVirtualMemory,
        Key_Memory_Dynamic_UsedPhysicalMemory,
        Key_Memory_Dynamic_End,

        Key_Network_Dynamic_Start,
        Key_Network_Dynamic_Info,
        Key_Network_Dynamic_End,

        Key_Process_Dynamic_Start,
        Key_Process_Dynamic_Info,
        Key_Process_Dynamic_End,

        Key_SysInfo_Start,
        Key_SysInfo_OS_Name,
        Key_SysInfo_OS_CodeName,
        Key_SysInfo_OS_Version,
        Key_SysInfo_End,

        Key_Api_Functions_StatusSupport_Start,
        Key_Api_Functions_StatusSupport_Adlx,
        Key_Api_Functions_StatusSupport_Nvapi,
        Key_Api_Functions_StatusSupport_Nvml,
        Key_Api_Functions_StatusSupport_RyzenMaster,
        Key_Api_Functions_StatusSupport_Wmi,
        Key_Api_Functions_StatusSupport_End,

        Key_Api_Functions_ErrorMessage_Start,
        Key_Api_Functions_ErrorMessage_Adlx,
        Key_Api_Functions_ErrorMessage_Nvapi,
        Key_Api_Functions_ErrorMessage_Nvml,
        Key_Api_Functions_ErrorMessage_RyzenMaster,
        Key_Api_Functions_ErrorMessage_Wmi,
        Key_Api_Functions_ErrorMessage_End,

        Key_Process_GpuUsages,

        Key_End,
    };
};
