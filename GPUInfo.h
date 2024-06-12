#pragma once

#include <QObject>
#include <string>

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

    struct GpuStaticInfo
    {
        std::string gpuBrand{""};
    };

    struct GpuDynamicInfo
    {
        uint8_t gpuTotalLoad{0};
        uint8_t gpuTemperature{0};
    };

    GpuInfo();

    void init();
    void update();

    const GpuStaticInfo& getStaticInfo() const
    {
        return m_staticInfo;
    }

    const GpuDynamicInfo& getDynamicInfo() const
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

    GpuStaticInfo m_staticInfo;
    GpuDynamicInfo m_dynamicInfo;
};
