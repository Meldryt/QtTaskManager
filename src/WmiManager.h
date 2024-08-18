#pragma once

#ifdef _WIN32
#include <Wbemidl.h>
#endif

#include "Globals.h"

#include <vector>
#include <string>
#include <map>

class WmiManager
{
public:
    WmiManager();

    bool init();
    void readStaticInfo();
    void update();

    //@note: disable costly updates
    void disableCpuUpdates();

    const Globals::CpuStaticInfo& cpuStaticInfo() const;
    const Globals::CpuDynamicInfo& cpuDynamicInfo() const;
    const Globals::NetworkDynamicInfo& networkDynamicInfo() const;

private:

    bool executeQuery(const std::wstring& query);
    //bool executeQueryAsync(const std::wstring& query);

    std::vector<std::string> query(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter = L"", const ULONG count = 1);
    std::map<std::string, std::vector<std::string>> queryArray(const std::wstring& wmi_class, const std::vector<std::wstring>& fields, const std::wstring& filter = L"", const ULONG count = 1);
    //void queryAsync(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter = L"", const ULONG count = 1);

    //QuerySink* m_sink;

    void readCpuFrequency();
    void readCpuInfo();
    void readFanSpeed();
    void readNetworkSpeed();

    bool m_readCpuInfos{ true };

    //wmi
    IWbemLocator* m_locator{ nullptr };
    IWbemServices* m_service{ nullptr };
    IEnumWbemClassObject* m_enumerator{ nullptr };

    bool m_isWmiFrequencyInfoAvailable{ false };
    bool m_isWmiFanInfoAvailable{ false };

    Globals::CpuStaticInfo m_cpuStaticInfo;
    Globals::CpuDynamicInfo m_cpuDynamicInfo;
    Globals::NetworkDynamicInfo m_networkDynamicInfo;
};