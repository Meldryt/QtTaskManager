#pragma once

#include <QObject>
#include <string>
#include "AdlManager.h"

class GpuInfoAmd
{
public:
    GpuInfoAmd();

    std::string getModel() const {
        return m_model;
    }

    std::string getManufacturer() const {
        return m_manufacturer;
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
    std::string m_model{ "" };
    std::string m_manufacturer{ "" };
    uint8_t m_totalLoad{ 0 };
    uint8_t m_temperature{ 0 };
    AdlManager* m_adlManager{ nullptr };
};
