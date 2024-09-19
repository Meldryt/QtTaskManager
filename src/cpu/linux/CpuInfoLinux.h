#pragma once

#include <string>
#include <vector>

#ifdef __linux__
#include "sys/types.h"
#include "sys/sysinfo.h"
#endif

#include <QMap>
#include <QVariant>

class CpuInfoLinux
{
public:
    CpuInfoLinux();
    ~CpuInfoLinux();

    void init();
    void readStaticInfo();
    void readDynamicInfo();

    const std::string& cpuBrand() const { return m_cpuBrand; };
    // const std::string& cpuVendor() const { return m_cpuVendor; };
    // const std::string& cpuDescription() const { return m_cpuDescription; };
    // const std::string& cpuSocket() const { return m_cpuSocket; };
    // const std::string& cpuBiosVersion() const { return m_cpuBiosVersion; };
    // const std::string& cpuBiosDate() const { return m_cpuBiosDate; };
    
    const uint16_t& cpuCoreCount() const { return m_cpuCoreCount; };
    const uint16_t& cpuBaseFrequency() const { return m_cpuBaseFrequency; };
    const uint16_t& cpuMaxTurboFrequency() const { return m_cpuMaxTurboFrequency; };
    const uint32_t& cpuL1CacheSize() const { return m_cpuL1CacheSize; };
    const uint32_t& cpuL2CacheSize() const { return m_cpuL2CacheSize; };
    const uint32_t& cpuL3CacheSize() const { return m_cpuL3CacheSize; };
    const std::vector<double>& cpuCoreFrequencies() const { return m_cpuCoreFrequencies; };
    const uint16_t& cpuCurrentMaxFrequency() const { return m_cpuCurrentMaxFrequency; };
    const double& cpuCoreVoltage() const { return m_cpuCoreVoltage; };
    const double& cpuPower() const { return m_cpuPower; };
    const double& cpuSocPower() const { return m_cpuSocPower; };
    const double& cpuTemperature() const { return m_cpuTemperature; };
    const uint16_t& cpuFanSpeed() const { return m_cpuFanSpeed; };

    const QMap<uint8_t, QVariant>& staticInfo() const;
    const QMap<uint8_t, QVariant>& dynamicInfo() const;

private:
    void readCpuVendor();
    void readCpuBrand();
    void readCpuBaseFrequency();
    void readCpuMaxFrequency();
    void readCpuCoreCount();

    void readCpuCoreFrequencies();
    void readCpuTemperature();

    std::string m_cpuVendor{ "" };
    std::string m_cpuBrand{ "" };
    std::string m_cpuSocket{""};
    uint16_t m_cpuCoreCount{ 0 };
    uint16_t m_cpuThreadCount{ 0 };
    uint16_t m_cpuBaseFrequency{ 0 };
    uint16_t m_cpuMaxTurboFrequency{ 0 };
    uint32_t m_cpuL1CacheSize{ 0 }; //size in KB
    uint32_t m_cpuL2CacheSize{ 0 }; //size in KB
    uint32_t m_cpuL3CacheSize{ 0 }; //size in KB

    double m_cpuTotalUsage{ 0 };

    std::vector<double> m_cpuCoreUsages;
    std::vector<double> m_cpuCoreFrequencies;

    uint16_t m_cpuCurrentMaxFrequency{ 0 };
    std::vector<double> m_cpuThreadFrequencies;
    std::vector<double> m_cpuThreadUsages;
    
    double m_cpuCoreVoltage{ 0 }; // Current voltage in V
    double m_cpuPower{ 0 }; //in Watt
    double m_cpuSocPower{ 0 }; //in Watt

    double m_cpuTemperature{ 0 };

    uint16_t m_cpuFanSpeed{ 0 }; // Current fan RPM value

    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;
};

