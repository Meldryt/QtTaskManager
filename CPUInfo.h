#pragma once

#include <QObject>
#include <string>
#include <vector>
//#include <windows.h>
#include <pdh.h>

class CPUInfo
{
public:

    struct CPU_StaticInfo
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

    struct CPU_DynamicInfo
    {
        double cpuTotalLoad{0};
        std::vector<double> singleCoreLoads;
        uint8_t cpuTemperature{0};
        uint32_t currentFrequency{0};
    };

    CPUInfo();

    void init();
    void update();

    const CPU_StaticInfo &getStaticInfo() const;
    const CPU_DynamicInfo &getDynamicInfo() const;

private:
    void fetchStaticInfo();
    void fetchDynamicInfo();

    CPU_StaticInfo staticInfo;
    CPU_DynamicInfo dynamicInfo;

    PDH_HQUERY totalCPUQuery;
    PDH_HCOUNTER totalCPUCounter;
    std::vector<PDH_HQUERY> singleCPUQueries;
    std::vector<PDH_HCOUNTER> singleCPUCounters;
};

