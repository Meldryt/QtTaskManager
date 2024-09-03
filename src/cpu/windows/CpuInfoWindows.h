#pragma once

#include <string>
#include <vector>

#include <system_error>
#ifdef _WIN32
#include <pdh.h>
#include <Wbemidl.h>
#endif

#include <QMap>
#include <QVariant>

class CpuInfoWindows
{
public:
    CpuInfoWindows();
    ~CpuInfoWindows();

    void init();

    void readCpuIdBrand();

    void readStaticInfo();
    void readDynamicInfo();

    const std::string& cpuBrand() const { return m_cpuBrand; };
    
    const uint8_t& cpuProcessorCount() const { return m_cpuProcessorCount; };
    const uint16_t& cpuBaseFrequency() const { return m_cpuBaseFrequency; };
    const uint32_t& cpuL1CacheSize() const { return m_cpuL1CacheSize; };
    const uint32_t& cpuL2CacheSize() const { return m_cpuL2CacheSize; };
    const uint32_t& cpuL3CacheSize() const { return m_cpuL3CacheSize; };
    const uint16_t& cpuCurrentMaxFrequency() const { return m_cpuCurrentMaxFrequency; };

    const QMap<uint8_t, QVariant>& staticInfo() const;
    const QMap<uint8_t, QVariant>& dynamicInfo() const;

private:
    void readLogicalProcessorInfo();

    void initPdh();
    void readPdhBaseFrequency();
    void readPdhCoreUsage();
    void readPdhCurrentMaxFrequency();

#ifdef _WIN32
    //pdh
    PDH_HQUERY m_pdhTotalCpuQuery;
    PDH_HCOUNTER m_pdhTotalCpuCounter;
    std::vector<PDH_HQUERY> m_pdhSingleCpuQueries;
    std::vector<PDH_HCOUNTER> m_pdhSingleCpuCounters;

    PDH_HQUERY m_pdhCpuQueryFreq;
    PDH_HCOUNTER m_pdhCpuFreqCounter;

    PDH_HQUERY m_pdhCpuQueryPerformance;
    PDH_HCOUNTER m_pdhCpuPerformanceCounter;
#endif

    std::string m_cpuBrand{ "" };
    uint8_t m_cpuProcessorCount{ 0 };
    uint8_t m_cpuThreadCount{ 0 };
    uint32_t m_cpuBaseFrequency{ 0 };
    uint32_t m_cpuL1CacheSize{ 0 }; //size in KB
    uint32_t m_cpuL2CacheSize{ 0 }; //size in KB
    uint32_t m_cpuL3CacheSize{ 0 }; //size in KB

    double m_cpuTotalUsage{ 0 };
    std::vector<double> m_cpuCoreUsages;
    uint16_t m_cpuCurrentMaxFrequency{ 0 };

    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;
};

