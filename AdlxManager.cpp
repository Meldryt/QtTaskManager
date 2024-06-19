
#include "AdlxManager.h"
#include <string>
#include <iostream>

#include <QDebug>

#include "external/ADLX/SDK/Include/ISystem1.h"

// ADLXHelper instance
// No outstanding interfaces from ADLX must exist when ADLX is destroyed.
// Use global variables to ensure validity of the interface.
static ADLXHelper g_ADLXHelp;

AdlxManager::AdlxManager()
{
	m_staticInfo.chipDesigner = "AMD";
}

AdlxManager::~AdlxManager()
{
    ADLX_RESULT res = g_ADLXHelp.Terminate();
    std::cout << "Destroy ADLX result: " << res << std::endl;
}

bool AdlxManager::init()
{

    ADLX_RESULT res = ADLX_FAIL;

    // Initialize ADLX
    res = g_ADLXHelp.Initialize();

    if (ADLX_SUCCEEDED(res))
    {
        // Get Performance Monitoring services
        //IADLXPerformanceMonitoringServicesPtr perfMonitoringService;
        ADLX_RESULT res = g_ADLXHelp.GetSystemServices()->GetPerformanceMonitoringServices(&m_perfMonitoringService);
        if (ADLX_SUCCEEDED(res))
        {
            IADLXGPUListPtr gpus;
            // Get GPU list
            res = g_ADLXHelp.GetSystemServices()->GetGPUs(&gpus);
            if (ADLX_SUCCEEDED(res))
            {
                // Use the first GPU in the list
                //IADLXGPUPtr oneGPU;
                res = gpus->At(gpus->Begin(), &m_oneGPU);
                if (ADLX_SUCCEEDED(res))
                {
                    ShowGPUInfo();
                    //// Display main menu options
                    //MainMenu();
                    //// Get and execute the choice
                    //MenuControl(perfMonitoringService, oneGPU);
                }
                else
                    std::cout << "\tGet particular GPU failed" << std::endl;
            }
            else
                std::cout << "\tGet GPU list failed" << std::endl;
        }
        else
            std::cout << "\tGet performance monitoring services failed" << std::endl;
    }
    //else
    //    return WaitAndExit("\tg_ADLXHelp initialize failed", 0);

    // Destroy ADLX
    //res = g_ADLXHelp.Terminate();
    //std::cout << "Destroy ADLX result: " << res << std::endl;

	return true;
}

bool AdlxManager::fetchStaticInfo()
{
    //ShowCurrentAllMetrics(m_perfMonitoringService, m_oneGPU);
	return true;
}

bool AdlxManager::fetchDynamicInfo()
{
    ShowCurrentAllMetrics(m_perfMonitoringService, m_oneGPU);
	return true;
}

