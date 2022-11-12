#pragma once

#include <QObject>
#include <cstdint>

class MemoryInfo
{
public:

    struct Memory_StaticInfo
    {
        uint32_t totalVirtualMemory{0};
        uint32_t totalPhysicalMemory{0};
    };

    struct Memory_DynamicInfo
    {
        uint32_t usedVirtualMemory{0};
        uint32_t usedPhysicalMemory{0};
    };

    MemoryInfo();

    void init();
    void update();

    const Memory_StaticInfo &getStaticInfo() const;
    const Memory_DynamicInfo &getDynamicInfo() const;

private:
    void fetchStaticInfo();
    void fetchDynamicInfo();

    Memory_StaticInfo staticInfo;
    Memory_DynamicInfo dynamicInfo;
};

