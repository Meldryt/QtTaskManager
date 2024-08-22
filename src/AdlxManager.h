#pragma once

#include "external/ADLX/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h"
#include "external/ADLX/SDK/Include/IPerformanceMonitoring.h"
#include "external/ADLX/SDK/Include/ISystem.h"
#include <map>
#include <vector>
#include <string>

#include <QMap>
#include <QVariant>
#include "Globals.h"

// Use ADLX namespace
using namespace adlx;

class AdlxManager
{
public:
    AdlxManager();
    ~AdlxManager();

    bool init();

    const QMap<uint8_t,QVariant>& staticInfo() const
    {
        return m_staticInfo;
    }

    const QMap<uint8_t,QVariant>& dynamicInfo() const
    {
        return m_dynamicInfo;
    }

    bool fetchStaticInfo();
    bool fetchDynamicInfo();

private:

    void ShowGPUInfo();
    // Show current all metrics
    void ShowCurrentAllMetrics(IADLXPerformanceMonitoringServicesPtr perfMonitoringServices, IADLXGPUPtr oneGPU);

    void ShowCPUUsage(IADLXSystemMetricsSupportPtr systemMetricsSupport, IADLXSystemMetricsPtr systemMetrics);
    void ShowSystemRAM(IADLXSystemMetricsSupportPtr systemMetricsSupport, IADLXSystemMetricsPtr systemMetrics);
    void ShowSmartShift(IADLXSystemMetricsSupportPtr systemMetricsSupport, IADLXSystemMetricsPtr systemMetrics);
    void GetTimeStamp(IADLXSystemMetricsPtr systemMetrics);
    void GetTimeStamp(IADLXGPUMetricsPtr gpuMetrics);
    void ShowGPUUsage(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics);
    void ShowGPUClockSpeed(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics);
    void ShowGPUVRAMClockSpeed(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics);
    void ShowGPUTemperature(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics);
    void ShowGPUHotspotTemperature(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics);
    void ShowGPUPower(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics);
    void ShowGPUFanSpeed(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics);
    void ShowGPUVRAM(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics);
    void ShowGPUVoltage(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics);
    void ShowGPUTotalBoardPower(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics);
    void ShowGPUIntakeTemperature(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics);
    
    const std::map<uint32_t, std::string> MapVendorIdName = {
        {0x1002, "AMD"},
        {0x10DE, "NVIDIA"},
        {0x1043, "ASUS"},
        {0x196D, "Club 3D"},
        {0x1092, "Diamond Multimedia"},
        {0x18BC, "GeCube"},
        {0x1458, "Gigabyte"},
        {0x17AF, "HIS"},
        {0x16F3, "Jetway"},
        {0x1462, "MSI"},
        {0x1DA2, "Sapphire"},
        {0x148C, "PowerColor"},
        {0x1545, "VisionTek"},
        {0x1682, "XFX"},
        {0x1025, "Acer"},
        {0x106B, "Apple"},
        {0x1028, "Dell"},
        {0x107B, "Gateway"},
        {0x103C, "HP"},
        {0x17AA, "Lenovo"},
        {0x104D, "Sony"},
        {0x1179, "Toshiba"}
    };

    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;

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

    // ASCII �
    static const signed char g_degree = 248;

    IADLXGPUPtr m_oneGPU{ nullptr };
    IADLXPerformanceMonitoringServicesPtr m_perfMonitoringService{ nullptr };

    bool m_initialized{ false };
};