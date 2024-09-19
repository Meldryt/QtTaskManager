#pragma once

#include <QMap>
#include <QVariant>

#ifdef _WIN32
class SystemInfoWindows;
#elif __linux__
class SystemInfoLinux;
#endif

#include <cstdint>

class SystemInfo
{
public:

    SystemInfo();
    ~SystemInfo();

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

#ifdef _WIN32
    SystemInfoWindows* m_systemInfoWindows{ nullptr };
#elif __linux__
    SystemInfoLinux* m_systemInfoLinux{ nullptr };
#endif

    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;
};
