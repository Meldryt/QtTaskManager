#pragma once

#include "external/ADLX/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h"
#include "external/ADLX/SDK/Include/IPerformanceMonitoring.h"
#include "external/ADLX/SDK/Include/ISystem.h"

#include <QMap>
#include <QVariant>
#include <QString>

// Use ADLX namespace
using namespace adlx;

class AdlxHandler
{
public:
    AdlxHandler();
    ~AdlxHandler();

    void init();

    const QMap<uint8_t, QVariant>& staticInfo() const;
    const QMap<uint8_t, QVariant>& dynamicInfo() const;

    bool readStaticInfo();
    bool readDynamicInfo();

private:
    void checkSupportedDynamicFunctions();
    void setFunctionStatus(const char* key, const bool support, const ADLX_RESULT res);

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

    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;

    std::string m_gpuChipDesigner{ "" };
    std::string m_gpuCardManufacturer{ "" };
    std::string m_gpuModel{ "" };
    uint16_t m_gpuMemorySize{ 0 };
    std::string m_gpuMemoryType{ "" };
    std::string m_gpuPnpString{ "" };

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

    IADLXGPUPtr m_oneGPU{ nullptr };
    IADLXPerformanceMonitoringServicesPtr m_perfMonitoringService{ nullptr };

    bool m_initialized{ false };

    QMap<QString, bool> m_functionsSupportStatus;
    QMap<QString, QString> m_functionsStatusMessage;
};