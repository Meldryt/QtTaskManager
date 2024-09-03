#pragma once

#include <string>

class AgsHandler
{
public:
    AgsHandler();
    ~AgsHandler();

    bool init();

    const std::string& agsVersion() const { return m_agsVersion; };
    const std::string& driverVersion() const { return m_driverVersion; };
    const std::string& softwareVersion() const { return m_softwareVersion; };

private:
    bool m_initialized{false};

    std::string m_agsVersion{""};
    std::string m_driverVersion{""};
    std::string m_softwareVersion{""};
};

