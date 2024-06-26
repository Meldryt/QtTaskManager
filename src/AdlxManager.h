#pragma once

#include "external/ADLX/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h"
#include "external/ADLX/SDK/Include/IPerformanceMonitoring.h"
#include "external/ADLX/SDK/Include/ISystem.h"
#include <map>
#include <vector>
#include <string>

#include "Globals.h"

// Use ADLX namespace
using namespace adlx;

class AdlxManager
{
public:
    AdlxManager();
    ~AdlxManager();

    bool init();

    const Globals::GpuStaticInfo& staticInfo() const
    {
        return m_staticInfo;
    }

    const Globals::GpuDynamicInfo& dynamicInfo() const
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

    Globals::GpuStaticInfo m_staticInfo;
    Globals::GpuDynamicInfo m_dynamicInfo;

    // ASCII °
    static const signed char g_degree = 248;

    IADLXGPUPtr m_oneGPU{ nullptr };
    IADLXPerformanceMonitoringServicesPtr m_perfMonitoringService{ nullptr };
};