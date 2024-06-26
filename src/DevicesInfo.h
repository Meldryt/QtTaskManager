#pragma once


class DevicesInfo
{
public:
    DevicesInfo();

    void init();
    void update();

private:
    void updateBIOSInfo();
    void updateConnectedDevices();
    void updateDevicesInfo();
};

