#pragma once

#include <QObject>
#include <QMap>
#include <QVariant>

#include <string>

#include "../Globals.h"

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
    ~GpuInfo();

    void init();
    void update();

    const QMap<uint8_t,QVariant>& getStaticInfo() const
    {
        return m_staticInfo;
    }

    const QMap<uint8_t,QVariant>& getDynamicInfo() const
    {
        return m_dynamicInfo;
    }

private:
    void detectGpu();
    void readStaticInfo();
    void readDynamicInfo();

    bool m_gpuDetected{false};
    GpuManufacturer m_gpuManufacturer{AMD};

    GpuInfoNVidia* m_gpuInfoNVidia{ nullptr };
    GpuInfoAmd* m_gpuInfoAmd{ nullptr };

    QMap<uint8_t,QVariant> m_staticInfo;
    QMap<uint8_t,QVariant> m_dynamicInfo;
};
