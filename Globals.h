#pragma once

#include <string>
#include <vector>

class Globals
{
public:
    struct CpuStaticInfo
    {
        std::string cpuBrand{ "" };
        uint8_t processorCount{ 0 };
        uint8_t threadCount{ 0 };
        uint32_t baseFrequency{ 0 };
        uint32_t maxFrequency{ 0 };
        uint32_t l1CacheSize{ 0 }; //size in KB
        uint32_t l2CacheSize{ 0 }; //size in KB
        uint32_t l3CacheSize{ 0 }; //size in KB
    };

    struct CpuDynamicInfo
    {
        double cpuTotalUsage{ 0 };

        std::vector<double> cpuCoreUsages;
        std::vector<double> cpuCoreFrequencies;

        uint16_t cpuMaxFrequency{ 0 };
        std::vector<uint16_t> cpuThreadFrequencies;
        std::vector<double> cpuThreadUsages;
        
        uint16_t cpuVoltage{ 0 }; // Current voltage in mV
        double cpuPower{ 0 }; //in Watt
        double cpuSocPower{ 0 }; //in Watt

        double cpuTemperature{ 0 };

        uint16_t cpuFanSpeed{ 0 }; // Current fan RPM value
    };

    struct GpuStaticInfo
    {
        std::string chipDesigner{ "" };
        std::string cardManufacturer{ "" };
        std::string gpuModel{ "" };

        std::string memoryVendor{ "" };
        uint16_t memorySize{ 0 };
        std::string memoryType{ "" };

        uint32_t memoryBandwidth{ 0 };

        std::string driverInfo{ "" };
        std::string driverVersion{ "" };
    };

    struct GpuDynamicInfo
    {
        uint16_t gpuGraphicsClock{ 0 }; // Current graphic clock value in MHz
        uint16_t gpuMemoryClock{ 0 }; // Current memory clock value in MHz

        uint8_t gpuGraphicsUsage{ 0 }; // Current graphic activity level in percentage
        uint8_t gpuMemoryUsage{ 0 }; // Current memory activity level in percentage

        uint16_t gpuGraphicsVoltage{ 0 }; // Current graphic voltage in mV
        uint16_t gpuMemoryVoltage{ 0 }; // Current memory voltage in mV
        uint16_t gpuGraphicsPower{ 0 }; //in Watt
        uint16_t gpuAsicPower{ 0 }; //in Watt

        uint8_t gpuTemperature{ 0 };
        uint8_t gpuTemperatureHotspot{ 0 }; // Current center of the die temperature value in C

        uint16_t gpuFanSpeed{ 0 }; // Current fan RPM value
        uint8_t gpuFanSpeedPercent{ 0 }; // Current ratio of fan RPM and max RPM
    };

    struct MemoryStaticInfo
    {
        uint32_t totalVirtualMemory{0};
        uint32_t totalPhysicalMemory{0};
    };

    struct MemoryDynamicInfo
    {
        uint32_t usedVirtualMemory{0};
        uint32_t usedPhysicalMemory{0};
    };
};