// Get and dump GPU vender id and name
void AdlxManager::ShowGPUInfo()
{
    std::cout << "\n==== GPU info ====" << std::endl;
    // Display GPU info
    const char* vendorId = nullptr;
    ADLX_RESULT ret = m_oneGPU->VendorId(&vendorId);
    std::cout << "VendorId: " << vendorId << "return code is: " << ret << "(0 means success)" << std::endl;

    ADLX_ASIC_FAMILY_TYPE asicFamilyType = ASIC_UNDEFINED;
    ret = m_oneGPU->ASICFamilyType(&asicFamilyType);
    std::cout << "ASICFamilyType: " << asicFamilyType << "return code is: " << ret << "(0 means success)" << std::endl;

    ADLX_GPU_TYPE gpuType = GPUTYPE_UNDEFINED;
    ret = m_oneGPU->Type(&gpuType);
    std::cout << "Type: " << gpuType << std::endl;

    adlx_bool isExternal = false;
    ret = m_oneGPU->IsExternal(&isExternal);
    std::cout << "IsExternal: " << isExternal << ", return code is: " << ret << "(0 means success)" << std::endl;

    const char* gpuName = nullptr;
    ret = m_oneGPU->Name(&gpuName);
    std::cout << "Name: " << gpuName << ", return code is: " << ret << "(0 means success)" << std::endl;

    const char* driverPath = nullptr;
    ret = m_oneGPU->DriverPath(&driverPath);
    std::cout << "DriverPath: " << driverPath << ", return code is: " << ret << "(0 means success)" << std::endl;

    const char* pnpString = nullptr;
    ret = m_oneGPU->PNPString(&pnpString);
    std::cout << "PNPString: " << pnpString << ", return code is: " << ret << "(0 means success)" << std::endl;

    adlx_bool hasDesktops = false;
    ret = m_oneGPU->HasDesktops(&hasDesktops);
    std::cout << "HasDesktops: " << hasDesktops << ", return code is: " << ret << "(0 means success)" << std::endl;

    adlx_uint totalVRAM;
    ret = m_oneGPU->TotalVRAM(&totalVRAM);
    std::cout << "TotalVRAM: " << totalVRAM << " MB" << ", return code is: " << ret << "(0 means success)" << std::endl;

    const char* vramTypeString = nullptr;
    ret = m_oneGPU->VRAMType(&vramTypeString);
    std::cout << "VRAMType: " << vramTypeString << ", return code is: " << ret << "(0 means success)" << std::endl;

    adlx_int id;
    ret = m_oneGPU->UniqueId(&id);
    std::cout << "UniqueId: " << id << std::endl;

    const char* subSystemIdString = nullptr;
    ret = m_oneGPU->SubSystemId(&subSystemIdString);
    std::cout << "SubSystemId: " << subSystemIdString << ", return code is: " << ret << "(0 means success)" << std::endl;

    const char* subSystemVendorIdString = nullptr;
    ret = m_oneGPU->SubSystemVendorId(&subSystemVendorIdString);
    std::cout << "SubSystemVendorId: " << subSystemVendorIdString << ", return code is: " << ret << "(0 means success)" << std::endl;
    const int subSystemVendorId = std::stoi(subSystemVendorIdString, 0, 16);

    const char* revisionId = nullptr;
    ret = m_oneGPU->RevisionId(&revisionId);
    std::cout << "RevisionId: " << revisionId << ", return code is: " << ret << "(0 means success)" << std::endl;

    if (MapVendorIdName.find(subSystemVendorId) != MapVendorIdName.end())
    {
        m_staticInfo.cardManufacturer = MapVendorIdName.at(subSystemVendorId);
    }
    m_staticInfo.gpuModel = gpuName;
    m_staticInfo.memorySize = totalVRAM;
    m_staticInfo.memoryType = vramTypeString;

    IADLXGPU1Ptr gpu1(m_oneGPU);
    if (gpu1)
    {
        const char* productName = nullptr;
        ret = gpu1->ProductName(&productName);
        std::cout << "ProductName: " << productName << std::endl;

        ADLX_MGPU_MODE mode = MGPU_NONE;
        ret = gpu1->MultiGPUMode(&mode);
        printf("Multi-GPU Mode: ");
        if (mode == MGPU_PRIMARY)
            std::cout << "GPU is the primary GPU" << std::endl;
        else if (mode == MGPU_SECONDARY)
            std::cout << "GPU is the secondary GPU" << std::endl;
        else
            std::cout << "GPU is not in Multi-GPU" << std::endl;

        ADLX_PCI_BUS_TYPE busType = UNDEFINED;
        ret = gpu1->PCIBusType(&busType);
        std::cout << "PCIBusType: " << busType << std::endl;

        adlx_uint laneWidth = 0;
        ret = gpu1->PCIBusLaneWidth(&laneWidth);
        std::cout << "PCIBusLaneWidth: " << laneWidth << std::endl;
    }
}

