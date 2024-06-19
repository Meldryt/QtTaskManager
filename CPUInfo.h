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

#include <Globals.h>

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
#else
    void fetchStaticInfoLinux();
#endif
    void fetchDynamicInfo();
#ifdef _WIN32
    void fetchDynamicInfoWindows();
#else
    void fetchDynamicInfoLinux();
#endif

    void readFrequency();

    Globals::CpuStaticInfo staticInfo;
    Globals::CpuDynamicInfo dynamicInfo;

#ifdef _WIN32
    PDH_HQUERY totalCPUQuery;
    PDH_HCOUNTER totalCPUCounter;
    std::vector<PDH_HQUERY> singleCPUQueries;
    std::vector<PDH_HCOUNTER> singleCPUCounters;
#else

#endif
};

