#pragma once

#include <stdint.h>

class Globals
{
public:

    //enum used for static and dynamic system info map QMap<uint8_t, QVariant>
    static const int8_t SysInfo_Uninitialized{-1};

    enum SysInfoAttr : uint8_t
    {
        Key_Cpu_Static_Start = 0,
        Key_Cpu_Brand,
        Key_Cpu_Socket,
        Key_Cpu_ProcessorCount,
        Key_Cpu_ThreadCount,
        Key_Cpu_BaseFrequency,
        Key_Cpu_MaxTurboFrequency,
        Key_Cpu_L1CacheSize,
        Key_Cpu_L2CacheSize,
        Key_Cpu_L3CacheSize,
        Key_Cpu_Static_End,

        Key_Cpu_Dynamic_Start,
        Key_Cpu_TotalUsage,
        Key_Cpu_CoreUsages,
        Key_Cpu_CoreFrequencies,
        Key_Cpu_CurrentMaxFrequency,
        Key_Cpu_ThreadFrequencies,
        Key_Cpu_ThreadUsages,
        Key_Cpu_CoreVoltage,
        Key_Cpu_Power,
        Key_Cpu_SocPower,
        Key_Cpu_Temperature,
        Key_Cpu_Fanspeed,
        Key_Cpu_Dynamic_End,

        Key_Gpu_Static_Start,
        Key_Gpu_ChipDesigner,
        Key_Gpu_CardManufacturer,
        Key_Gpu_Model,
        Key_Gpu_MemoryVendor,
        Key_Gpu_MemorySize,
        Key_Gpu_MemoryType,
        Key_Gpu_MemoryBandwidth,
        Key_Gpu_DriverInfo,
        Key_Gpu_DriverVersion,
        Key_Gpu_PnpString,
        Key_Gpu_Static_End,

        Key_Gpu_Dynamic_Start,
        Key_Gpu_Usage,
        Key_Gpu_ClockSpeed,
        Key_Gpu_VRamUsage,
        Key_Gpu_VRamClockSpeed,
        Key_Gpu_VRamUsed,
        Key_Gpu_Power,
        Key_Gpu_TotalBoardPower,
        Key_Gpu_Voltage,
        Key_Gpu_Temperature,
        Key_Gpu_HotSpotTemperature,
        Key_Gpu_FanSpeed,
        Key_Gpu_FanSpeedUsage,
        Key_Gpu_Dynamic_End,

        Key_Memory_Start,
        Key_Memory_TotalVirtualMemory,
        Key_Memory_TotalPhysicalMemory,
        Key_Memory_UsedVirtualMemory,
        Key_Memory_UsedPhysicalMemory,
        Key_Memory_End,

        Key_Network_Start,
        Key_Network_Names,
        Key_Network_BytesReceivedPerSec,
        Key_Network_BytesSentPerSec,
        Key_Network_TotalBytesPerSec,
        Key_Network_CurrentBandwidth,
        Key_Network_End,

        Key_SysInfo_Start,
        Key_SysInfo_OpenGL,
        Key_SysInfo_OS,
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

        Key_End,
    };
};
