#pragma once

#include <string>
#include <vector>

class Globals
{
public:
    enum SysInfoAttr : uint8_t
    {
        Key_Cpu_Brand = 0,
        Key_Cpu_ProcessorCount,
        Key_Cpu_ThreadCount,
        Key_Cpu_BaseFrequency,
        Key_Cpu_MaxFrequency,
        Key_Cpu_L1CacheSize,
        Key_Cpu_L2CacheSize,
        Key_Cpu_L3CacheSize,

        Key_Cpu_TotalUsage,
        Key_Cpu_CoreUsages,
        Key_Cpu_CoreFrequencies,
        Key_Cpu_CurrentMaxFrequency,
        Key_Cpu_ThreadFrequencies,
        Key_Cpu_ThreadUsages,
        Key_Cpu_Voltage,
        Key_Cpu_Power,
        Key_Cpu_SocPower,
        Key_Cpu_Temperature,
        Key_Cpu_Fanspeed,

        Key_Gpu_ChipDesigner,
        Key_Gpu_CardManufacturer,
        Key_Gpu_Model,
        Key_Gpu_MemoryVendor,
        Key_Gpu_MemorySize,
        Key_Gpu_MemoryType,
        Key_Gpu_MemoryBandwidth,
        Key_Gpu_DriverInfo,
        Key_Gpu_DriverVersion,

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

        Key_Memory_TotalVirtualMemory,
        Key_Memory_TotalPhysicalMemory,
        Key_Memory_UsedVirtualMemory,
        Key_Memory_UsedPhysicalMemory,

        Key_Network_Names,
        Key_Network_BytesReceivedPerSec,
        Key_Network_BytesSentPerSec,
        Key_Network_TotalBytesPerSec,
        Key_Network_CurrentBandwidth,
    };
};
