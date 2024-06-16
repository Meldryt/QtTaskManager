#pragma once

#include <QObject>
#include <string>
#include <vector>
//#include <windows.h>
#ifdef _WIN32
#include <pdh.h>
#else
#include "sys/types.h"
#include "sys/sysinfo.h"
#endif

class CpuInfo
{
public:

    struct CpuStaticInfo
    {
        std::string cpuBrand{""};
        uint8_t processorCount{0};
        uint8_t threadCount{0};
        uint32_t baseFrequency{0};
        uint32_t maxFrequency{0};
        uint32_t l1CacheSize{0}; //size in KB
        uint32_t l2CacheSize{0}; //size in KB
        uint32_t l3CacheSize{0}; //size in KB
    };

    struct CpuDynamicInfo
    {
        double cpuTotalLoad{0};
        std::vector<double> singleCoreLoads;
        uint8_t cpuTemperature{0};
        uint32_t currentFrequency{0};
    };

    CpuInfo();

    void init();
    void update();

    const CpuStaticInfo &getStaticInfo() const;
    const CpuDynamicInfo &getDynamicInfo() const;

private:
    void fetchStaticInfo();
#ifdef _WIN32
    void fetchStaticInfoWindows();
#else
    void fetchStaticInfoLinux();
#endif
    void fetchDynamicInfo();
#ifdef _WIN32
    void fetchDynamicInfoWindows();
#else
    void fetchDynamicInfoLinux();
#endif

    CpuStaticInfo staticInfo;
    CpuDynamicInfo dynamicInfo;

#ifdef _WIN32
    PDH_HQUERY totalCPUQuery;
    PDH_HCOUNTER totalCPUCounter;
    std::vector<PDH_HQUERY> singleCPUQueries;
    std::vector<PDH_HCOUNTER> singleCPUCounters;
#else

#endif
};

