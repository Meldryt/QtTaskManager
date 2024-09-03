#pragma once

#include <vector>
#include <string>

#include <QMap>
#include <QString>

#ifdef HAS_RYZEN_MASTER_SDK
class ICPUEx;
class IBIOSEx;
#endif

class RyzenMasterSdkHandler
{
public:
    RyzenMasterSdkHandler();
    ~RyzenMasterSdkHandler();

    bool init();
    void readStaticInfo();
    void readDynamicInfo();

    const std::string& cpuBrand() const { return m_cpuBrand; };
    const std::string& cpuVendor() const { return m_cpuVendor; };
    const std::string& cpuDescription() const { return m_cpuDescription; };
    const std::string& cpuSocket() const { return m_cpuSocket; };
    const std::string& cpuBiosVersion() const { return m_cpuBiosVersion; };
    const std::string& cpuBiosDate() const { return m_cpuBiosDate; };

    const uint8_t& cpuProcessorCount() const { return m_cpuProcessorCount; };
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

    const QMap<QString, bool>& functionsSupportStatus() const {
        return m_functionsSupportStatus;
    };

    const QMap<QString, QString>& functionsStatusMessage() const {
        return m_functionsStatusMessage;
    };

private:
    void readCpuInfo();
    void readBiosInfo();

    std::string m_cpuBrand{ "" };
    std::string m_cpuVendor{ "" };
    std::string m_cpuDescription{""};
    std::string m_cpuSocket{""};
    std::string m_cpuBiosVersion{""};
    std::string m_cpuBiosDate{""};

    uint8_t m_cpuProcessorCount{ 0 };
    uint16_t m_cpuBaseFrequency{ 0 }; //in MHz
    uint16_t m_cpuMaxTurboFrequency{ 0 }; //in MHz
    uint32_t m_cpuL1CacheSize{ 0 }; //in KB
    uint32_t m_cpuL2CacheSize{ 0 }; //in KB
    uint32_t m_cpuL3CacheSize{ 0 }; //in KB

    std::vector<double> m_cpuCoreFrequencies; 
    uint16_t m_cpuCurrentMaxFrequency{ 0 };
    double m_cpuCoreVoltage{ 0 }; // Current voltage in V
    double m_cpuPower{ 0 }; //in Watt
    double m_cpuSocPower{ 0 }; //in Watt
    double m_cpuTemperature{ 0 };

    bool m_initialized{false};

#ifdef HAS_RYZEN_MASTER_SDK
    //amd ryzen master sdk
    ICPUEx* m_cpuDevice{ nullptr };
    IBIOSEx* m_cpuBiosDevice{ nullptr };
#endif

    QMap<QString, bool> m_functionsSupportStatus;
    QMap<QString, QString> m_functionsStatusMessage;
};

