#pragma once

#include "../main/BaseInfo.h"

class MemoryInfo : public BaseInfo
{
public:

    MemoryInfo();
    ~MemoryInfo();

    virtual void init() override;
    virtual void update() override;

private:
    void readTotalMemory();
    void readUsedMemory();

    uint32_t m_totalVirtualMemory{0};
    uint32_t m_totalPhysicalMemory{0};

    uint32_t m_usedVirtualMemory{0};
    uint32_t m_usedPhysicalMemory{0};
};

