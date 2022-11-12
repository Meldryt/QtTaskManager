#pragma once

#include <QObject>
#include <string>

class GPUInfo
{
public:

    enum GPUManufacturer
    {
        NVIDIA = 0,
        AMD
    };

    struct GPU_StaticInfo
    {
        std::string gpuBrand{""};
    };

    struct GPU_DynamicInfo
    {
        uint8_t gpuTotalLoad{0};
        uint8_t gpuTemperature{0};
    };

    GPUInfo();

    void init();
    void update();

    const GPU_StaticInfo &getStaticInfo() const;
    const GPU_DynamicInfo &getDynamicInfo() const;

private:
    void detectGPU();
    void detectGPU_NVidia();
    void detectGPU_AMD(); //todo: implement

    void fetchStaticInfo();
    void fetchStaticInfo_NVidia();
    void fetchStaticInfo_AMD();

    void fetchDynamicInfo();
    void fetchDynamicInfo_NVidia();
    void fetchDynamicInfo_AMD();

    bool gpuDetected{false};
    GPUManufacturer gpuManufacturer{NVIDIA};

    GPU_StaticInfo staticInfo;
    GPU_DynamicInfo dynamicInfo;
};
