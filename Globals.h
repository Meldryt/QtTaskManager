#pragma once

#include <string>

class Globals
{
public:
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
};
