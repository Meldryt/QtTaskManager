#pragma once

#include <QMap>
#include <QVariant>

#include <vector>
#include <string>
#include <map>

class SystemInfoWindows
{
public:
    SystemInfoWindows();
    ~SystemInfoWindows();

    bool init();
    void update();
    void readStaticInfo();
    void readDynamicInfo();

    const std::string& osName() const { return m_osName; };
    const std::string& osCodeName() const { return m_osCodeName; };
    const std::string& osVersion() const { return m_osVersion; };

private:

    std::string m_osName{""};
    std::string m_osCodeName{""};
    std::string m_osVersion{""};
};
