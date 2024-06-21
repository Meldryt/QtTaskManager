#pragma once

#include <QObject>
#include <string>
#include <vector>
//#include <windows.h>
#ifdef _WIN32
#include <pdh.h>
#include <Wbemidl.h>

#else
#include "sys/types.h"
#include "sys/sysinfo.h"
#endif

#include <Globals.h>

class ICPUEx;
class IBIOSEx;

class CpuInfo
{
public:
    CpuInfo();

    void init();
    void update();

    const Globals::CpuStaticInfo &getStaticInfo() const;
    const Globals::CpuDynamicInfo &getDynamicInfo() const;

private:
    void fetchStaticInfo();
#ifdef _WIN32
    void fetchStaticInfoWindows();

    bool setupWmi();
    void initAmdRyzenMaster();

    bool executeQuery(const std::wstring& query);
    bool executeQueryAsync(const std::wstring& query);

    //template <typename T>
    //std::vector<T> query(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter = L"");
    std::vector<std::string> query(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter = L"", const ULONG count = 1);
    void queryAsync(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter = L"", const ULONG count = 1);

#else
    void fetchStaticInfoLinux();
#endif
    void fetchDynamicInfo();
#ifdef _WIN32
    void fetchDynamicInfoWindows();
#else
    void fetchDynamicInfoLinux();
#endif
    void readPdhFrequency();
    void readRyzenCpuParameters();
    void readWmiFrequency();

    Globals::CpuStaticInfo staticInfo;
    Globals::CpuDynamicInfo dynamicInfo;

#ifdef _WIN32
    //pdh
    PDH_HQUERY totalCPUQuery;
    PDH_HCOUNTER totalCPUCounter;
    std::vector<PDH_HQUERY> singleCPUQueries;
    std::vector<PDH_HCOUNTER> singleCPUCounters;

    PDH_HQUERY m_cpuQueryFreq;
    PDH_HCOUNTER m_cpuFreqCounter;

    PDH_HQUERY m_cpuQueryPerformance;
    PDH_HCOUNTER m_cpuPerformanceCounter;
    
    //wmi
    IWbemLocator* locator{ nullptr };
    IWbemServices* service{ nullptr };
    IEnumWbemClassObject* enumerator{ nullptr };

    //amd ryzen master sdk
    ICPUEx* m_amdCpuDevice{ nullptr };
    IBIOSEx* m_amdCpuBiosDevice{ nullptr };

    bool m_useRyzenCpuParameters{ false };
#else

#endif
};

