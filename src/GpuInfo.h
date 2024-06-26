#pragma once

#include <QObject>
#include <string>

#include "Globals.h"

class GpuInfoNVidia;
class GpuInfoAmd;

class GpuInfo
{
public:

    enum GpuManufacturer
    {
        NVIDIA = 0,
        AMD,
        UNKNOWN
    };

    GpuInfo();

    void init();
    void update();

    const Globals::GpuStaticInfo& getStaticInfo() const
    {
        return m_staticInfo;
    }

    const Globals::GpuDynamicInfo& getDynamicInfo() const
    {
        return m_dynamicInfo;
    }

private:
    void detectGpu();
    void fetchStaticInfo();
    void fetchDynamicInfo();

    bool m_gpuDetected{false};
    GpuManufacturer m_gpuManufacturer{AMD};

    GpuInfoNVidia* m_gpuInfoNVidia{ nullptr };
    GpuInfoAmd* m_gpuInfoAmd{ nullptr };

    Globals::GpuStaticInfo m_staticInfo;
    Globals::GpuDynamicInfo m_dynamicInfo;
};
