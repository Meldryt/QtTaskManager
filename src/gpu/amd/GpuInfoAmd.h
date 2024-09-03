#pragma once

#include <QMap>
#include <QVariant>

class AdlxHandler;
class AgsHandler;

class GpuInfoAmd
{
public:
    GpuInfoAmd();
    ~GpuInfoAmd();

    const QMap<uint8_t,QVariant>& staticInfo() const;
    const QMap<uint8_t,QVariant>& dynamicInfo() const;

    bool init();
    void readStaticInfo();
    void readDynamicInfo();

private:

    AdlxHandler* m_adlxHandler{ nullptr };
    AgsHandler* m_agsHandler{ nullptr };

    QMap<uint8_t,QVariant> m_staticInfo;
};
