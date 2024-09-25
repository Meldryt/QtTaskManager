#pragma once

#include "../main/BaseInfo.h"

#ifdef _WIN32
class CpuInfoWindows;
#elif __linux__
class CpuInfoLinux;
#endif

class CpuInfoAmd;
class CpuInfoIntel;

class CpuInfo : public BaseInfo
{
public:

    enum CpuManufacturer
    {
        AMD = 0,
        INTEL,
        UNKNOWN
    };

    CpuInfo();
    ~CpuInfo();

    virtual void init() override;
    virtual void update() override;

private:
    void readStaticInfo();
    void readDynamicInfo();

    void detectCpu();

    bool m_cpuDetected{false};
    CpuManufacturer m_cpuManufacturer{UNKNOWN};

#ifdef _WIN32
    CpuInfoWindows* m_cpuInfoWindows{ nullptr };
#elif __linux__
    CpuInfoLinux* m_cpuInfoLinux{ nullptr };
#endif

    CpuInfoAmd* m_cpuInfoAmd{ nullptr };
    CpuInfoIntel* m_cpuInfoIntel{ nullptr };
};

