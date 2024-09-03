#pragma once

#include <string>
#include <QMap>
#include <QString>

class NvmlHandler
{
public:
    NvmlHandler();
    ~NvmlHandler();

    bool init();
    void readStaticInfo();
    void readDynamicInfo();

    const double& gpuPower() const { return m_gpuPower; };
    const uint8_t& gpuPowerState() const { return m_gpuPowerState; };
    const double& gpuTemperature() const { return m_gpuTemperature; };
    const uint16_t& gpuFanSpeed() const { return m_gpuFanSpeed; };

    const QMap<QString, bool>& functionsSupportStatus() const {
        return m_functionsSupportStatus;
    };

    const QMap<QString, QString>& functionsStatusMessage() const {
        return m_functionsStatusMessage;
    };

private:
    void checkSupportedDynamicFunctions();
    
    void readGpuMemory();
    void readGpuTemperature();
    void readGpuFanSpeed();
    void readGpuPowerUsage();

    bool m_initialized{ false };

    double m_gpuPower{ 0 }; //in Watt
    uint8_t m_gpuPowerState{ 0 };
    double m_gpuTemperature{ 0 };
    uint16_t m_gpuFanSpeed{ 0 }; // Current fan RPM value

    QMap<QString, bool> m_functionsSupportStatus;
    QMap<QString, QString> m_functionsStatusMessage;
};
