#pragma once

#include "../main/BaseInfo.h"

#ifdef _WIN32
class SystemInfoWindows;
#elif __linux__
class SystemInfoLinux;
#endif

#include <cstdint>

class SystemInfo : public BaseInfo
{
public:

    SystemInfo();
    ~SystemInfo();

    virtual void init() override;
    virtual void update() override;

private:
    void readStaticInfo();
    void readDynamicInfo();

#ifdef _WIN32
    SystemInfoWindows* m_systemInfoWindows{ nullptr };
#elif __linux__
    SystemInfoLinux* m_systemInfoLinux{ nullptr };
#endif

    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;
};
