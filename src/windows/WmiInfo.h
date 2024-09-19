#pragma once

#ifdef _WIN32
#include <Wbemidl.h>
#endif

#include "../Globals.h"

#include <QMap>
#include <QVariant>

#include <vector>
#include <string>
#include <map>

class WmiInfo
{
public:
    WmiInfo();
    ~WmiInfo();

    bool init();
    void update();

    void readStaticInfo();

    //@note: disable costly updates
    //void disableCpuUpdates();

    const QMap<uint8_t, QVariant>& staticInfo() const
    {
        return m_staticInfo;
    }

    const QMap<uint8_t, QVariant>& dynamicInfo() const
    {
        return m_dynamicInfo;
    }

private:
    void checkSupportedFunctions();

    bool executeQuery(const std::wstring& query);

    std::vector<std::string> query(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter = L"", const ULONG count = 1);
    std::map<std::string, std::vector<std::string>> queryArray(const std::wstring& wmi_class, const std::vector<std::wstring>& fields, const std::wstring& filter = L"", const ULONG count = 1);

    void readGpuUsage();
    void readCpuFrequency();
    void readCpuInfo();
    void readTemperature();
    void readFanSpeed();
    void readGpuInfo();
    void readNetworkSpeed();
    void readPowerSupply();

    bool m_readCpuParameters{ true };

    //wmi
    IWbemLocator* m_locator{ nullptr };
    IWbemServices* m_service{ nullptr };
    IEnumWbemClassObject* m_enumerator{ nullptr };

    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;

    std::string m_cpuBrand{ "" };
    uint16_t m_cpuCoreCount{ 0 };
    uint16_t m_cpuThreadCount{ 0 };
    uint32_t m_cpuBaseFrequency{ 0 };
    uint32_t m_cpuMaxTurboFrequency{ 0 };

    uint32_t m_cpuCurrentMaxFrequency{ 0 };
    std::vector<double> m_cpuThreadFrequencies;
    std::vector<double> m_cpuThreadUsages;

    std::string m_gpuModel;
    uint16_t m_gpuMemorySize;
    std::string m_gpuDriverDate;
    std::string m_gpuDriverInfo;
    std::string m_gpuDriverVersion;
    std::string m_gpuPnpString;

    double m_gpuUsage{0.0};
    QMap<uint32_t, QPair<uint8_t,uint64_t>> m_processGpuUsage;

    std::vector<std::string> m_networkNames;
    std::vector<uint32_t> m_networkBytesReceivedPerSec;
    std::vector<uint32_t> m_networkBytesSentPerSec;
    std::vector<uint32_t> m_networkBytesTotalPerSec;
    std::vector<uint32_t> m_networkCurrentBandwidth;

    QMap<QString, bool> m_functionsSupportStatus;
    QMap<QString, QString> m_functionsStatusMessage;

};