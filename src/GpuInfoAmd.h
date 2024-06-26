#pragma once

#include <QObject>
#include <string>
#include "AdlxManager.h"

class GpuInfoAmd
{
public:
    GpuInfoAmd();

    const Globals::GpuStaticInfo& staticInfo() const
    {
        return m_adlxManager->staticInfo();
    }

    const Globals::GpuDynamicInfo& dynamicInfo() const
    {
        return m_adlxManager->dynamicInfo();
    }

    bool detectGpu();
    void fetchStaticInfo();
    void fetchDynamicInfo();

private:
    void initAgs();

    AdlxManager* m_adlxManager{ nullptr };
};
