#pragma once

#include <QObject>
#include <string>
#include "AdlxManager.h"

class GpuInfoAmd
{
public:
    GpuInfoAmd();

    const QMap<uint8_t,QVariant>& staticInfo() const
    {
        return m_staticInfo;
    }

    const QMap<uint8_t,QVariant>& dynamicInfo() const
    {
        return m_adlxManager->dynamicInfo();
    }

    bool init();
    void fetchStaticInfo();
    void fetchDynamicInfo();

private:
    void initAgs();

    AdlxManager* m_adlxManager{ nullptr };

    QMap<uint8_t,QVariant> m_staticInfo;
};
