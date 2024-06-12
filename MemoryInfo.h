#pragma once

#include <QObject>
#include <cstdint>

class MemoryInfo
{
public:

    struct MemoryStaticInfo
    {
        uint32_t totalVirtualMemory{0};
        uint32_t totalPhysicalMemory{0};
    };

    struct MemoryDynamicInfo
    {
        uint32_t usedVirtualMemory{0};
        uint32_t usedPhysicalMemory{0};
    };

    MemoryInfo();

    void init();
    void update();

    const MemoryStaticInfo &getStaticInfo() const;
    const MemoryDynamicInfo &getDynamicInfo() const;

private:
    void fetchStaticInfo();
    void fetchDynamicInfo();

    MemoryStaticInfo staticInfo;
    MemoryDynamicInfo dynamicInfo;
};

