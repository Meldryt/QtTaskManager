#pragma once

#include <string>

#include <QMap>
#include <QString>

#include "ze_api.h"
#include "zes_api.h"

class ZeroLevelHandler
{
public:
    ZeroLevelHandler();
    ~ZeroLevelHandler();

    bool init();

    void readStaticInfo();
    void readDynamicInfo();

    const std::string& gpuCardManufacturer() const { return m_gpuCardManufacturer; };
    const std::string& gpuModel() const { return m_gpuModel; };
    const uint16_t& gpuMemorySize() const { return m_gpuMemorySize; };
    const std::string& gpuMemoryType() const { return m_gpuMemoryType; };
    const std::string& gpuMemoryVendor() const { return m_gpuMemoryVendor; };
    const std::string& gpuDriverInfo() const { return m_gpuDriverInfo; };
    const std::string& gpuDriverVersion() const { return m_gpuDriverVersion; };

    const double& gpuUsage() const { return m_gpuUsage; };
    const double& gpuVramUsage() const { return m_gpuVramUsage; };
    const uint16_t& gpuClockSpeed() const { return m_gpuClockSpeed; };
    const uint16_t& gpuVramClockSpeed() const { return m_gpuVramClockSpeed; };
    const uint16_t& gpuVramUsed() const { return m_gpuVramUsed; };
    const double& gpuTemperature() const { return m_gpuTemperature; };
    const double& gpuHotspotTemperature() const { return m_gpuHotspotTemperature; };
    const uint16_t& gpuFanSpeed() const { return m_gpuFanSpeed; };
    const uint8_t& gpuFanSpeedUsage() const { return m_gpuFanSpeedUsage; };

    const QMap<QString, bool>& functionsSupportStatus() const {
        return m_functionsSupportStatus;
    };

    const QMap<QString, QString>& functionsStatusMessage() const {
        return m_functionsStatusMessage;
    };

private:
    void checkSupportedDynamicFunctions();

    void readLoaderVersions();
    void readDeviceProperties();
    void readDevicePciProperties();
    void readDriverVersion();
    void readDriverInfo();
    void readDeviceFirmware();

    void readProcesses();
    void readGpuEngine();
    void readGpuFrequencies();
    void readGpuMemory();
    void readGpuTemperature();
    void readGpuFanSpeed();
    void readGpuPower();

    bool m_initialized{ false };

    std::string m_gpuCardManufacturer;
    std::string m_gpuModel;
    uint16_t m_gpuMemorySize;
    std::string m_gpuMemoryType;
    std::string m_gpuMemoryVendor;
    std::string m_gpuDriverInfo;
    std::string m_gpuDriverVersion;

    double m_gpuUsage{ 0 }; // Current graphic activity level in percentage
    double m_gpuVramUsage{ 0 }; // Current memory activity level in percentage
    uint16_t m_gpuClockSpeed{ 0 }; // Current graphic clock value in MHz
    uint16_t m_gpuVramClockSpeed{ 0 }; // Current memory clock value in MHz
    uint16_t m_gpuVramUsed{ 0 };
    double m_gpuTemperature{ 0 };
    double m_gpuHotspotTemperature{ 0 }; // Current center of the die temperature value in C
    uint16_t m_gpuFanSpeed{ 0 }; // Current fan RPM value
    uint8_t m_gpuFanSpeedUsage{ 0 }; // Current ratio of fan RPM and max RPM

    QMap<QString, bool> m_functionsSupportStatus;
    QMap<QString, QString> m_functionsStatusMessage;

    ze_device_handle_t m_zeDevice{nullptr};

    uint32_t m_deviceID{0};
    uint32_t m_subDeviceID{UINT32_MAX};

    uint32_t m_zesEngineGroupsCount{0};
    uint32_t m_zesFrequencyDomainCount{0};
    uint32_t m_zesMemoryModulesCount{0};
    uint32_t m_zesTemperatureSensorCount{0};
    uint32_t m_zesFanCount{0};
    uint32_t m_zesPowerDomainCount{0};

    std::vector<_zes_engine_stats_t> m_engineGroupStats;
};
