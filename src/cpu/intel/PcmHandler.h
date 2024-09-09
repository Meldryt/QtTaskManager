#pragma once

#include <vector>
#include <string>

class PcmHandler
{
public:

    PcmHandler();
    ~PcmHandler();

    void init();
    void update();

    const std::string& cpuBrand() const { return m_cpuBrand; };

    const uint16_t& cpuCoreCount() const { return m_cpuCoreCount; };
    const uint16_t& cpuThreadCount() const { return m_cpuThreadCount; };
    const uint16_t& cpuBaseFrequency() const { return m_cpuBaseFrequency; };
    const uint32_t& cpuL1CacheSize() const { return m_cpuL1CacheSize; };
    const uint32_t& cpuL2CacheSize() const { return m_cpuL2CacheSize; };
    const uint32_t& cpuL3CacheSize() const { return m_cpuL3CacheSize; };
    const uint32_t& cpuThermalDesignPower() const { return m_cpuThermalDesignPower; };
    const bool& cpuHyperThreadingEnabled() const { return m_cpuHyperThreadingEnabled; };

    const double& cpuTotalUsage() const { return m_cpuTotalUsage; };
    const uint16_t& cpuCurrentMaxFrequency() const { return m_cpuCurrentMaxFrequency; };
    const std::vector<double>& cpuCoreUsages() const { return m_cpuCoreUsages; };
    const std::vector<double>& cpuCoreFrequencies() const { return m_cpuCoreFrequencies; };
    //const std::vector<double>& cpuThreadUsages() const { return m_cpuThreadUsages; };
    const double& cpuPackagePower() const { return m_packagePower; };
    const double& cpuTemperature() const { return m_cpuTemperature; };

private:
    void readStaticInfo();
    void readDynamicInfo();

    void updateKPIs();

    std::vector<double> m_cpuCoreIpc;
    std::vector<double> m_cpuSocketPackagePower;
    std::vector<double> m_cpuSocketDrawPower;

    std::string m_cpuBrand{ "" };
    uint16_t m_cpuCoreCount{ 0 };
    uint16_t m_cpuThreadCount{ 0 };
    uint32_t m_cpuBaseFrequency{ 0 };
    uint32_t m_cpuL1CacheSize{ 0 }; //size in KB
    uint32_t m_cpuL2CacheSize{ 0 }; //size in KB
    uint32_t m_cpuL3CacheSize{ 0 }; //size in KB
    uint32_t m_cpuThermalDesignPower{ 0 };
    bool m_cpuHyperThreadingEnabled{ false };

    double m_cpuTotalUsage{ 0 };
    std::vector<double> m_cpuCoreUsages;
    std::vector<double> m_cpuCoreFrequencies;

    uint16_t m_cpuCurrentMaxFrequency{ 0 };
    //std::vector<double> m_cpuThreadFrequencies;
    //std::vector<double> m_cpuThreadUsages;

    std::string m_cpuFamilyModel{ "" };
    uint16_t m_cpuSocketCount{ 0 };
    double m_packagePower{ 0.0 };
    double m_cpuTemperature{ 0 };
};