#pragma once

#include <QMap>
#include <QVariant>

#ifdef _WIN32
class CpuInfoWindows;
#elif __linux__
class CpuInfoLinux;
#endif

class CpuInfoAmd;
class CpuInfoIntel;

class CpuInfo
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

    void init();
    void update();

    const QMap<uint8_t,QVariant>& staticInfo() const
    {
        return m_staticInfo;
    }

    const QMap<uint8_t,QVariant>& dynamicInfo() const
    {
        return m_dynamicInfo;
    }

private:
    void detectCpu();
    void readStaticInfo();
    void readDynamicInfo();

    bool m_cpuDetected{false};
    CpuManufacturer m_cpuManufacturer{UNKNOWN};

#ifdef _WIN32
    CpuInfoWindows* m_cpuInfoWindows{ nullptr };
#elif __linux__
    CpuInfoLinux* m_cpuInfoLinux{ nullptr };
#endif

    CpuInfoAmd* m_cpuInfoAmd{ nullptr };
    CpuInfoIntel* m_cpuInfoIntel{ nullptr };

    QMap<uint8_t,QVariant> m_staticInfo;
    QMap<uint8_t,QVariant> m_dynamicInfo;
};

