#pragma once

#include <QObject>
#include <QMap>
#include <QVariant>
#include <string>

#include "Globals.h"

class GpuInfoNVidia
{
public:
    GpuInfoNVidia();

    const QMap<uint8_t,QVariant>& staticInfo() const
    {
        return m_staticInfo;
    }

    const QMap<uint8_t,QVariant>& dynamicInfo() const
    {
        return m_dynamicInfo;
    }

    bool detectGpu();
    void fetchStaticInfo();
    void fetchDynamicInfo();

private:

    QMap<uint8_t,QVariant> m_staticInfo;
    QMap<uint8_t,QVariant> m_dynamicInfo;

    std::string m_gpuChipDesigner;
    std::string m_gpuCardManufacturer;
    std::string m_gpuModel;
    uint16_t m_gpuMemorySize;
    std::string m_gpuMemoryType;

    double m_gpuUsage{ 0 }; // Current graphic activity level in percentage
    double m_gpuVramUsage{ 0 }; // Current memory activity level in percentage
    uint16_t m_gpuClockSpeed{ 0 }; // Current graphic clock value in MHz
    uint16_t m_gpuVramClockSpeed{ 0 }; // Current memory clock value in MHz
    uint16_t m_gpuVramUsed{ 0 };
    double m_gpuPower{ 0 }; //in Watt
    double m_gpuTotalBoardPower{ 0 }; //in Watt
    uint16_t m_gpuVoltage{ 0 }; // Current graphic voltage in mV
    double m_gpuTemperature{ 0 };
    double m_gpuHotspotTemperature{ 0 }; // Current center of the die temperature value in C
    uint16_t m_gpuFanSpeed{ 0 }; // Current fan RPM value
    uint8_t m_gpuFanSpeedUsage{ 0 }; // Current ratio of fan RPM and max RPM
};
