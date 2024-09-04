#pragma once

#include <QMap>
#include <QVariant>

#ifdef _WIN32
class RyzenMasterSdkHandler;
#endif
class CpuInfoAmd
{
public:
    CpuInfoAmd();
    ~CpuInfoAmd();

    const QMap<uint8_t,QVariant>& staticInfo() const;
    const QMap<uint8_t,QVariant>& dynamicInfo() const;

    void init();
    void readStaticInfo();
    void readDynamicInfo();

private:

#ifdef _WIN32
    RyzenMasterSdkHandler* m_ryzenMasterSdkHandler{ nullptr };
#endif
    QMap<uint8_t,QVariant> m_staticInfo;
    QMap<uint8_t,QVariant> m_dynamicInfo;
};
