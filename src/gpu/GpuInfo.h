#pragma once

#include "../main/BaseInfo.h"

#include <string>

class GpuInfoAmd;
class GpuInfoIntel;
class GpuInfoNVidia;
#ifdef _WIN32
#else
    class GpuInfoLinux;
#endif

class GpuInfo : public BaseInfo
{
public:

    enum GpuManufacturer
    {
        AMD = 0,
        INTEL,
        NVIDIA ,
        UNKNOWN
    };

    GpuInfo();
    ~GpuInfo();

    virtual void init();
    virtual void update();

private:
    void detectGpu();
    void readStaticInfo();
    void readDynamicInfo();

    bool m_gpuDetected{false};
    GpuManufacturer m_gpuManufacturer{UNKNOWN};

    GpuInfoAmd* m_gpuInfoAmd{ nullptr };
    GpuInfoIntel* m_gpuInfoIntel{ nullptr };
    GpuInfoNVidia* m_gpuInfoNVidia{ nullptr };

#ifdef _WIN32
#else
    GpuInfoLinux* m_gpuInfoLinux{ nullptr };
#endif
};
