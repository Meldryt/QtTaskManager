#pragma once

#include <QObject>
#include <string>

#include "Globals.h"

class GpuInfoNVidia
{
public:
    GpuInfoNVidia();

    const Globals::GpuStaticInfo& staticInfo() const
    {
        return m_staticInfo;
    }

    const Globals::GpuDynamicInfo& dynamicInfo() const
    {
        return m_dynamicInfo;
    }

    bool detectGpu();
    void fetchStaticInfo();
    void fetchDynamicInfo();

private:

    Globals::GpuStaticInfo m_staticInfo;
    Globals::GpuDynamicInfo m_dynamicInfo;
};
