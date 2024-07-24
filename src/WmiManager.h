#pragma once

#ifdef _WIN32
#include <Wbemidl.h>
#endif

#include "Globals.h"

#include <vector>
#include <string>

class WmiManager
{
public:
    WmiManager();

    bool init();
    void readQuery(const std::string className, const std::string parameter);

private:

    bool executeQuery(const std::wstring& query);
    //bool executeQueryAsync(const std::wstring& query);

    //template <typename T>
    //std::vector<T> query(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter = L"");
    std::vector<std::string> query(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter = L"", const ULONG count = 1);
    //void queryAsync(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter = L"", const ULONG count = 1);

    //QuerySink* m_sink;

    void readCpuFrequency();
    void readCpuInfo();
    void readFanSpeed();

    //wmi
    IWbemLocator* m_locator{ nullptr };
    IWbemServices* m_service{ nullptr };
    IEnumWbemClassObject* m_enumerator{ nullptr };

    bool m_isWmiFrequencyInfoAvailable{ false };
    bool m_isWmiFanInfoAvailable{ false };

    Globals::CpuStaticInfo staticInfo;
    Globals::CpuDynamicInfo dynamicInfo;
};