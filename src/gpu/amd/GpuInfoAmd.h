#pragma once

#include <QMap>
#include <QVariant>

#ifdef _WIN32
class AdlxHandler;
class AgsHandler;
#endif

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
#ifdef _WIN32
    AdlxHandler* m_adlxHandler{ nullptr };
    AgsHandler* m_agsHandler{ nullptr };
#endif
    QMap<uint8_t,QVariant> m_staticInfo;
};
