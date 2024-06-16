#pragma once

#include <QObject>
#include <string>
#include "AdlManager.h"

class GpuInfoAmd
{
public:
    GpuInfoAmd();

    const Globals::GpuStaticInfo& staticInfo() const
    {
        return m_adlManager->staticInfo();
    }

    const Globals::GpuDynamicInfo& dynamicInfo() const
    {
        return m_adlManager->dynamicInfo();
    }

    bool detectGpu();
    void fetchStaticInfo();
    void fetchDynamicInfo();

private:
    AdlManager* m_adlManager{ nullptr };
};
