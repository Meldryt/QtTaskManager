#pragma once

#include <QObject>
#include <QMap>
#include <QVariant>

#include <cstdint>

#include "Globals.h"

class MemoryInfo
{
public:

    MemoryInfo();
    ~MemoryInfo();

    void init();
    void update();

    const QMap<uint8_t, QVariant>& getStaticInfo() const
    {
        return m_staticInfo;
    }

    const QMap<uint8_t, QVariant>& getDynamicInfo() const
    {
        return m_dynamicInfo;
    }

private:
    void readStaticInfo();
    void readDynamicInfo();

    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;

    uint32_t m_totalVirtualMemory{0};
    uint32_t m_totalPhysicalMemory{0};

    uint32_t m_usedVirtualMemory{0};
    uint32_t m_usedPhysicalMemory{0};
};

