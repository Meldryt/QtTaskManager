#pragma once

#include <QObject>
#include <QMap>
#include <QVariant>

#include <string>

#include "../Globals.h"


class GpuInfoAmd;
class GpuInfoIntel;
class GpuInfoNVidia;
<<<<<<< HEAD
#ifdef _WIN32
#else
    class GpuInfoLinux;
#endif
=======
>>>>>>> master

class GpuInfo
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
    GpuManufacturer m_gpuManufacturer{UNKNOWN};

    GpuInfoAmd* m_gpuInfoAmd{ nullptr };
    GpuInfoIntel* m_gpuInfoIntel{ nullptr };
    GpuInfoNVidia* m_gpuInfoNVidia{ nullptr };
<<<<<<< HEAD

#ifdef _WIN32
#else
    GpuInfoLinux* m_gpuInfoLinux{ nullptr };
#endif
=======
>>>>>>> master

    QMap<uint8_t,QVariant> m_staticInfo;
    QMap<uint8_t,QVariant> m_dynamicInfo;
};