// Show current all metrics
void AdlxManager::ShowCurrentAllMetrics(IADLXPerformanceMonitoringServicesPtr perfMonitoringServices, IADLXGPUPtr oneGPU)
{
    // Get system metrics support
    IADLXSystemMetricsSupportPtr systemMetricsSupport;
    ADLX_RESULT res2 = perfMonitoringServices->GetSupportedSystemMetrics(&systemMetricsSupport);
    // Get GPU metrics support
    IADLXGPUMetricsSupportPtr gpuMetricsSupport;
    res2 = perfMonitoringServices->GetSupportedGPUMetrics(oneGPU, &gpuMetricsSupport);

    // Loop 10 time to accumulate data and show the current metrics in each loop
    IADLXAllMetricsPtr allMetrics;
    IADLXSystemMetricsPtr systemMetrics;
    IADLXGPUMetricsPtr gpuMetrics;
    IADLXFPSPtr oneFPS;

    // Get current All metrics
    ADLX_RESULT res1 = perfMonitoringServices->GetCurrentAllMetrics(&allMetrics);
    if (ADLX_SUCCEEDED(res1))
    {
        std::cout << "The current all metrics: " << std::endl;
        // Get current system metrics.
        res1 = allMetrics->GetSystemMetrics(&systemMetrics);
        // Show timestamp, CPU usage, system RAM and smart shift
        if (ADLX_SUCCEEDED(res1) && ADLX_SUCCEEDED(res2))
        {
            std::cout << std::boolalpha;  // display bool variable as true of false
            GetTimeStamp(systemMetrics);
            ShowCPUUsage(systemMetricsSupport, systemMetrics);
            ShowSystemRAM(systemMetricsSupport, systemMetrics);
            ShowSmartShift(systemMetricsSupport, systemMetrics);
            std::cout << std::noboolalpha;
        }
        // Get current GPU metrics
        res1 = allMetrics->GetGPUMetrics(oneGPU, &gpuMetrics);
        // Show timestamp and GPU metrics
        if (ADLX_SUCCEEDED(res1) && ADLX_SUCCEEDED(res2))
        {
            std::cout << std::boolalpha;  // display bool variable as true of false
            GetTimeStamp(gpuMetrics);
            ShowGPUUsage(gpuMetricsSupport, gpuMetrics);
            ShowGPUClockSpeed(gpuMetricsSupport, gpuMetrics);
            ShowGPUVRAMClockSpeed(gpuMetricsSupport, gpuMetrics);
            ShowGPUTemperature(gpuMetricsSupport, gpuMetrics);
            ShowGPUHotspotTemperature(gpuMetricsSupport, gpuMetrics);
            ShowGPUPower(gpuMetricsSupport, gpuMetrics);
            ShowGPUFanSpeed(gpuMetricsSupport, gpuMetrics);
            ShowGPUVRAM(gpuMetricsSupport, gpuMetrics);
            ShowGPUVoltage(gpuMetricsSupport, gpuMetrics);
            std::cout << std::noboolalpha;
        }
        // Get current FPS metrics
        //res1 = allMetrics->GetFPS(&oneFPS);
        //if (ADLX_SUCCEEDED(res1))
        //{
        //    adlx_int64 timeStamp = 0;
        //    res1 = oneFPS->TimeStamp(&timeStamp);
        //    if (ADLX_SUCCEEDED(res1))
        //        std::cout << "The current metric time stamp is: " << timeStamp << "ms\n";
        //    adlx_int fpsData = 0;
        //    res1 = oneFPS->FPS(&fpsData);
        //    if (ADLX_SUCCEEDED(res1))
        //        std::cout << "The current metric FPS is: " << fpsData << std::endl;
        //    else if (res1 == ADLX_NOT_SUPPORTED)
        //        std::cout << "Don't support FPS" << std::endl;
        //}
    }
}

// Display the system time stamp (in ms)
void AdlxManager::GetTimeStamp(IADLXSystemMetricsPtr systemMetrics)
{
    adlx_int64 timeStamp = 0;
    ADLX_RESULT res = systemMetrics->TimeStamp(&timeStamp);
    if (ADLX_SUCCEEDED(res))
        std::cout << "The system time stamp is: " << timeStamp << "ms" << std::endl;
}

// Show CPU usage(in %)
void AdlxManager::ShowCPUUsage(IADLXSystemMetricsSupportPtr systemMetricsSupport, IADLXSystemMetricsPtr systemMetrics)
{
    adlx_bool supported = false;
    // Display CPU usage support status
    ADLX_RESULT res = systemMetricsSupport->IsSupportedCPUUsage(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "CPU usage support status: " << supported << std::endl;
        if (supported)
        {
            adlx_double cpuUsage = 0;
            res = systemMetrics->CPUUsage(&cpuUsage);
            if (ADLX_SUCCEEDED(res))
                std::cout << "The CPU usage is: " << cpuUsage << "%" << std::endl;
        }
    }
}

// Display system RAM (in MB)
void AdlxManager::ShowSystemRAM(IADLXSystemMetricsSupportPtr systemMetricsSupport, IADLXSystemMetricsPtr systemMetrics)
{
    adlx_bool supported = false;
    // Display system RAM usage support status
    ADLX_RESULT res = systemMetricsSupport->IsSupportedSystemRAM(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "System RAM usage support status: " << supported << std::endl;
        if (supported)
        {
            adlx_int systemRAM = 0;
            res = systemMetrics->SystemRAM(&systemRAM);
            if (ADLX_SUCCEEDED(res))
                std::cout << "The system RAM is: " << systemRAM << "MB" << std::endl;
        }
    }
}

// Display SmartShift
void AdlxManager::ShowSmartShift(IADLXSystemMetricsSupportPtr systemMetricsSupport, IADLXSystemMetricsPtr systemMetrics)
{
    adlx_bool supported = false;
    // Display SmartShift support status
    ADLX_RESULT res = systemMetricsSupport->IsSupportedSmartShift(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "SmartShift support status: " << supported << std::endl;
        if (supported)
        {
            adlx_int smartShift;
            res = systemMetrics->SmartShift(&smartShift);
            if (ADLX_SUCCEEDED(res))
                std::cout << "The SmartShift is: " << smartShift << std::endl;
        }
    }
}

