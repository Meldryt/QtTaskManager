#pragma once

#include <string>
#include <vector>

#include <QMap>
#include <QVariant>

class PcmHandler;

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

    std::unique_ptr<PcmHandler> m_pcmHandler{ nullptr };

};

