#pragma once

#include <QObject>
#include <cstdint>

#include "Globals.h"

class MemoryInfo
{
public:

    MemoryInfo();

    void init();
    void update();

    const Globals::MemoryStaticInfo &getStaticInfo() const;
    const Globals::MemoryDynamicInfo &getDynamicInfo() const;

private:
    void fetchStaticInfo();
    void fetchDynamicInfo();

    Globals::MemoryStaticInfo staticInfo;
    Globals::MemoryDynamicInfo dynamicInfo;
};

