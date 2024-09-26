#pragma once

#include "../main/BaseInfo.h"

class NetworkInfo : public BaseInfo
{
public:
    struct Network
    {
        std::string name;
        uint32_t bytesReceivedPerSec;
        uint32_t bytesSentPerSec;
        uint32_t bytesTotalPerSec;
    };

    NetworkInfo();
    ~NetworkInfo();

    virtual void init() override;
    virtual void update() override;

private:
    void readStaticInfo();
    void readDynamicInfo();
    void readInterfaces();

    std::map<std::string, Network> m_networks;
};

