#pragma once

#include <QObject>
#include <string>

class GpuInfo;

class GpuInfoNVidia
{
public:
    GpuInfoNVidia();

    std::string getBrand() const {
        return m_brand;
    }

    uint8_t getTotalLoad() const {
        return m_totalLoad;
    }

    uint8_t getTemperature() const {
        return m_temperature;
    }

    bool detectGpu();
    void fetchStaticInfo();
    void fetchDynamicInfo();

private:
    std::string m_brand{ "" };
    uint8_t m_totalLoad{ 0 };
    uint8_t m_temperature{ 0 };
};
