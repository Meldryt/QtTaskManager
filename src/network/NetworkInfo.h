#pragma once

#include "../main/BaseInfo.h"

class NetworkInfo : public BaseInfo
{
public:

    NetworkInfo();
    ~NetworkInfo();

    virtual void init() override;
    virtual void update() override;

private:
    void readStaticInfo();
    void readDynamicInfo();
    void readInterfaces();

    std::vector<std::string> m_networkNames;
    std::vector<uint32_t> m_networkBytesReceivedPerSec;
    std::vector<uint32_t> m_networkBytesSentPerSec;
    std::vector<uint32_t> m_networkBytesTotalPerSec;
};

