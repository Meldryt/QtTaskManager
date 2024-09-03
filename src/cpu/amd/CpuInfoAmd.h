#pragma once

#include <QMap>
#include <QVariant>

class RyzenMasterSdkHandler;

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

    RyzenMasterSdkHandler* m_ryzenMasterSdkHandler{ nullptr };

    QMap<uint8_t,QVariant> m_staticInfo;
    QMap<uint8_t,QVariant> m_dynamicInfo;
};
