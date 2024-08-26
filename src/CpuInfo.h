#pragma once

#include <QObject>
#include <QMap>
#include <QVariant>

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

#include "Globals.h"


class ICPUEx;
class IBIOSEx;

class CpuInfo
{
public:
    CpuInfo();
    ~CpuInfo();

    void init();
    void update();

    const QMap<uint8_t,QVariant> &staticInfo() const;
    const QMap<uint8_t,QVariant> &dynamicInfo() const;

private:
    void fetchStaticInfo();

    void readSystemInfo();

    void initPdh();

    void initRyzenMaster();
    void readStaticInfoRyzenMaster();
    void readDynamicInfoRyzenMaster();
    void readRyzenDynamicCpuInfo();
    void readRyzenDynamicBiosInfo();

    //bool executeQuery(const std::wstring& query);
    //bool executeQueryAsync(const std::wstring& query);



    void fetchStaticInfoLinux();
    void fetchDynamicInfo();
#ifdef _WIN32
    void fetchDynamicInfoWindows();
#else
    void fetchDynamicInfoLinux();
#endif
    void readPdhBaseFrequency();
    void readPdhFrequency();

    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;

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

    //amd ryzen master sdk
    ICPUEx* m_amdCpuDevice{ nullptr };
    IBIOSEx* m_amdCpuBiosDevice{ nullptr };

    bool m_useRyzenCpuParameters{ false };
    bool m_useIntelCpuParameters{ false };

#else

#endif

    std::string m_cpuBrand{ "" };
    uint8_t m_cpuProcessorCount{ 0 };
    uint8_t m_cpuThreadCount{ 0 };
    uint32_t m_cpuBaseFrequency{ 0 };
    uint32_t m_cpuMaxFrequency{ 0 };
    uint32_t m_cpuL1CacheSize{ 0 }; //size in KB
    uint32_t m_cpuL2CacheSize{ 0 }; //size in KB
    uint32_t m_cpuL3CacheSize{ 0 }; //size in KB

    double m_cpuTotalUsage{ 0 };

    std::vector<double> m_cpuCoreUsages;
    std::vector<double> m_cpuCoreFrequencies;

    uint16_t m_cpuCurrentMaxFrequency{ 0 };
    std::vector<double> m_cpuThreadFrequencies;
    std::vector<double> m_cpuThreadUsages;
    
    uint16_t m_cpuVoltage{ 0 }; // Current voltage in mV
    double m_cpuPower{ 0 }; //in Watt
    double m_cpuSocPower{ 0 }; //in Watt

    double m_cpuTemperature{ 0 };

    uint16_t m_cpuFanSpeed{ 0 }; // Current fan RPM value
};