// Display the GPU metrics time stamp (in ms)
void AdlxManager::GetTimeStamp(IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_int64 timeStamp = 0;
    ADLX_RESULT res = gpuMetrics->TimeStamp(&timeStamp);
    if (ADLX_SUCCEEDED(res))
        std::cout << "The GPU time stamp is: " << timeStamp << "ms" << std::endl;
}

// Display GPU usage (in %)
void AdlxManager::ShowGPUUsage(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Display GPU usage support status
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUUsage(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "GPU usage support status: " << supported << std::endl;
        if (supported)
        {
            adlx_double usage = 0;
            res = gpuMetrics->GPUUsage(&usage);
            if (ADLX_SUCCEEDED(res))
                m_dynamicInfo.gpuGraphicsUsage = usage;
        }
    }
}

// Display GPU clock speed (in MHz)
void AdlxManager::ShowGPUClockSpeed(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Display GPU clock speed support status
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUClockSpeed(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "Get if the GPU clock speed is supported: " << supported << std::endl;
        if (supported)
        {
            adlx_int gpuClock = 0;
            res = gpuMetrics->GPUClockSpeed(&gpuClock);
            if (ADLX_SUCCEEDED(res))
                m_dynamicInfo.gpuGraphicsClock = gpuClock;
        }
    }
}

// Show GPU VRAM clock speed(MHz)
void AdlxManager::ShowGPUVRAMClockSpeed(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Get if the GPU VRAM clock speed is supported
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUVRAMClockSpeed(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "Get if the GPU VRAM clock speed is supported: " << supported << std::endl;
        if (supported)
        {
            adlx_int memoryClock = 0;
            res = gpuMetrics->GPUVRAMClockSpeed(&memoryClock);
            if (ADLX_SUCCEEDED(res))
                m_dynamicInfo.gpuMemoryClock = memoryClock;
        }
    }
}

// Show GPU temperature(°C)
void AdlxManager::ShowGPUTemperature(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Get if the GPU tempetature is supported
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUTemperature(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "Get if the GPU temperture is supported: " << supported << std::endl;
        if (supported)
        {
            adlx_double temperature = 0;
            res = gpuMetrics->GPUTemperature(&temperature);
            if (ADLX_SUCCEEDED(res))
                m_dynamicInfo.gpuTemperature = temperature;
        }
    }
}

// Show GPU hotspot temperature(°C)
void AdlxManager::ShowGPUHotspotTemperature(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Get if the GPU hotspot temperature is supported
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUHotspotTemperature(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "Get if the GPU hotspot temperature is supported: " << supported << std::endl;
        if (supported)
        {
            adlx_double hotspotTemperature = 0;
            res = gpuMetrics->GPUHotspotTemperature(&hotspotTemperature);
            if (ADLX_SUCCEEDED(res))
                m_dynamicInfo.gpuTemperatureHotspot = hotspotTemperature;
        }
    }
}

// Show GPU power(W)
void AdlxManager::ShowGPUPower(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Get if the GPU power is supported
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUPower(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "Get if the GPU power is supported: " << supported << std::endl;
        if (supported)
        {
            adlx_double power = 0;
            res = gpuMetrics->GPUPower(&power);
            if (ADLX_SUCCEEDED(res))
                m_dynamicInfo.gpuGraphicsPower = power;
        }
    }
}

// Show GPU fan speed(RPM)
void AdlxManager::ShowGPUFanSpeed(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Get if the GPU fan speed is supported
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUFanSpeed(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "Get if the GPU fan speed is supported: " << supported << std::endl;
        if (supported)
        {
            adlx_int fanSpeed = 0;
            res = gpuMetrics->GPUFanSpeed(&fanSpeed);
            if (ADLX_SUCCEEDED(res))
                m_dynamicInfo.gpuFanSpeed = fanSpeed;
        }
    }
}

// Show GPU VRAM(MB)
void AdlxManager::ShowGPUVRAM(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Get if the GPU VRAM is supported
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUVRAM(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "Get if the GPU VRAM is supported: " << supported << std::endl;
        if (supported)
        {
            adlx_int VRAM = 0;
            res = gpuMetrics->GPUVRAM(&VRAM);
            if (ADLX_SUCCEEDED(res))
                m_dynamicInfo.gpuMemoryUsage = VRAM;
        }
    }
}

// Show GPU Voltage(mV)
void AdlxManager::ShowGPUVoltage(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Get if the GPU voltage is supported
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUVoltage(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        std::cout << "Get if the GPU voltage is supported: " << supported << std::endl;
        if (supported)
        {
            adlx_int voltage = 0;
            res = gpuMetrics->GPUVoltage(&voltage);
            if (ADLX_SUCCEEDED(res))
                m_dynamicInfo.gpuGraphicsVoltage = voltage;
        }
    }
}