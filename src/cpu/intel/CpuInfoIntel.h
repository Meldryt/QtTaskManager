#pragma once

#include <string>
#include <vector>

#include <QMap>
#include <QVariant>

class CpuInfoIntel
{
public:
    CpuInfoIntel();
    ~CpuInfoIntel();

    const QMap<uint8_t, QVariant>& staticInfo() const;
    const QMap<uint8_t, QVariant>& dynamicInfo() const;

    void init();
    void readStaticInfo();
    void readDynamicInfo();

private:

    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;
};

