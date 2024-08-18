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

#include "Globals.h"

class ICPUEx;
class IBIOSEx;

class WmiManager;

class CpuInfo
{
public:
    CpuInfo();

    void init();
    void update();

    const Globals::CpuStaticInfo &staticInfo() const;
    const Globals::CpuDynamicInfo &dynamicInfo() const;

private:
    void fetchStaticInfo();

    void readSystemInfo();

    //bool initWmi();
    //void readStaticInfoWmi();
    void readDynamicInfoWmi();
    //void readWmiFrequency();
    //void readWmiFanSpeed();
    void readThermalZoneTemperature();

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

    Globals::CpuStaticInfo m_staticInfo;
    Globals::CpuDynamicInfo m_dynamicInfo;

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

    WmiManager* m_wmiManager{ nullptr };

#else

#endif
};

