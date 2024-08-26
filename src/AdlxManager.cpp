
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
    qDebug() << __FUNCTION__;

	m_gpuChipDesigner = "AMD";
}

AdlxManager::~AdlxManager()
{
    qDebug() << __FUNCTION__;

    ADLX_RESULT res = g_ADLXHelp.Terminate();
    qDebug() << "Destroy ADLX result: " << res;
}

bool AdlxManager::init()
{

    ADLX_RESULT res = ADLX_FAIL;

    // Initialize ADLX
    res = g_ADLXHelp.Initialize();

    if (!ADLX_SUCCEEDED(res))
    {
        qDebug() << "\tg_ADLXHelp initialize failed";
    }
    else
    {
        m_initialized = true;

        // Get Performance Monitoring services
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
                    qDebug() << "\tGet particular GPU failed";
            }
            else
                qDebug() << "\tGet GPU list failed";
        }
        else
        {
            qDebug() << "\tGet performance monitoring services failed";
        }
    }
    //else
    //    return WaitAndExit("\tg_ADLXHelp initialize failed", 0);

    // Destroy ADLX
    //res = g_ADLXHelp.Terminate();
    //qDebug() << "Destroy ADLX result: " << res;

	return true;
}

bool AdlxManager::fetchStaticInfo()
{
    if (!m_initialized)
    {
        return false;
    }

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
                qDebug() << "\tGet particular GPU failed";
        }
        else
            qDebug() << "\tGet GPU list failed";
    }
    else
    {
        qDebug() << "\tGet performance monitoring services failed";
    }

    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_ChipDesigner] = QString::fromStdString(m_gpuChipDesigner.c_str());
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_CardManufacturer] = QString::fromStdString(m_gpuCardManufacturer);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Model] = QString::fromStdString(m_gpuModel);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_MemorySize] = m_gpuMemorySize;
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_MemoryType] = QString::fromStdString(m_gpuMemoryType);
        
	return true;
}

bool AdlxManager::fetchDynamicInfo()
{
    ShowCurrentAllMetrics(m_perfMonitoringService, m_oneGPU);

    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Usage] = m_gpuUsage;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_ClockSpeed] = m_gpuClockSpeed;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamUsage] = m_gpuVramUsage;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamClockSpeed] = m_gpuVramClockSpeed;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamUsed] = m_gpuVramUsed;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Power] = m_gpuPower;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_TotalBoardPower] = m_gpuTotalBoardPower;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Voltage] = m_gpuVoltage;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Temperature] = m_gpuTemperature;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_HotSpotTemperature] = m_gpuHotspotTemperature;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_FanSpeed] = m_gpuFanSpeed;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_FanSpeedUsage] = m_gpuFanSpeedUsage;

	return true;
}

// Get and dump GPU vender id and name
void AdlxManager::ShowGPUInfo()
{
    qDebug() << "\n==== GPU info ====";
    // Display GPU info
    const char* vendorId = nullptr;
    ADLX_RESULT ret = m_oneGPU->VendorId(&vendorId);
    qDebug() << "VendorId: " << vendorId << "return code is: " << ret << "(0 means success)";

    ADLX_ASIC_FAMILY_TYPE asicFamilyType = ASIC_UNDEFINED;
    ret = m_oneGPU->ASICFamilyType(&asicFamilyType);
    qDebug() << "ASICFamilyType: " << asicFamilyType << "return code is: " << ret << "(0 means success)";

    ADLX_GPU_TYPE gpuType = GPUTYPE_UNDEFINED;
    ret = m_oneGPU->Type(&gpuType);
    qDebug() << "Type: " << gpuType;

    adlx_bool isExternal = false;
    ret = m_oneGPU->IsExternal(&isExternal);
    qDebug() << "IsExternal: " << isExternal << ", return code is: " << ret << "(0 means success)";

    const char* gpuName = nullptr;
    ret = m_oneGPU->Name(&gpuName);
    qDebug() << "Name: " << gpuName << ", return code is: " << ret << "(0 means success)";

    const char* driverPath = nullptr;
    ret = m_oneGPU->DriverPath(&driverPath);
    qDebug() << "DriverPath: " << driverPath << ", return code is: " << ret << "(0 means success)";

    const char* pnpString = nullptr;
    ret = m_oneGPU->PNPString(&pnpString);
    qDebug() << "PNPString: " << pnpString << ", return code is: " << ret << "(0 means success)";

    adlx_bool hasDesktops = false;
    ret = m_oneGPU->HasDesktops(&hasDesktops);
    qDebug() << "HasDesktops: " << hasDesktops << ", return code is: " << ret << "(0 means success)";

    adlx_uint totalVRAM;
    ret = m_oneGPU->TotalVRAM(&totalVRAM);
    qDebug() << "TotalVRAM: " << totalVRAM << " MB" << ", return code is: " << ret << "(0 means success)";

    const char* vramTypeString = nullptr;
    ret = m_oneGPU->VRAMType(&vramTypeString);
    qDebug() << "VRAMType: " << vramTypeString << ", return code is: " << ret << "(0 means success)";

    adlx_int id;
    ret = m_oneGPU->UniqueId(&id);
    qDebug() << "UniqueId: " << id;

    const char* subSystemIdString = nullptr;
    ret = m_oneGPU->SubSystemId(&subSystemIdString);
    qDebug() << "SubSystemId: " << subSystemIdString << ", return code is: " << ret << "(0 means success)";

    const char* subSystemVendorIdString = nullptr;
    ret = m_oneGPU->SubSystemVendorId(&subSystemVendorIdString);
    qDebug() << "SubSystemVendorId: " << subSystemVendorIdString << ", return code is: " << ret << "(0 means success)";
    const int subSystemVendorId = std::stoi(subSystemVendorIdString, 0, 16);

    const char* revisionId = nullptr;
    ret = m_oneGPU->RevisionId(&revisionId);
    qDebug() << "RevisionId: " << revisionId << ", return code is: " << ret << "(0 means success)";

    IADLXGPU1Ptr gpu1(m_oneGPU);
    if (gpu1)
    {
        const char* productName = nullptr;
        ret = gpu1->ProductName(&productName);
        qDebug() << "ProductName: " << productName;

        ADLX_MGPU_MODE mode = MGPU_NONE;
        ret = gpu1->MultiGPUMode(&mode);
        printf("Multi-GPU Mode: ");
        if (mode == MGPU_PRIMARY)
            qDebug() << "GPU is the primary GPU";
        else if (mode == MGPU_SECONDARY)
            qDebug() << "GPU is the secondary GPU";
        else
            qDebug() << "GPU is not in Multi-GPU";

        ADLX_PCI_BUS_TYPE busType = UNDEFINED;
        ret = gpu1->PCIBusType(&busType);
        qDebug() << "PCIBusType: " << busType;

        adlx_uint laneWidth = 0;
        ret = gpu1->PCIBusLaneWidth(&laneWidth);
        qDebug() << "PCIBusLaneWidth: " << laneWidth;
    }

    if (MapVendorIdName.find(subSystemVendorId) != MapVendorIdName.end())
    {
        m_gpuCardManufacturer = MapVendorIdName.at(subSystemVendorId);
    }

    m_gpuModel = gpuName;
    m_gpuMemorySize = totalVRAM;
    m_gpuMemoryType = vramTypeString;
}

// Show current all metrics
void AdlxManager::ShowCurrentAllMetrics(IADLXPerformanceMonitoringServicesPtr perfMonitoringServices, IADLXGPUPtr oneGPU)
{
    if (!m_initialized)
    {
        return;
    }

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
        //qDebug() << "The current all metrics: ";
        // Get current system metrics.
        res1 = allMetrics->GetSystemMetrics(&systemMetrics);
        // Show timestamp, CPU usage, system RAM and smart shift
        if (ADLX_SUCCEEDED(res1) && ADLX_SUCCEEDED(res2))
        {
            //qDebug() << std::boolalpha;  // display bool variable as true of false
            GetTimeStamp(systemMetrics);
            ShowCPUUsage(systemMetricsSupport, systemMetrics);
            ShowSystemRAM(systemMetricsSupport, systemMetrics);
            ShowSmartShift(systemMetricsSupport, systemMetrics);
            //qDebug() << std::noboolalpha;
        }
        // Get current GPU metrics
        res1 = allMetrics->GetGPUMetrics(oneGPU, &gpuMetrics);
        // Show timestamp and GPU metrics
        if (ADLX_SUCCEEDED(res1) && ADLX_SUCCEEDED(res2))
        {
            //qDebug() << std::boolalpha;  // display bool variable as true of false
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
            ShowGPUTotalBoardPower(gpuMetricsSupport, gpuMetrics);
            ShowGPUIntakeTemperature(gpuMetricsSupport, gpuMetrics);
            //qDebug() << std::noboolalpha;
        }
        // Get current FPS metrics
        //res1 = allMetrics->GetFPS(&oneFPS);
        //if (ADLX_SUCCEEDED(res1))
        //{
        //    adlx_int64 timeStamp = 0;
        //    res1 = oneFPS->TimeStamp(&timeStamp);
        //    if (ADLX_SUCCEEDED(res1))
        //        qDebug() << "The current metric time stamp is: " << timeStamp << "ms\n";
        //    adlx_int fpsData = 0;
        //    res1 = oneFPS->FPS(&fpsData);
        //    if (ADLX_SUCCEEDED(res1))
        //        qDebug() << "The current metric FPS is: " << fpsData;
        //    else if (res1 == ADLX_NOT_SUPPORTED)
        //        qDebug() << "Don't support FPS";
        //}
    }
}

// Display the system time stamp (in ms)
void AdlxManager::GetTimeStamp(IADLXSystemMetricsPtr systemMetrics)
{
    adlx_int64 timeStamp = 0;
    ADLX_RESULT res = systemMetrics->TimeStamp(&timeStamp);
    //if (ADLX_SUCCEEDED(res))
        //qDebug() << "The system time stamp is: " << timeStamp << "ms";
}

// Show CPU usage(in %)
void AdlxManager::ShowCPUUsage(IADLXSystemMetricsSupportPtr systemMetricsSupport, IADLXSystemMetricsPtr systemMetrics)
{
    adlx_bool supported = false;
    // Display CPU usage support status
    ADLX_RESULT res = systemMetricsSupport->IsSupportedCPUUsage(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        //qDebug() << "CPU usage support status: " << supported;
        if (supported)
        {
            adlx_double cpuUsage = 0;
            res = systemMetrics->CPUUsage(&cpuUsage);
            //if (ADLX_SUCCEEDED(res))
                //qDebug() << "The CPU usage is: " << cpuUsage << "%";
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
        //qDebug() << "System RAM usage support status: " << supported;
        if (supported)
        {
            adlx_int systemRAM = 0;
            res = systemMetrics->SystemRAM(&systemRAM);
            //if (ADLX_SUCCEEDED(res))
                //qDebug() << "The system RAM is: " << systemRAM << "MB";
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
        //qDebug() << "SmartShift support status: " << supported;
        if (supported)
        {
            adlx_int smartShift;
            res = systemMetrics->SmartShift(&smartShift);
            //if (ADLX_SUCCEEDED(res))
                //qDebug() << "The SmartShift is: " << smartShift;
        }
    }
}

// Display the GPU metrics time stamp (in ms)
void AdlxManager::GetTimeStamp(IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_int64 timeStamp = 0;
    ADLX_RESULT res = gpuMetrics->TimeStamp(&timeStamp);
    //if (ADLX_SUCCEEDED(res))
        //qDebug() << "The GPU time stamp is: " << timeStamp << "ms";
}

// Display GPU usage (in %)
void AdlxManager::ShowGPUUsage(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Display GPU usage support status
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUUsage(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        //qDebug() << "GPU usage support status: " << supported;
        if (supported)
        {
            adlx_double usage = 0;
            res = gpuMetrics->GPUUsage(&usage);
            if (ADLX_SUCCEEDED(res))
                m_gpuUsage = usage;
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
        //qDebug() << "Get if the GPU clock speed is supported: " << supported;
        if (supported)
        {
            adlx_int gpuClock = 0;
            res = gpuMetrics->GPUClockSpeed(&gpuClock);
            if (ADLX_SUCCEEDED(res))
                m_gpuClockSpeed = gpuClock;
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
        //qDebug() << "Get if the GPU VRAM clock speed is supported: " << supported;
        if (supported)
        {
            adlx_int memoryClock = 0;
            res = gpuMetrics->GPUVRAMClockSpeed(&memoryClock);
            if (ADLX_SUCCEEDED(res))
                m_gpuVramClockSpeed = memoryClock;
        }
    }
}

// Show GPU temperature(�C)
void AdlxManager::ShowGPUTemperature(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Get if the GPU tempetature is supported
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUTemperature(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        //qDebug() << "Get if the GPU temperture is supported: " << supported;
        if (supported)
        {
            adlx_double temperature = 0;
            res = gpuMetrics->GPUTemperature(&temperature);
            if (ADLX_SUCCEEDED(res))
                m_gpuTemperature = temperature;
        }
    }
}

// Show GPU hotspot temperature(�C)
void AdlxManager::ShowGPUHotspotTemperature(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Get if the GPU hotspot temperature is supported
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUHotspotTemperature(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        //qDebug() << "Get if the GPU hotspot temperature is supported: " << supported;
        if (supported)
        {
            adlx_double hotspotTemperature = 0;
            res = gpuMetrics->GPUHotspotTemperature(&hotspotTemperature);
            if (ADLX_SUCCEEDED(res))
                m_gpuHotspotTemperature = hotspotTemperature;
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
        //qDebug() << "Get if the GPU power is supported: " << supported;
        if (supported)
        {
            adlx_double power = 0;
            res = gpuMetrics->GPUPower(&power);
            if (ADLX_SUCCEEDED(res))
                m_gpuPower = power;
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
        //qDebug() << "Get if the GPU fan speed is supported: " << supported;
        if (supported)
        {
            adlx_int fanSpeed = 0;
            res = gpuMetrics->GPUFanSpeed(&fanSpeed);
            if (ADLX_SUCCEEDED(res))
                m_gpuFanSpeed = fanSpeed;
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
        //qDebug() << "Get if the GPU VRAM is supported: " << supported;
        if (supported)
        {
            adlx_int VRAM = 0;
            res = gpuMetrics->GPUVRAM(&VRAM);
            if (ADLX_SUCCEEDED(res))
            {
                m_gpuVramUsed = VRAM;
                m_gpuVramUsage = (static_cast<double>(m_gpuVramUsed) / m_gpuMemorySize) * 100.0;
            }
            
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
        //qDebug() << "Get if the GPU voltage is supported: " << supported;
        if (supported)
        {
            adlx_int voltage = 0;
            res = gpuMetrics->GPUVoltage(&voltage);
            if (ADLX_SUCCEEDED(res))
                m_gpuVoltage = voltage;
        }
    }
}

// Show GPU Total Board Power(W)
void AdlxManager::ShowGPUTotalBoardPower(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;
    // Get if the GPU voltage is supported
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUTotalBoardPower(&supported);
    if (ADLX_SUCCEEDED(res))
    {
        //qDebug() << "Get if the GPU voltage is supported: " << supported;
        if (supported)
        {
            adlx_double power = 0;
            res = gpuMetrics->GPUTotalBoardPower(&power);
            if (ADLX_SUCCEEDED(res))
                m_gpuTotalBoardPower = power;
        }
    }
}

// Display GPU intake temperature(in °C)
void AdlxManager::ShowGPUIntakeTemperature (IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_bool supported = false;

    // Display the GPU temperature support status
    ADLX_RESULT res = gpuMetricsSupport->IsSupportedGPUIntakeTemperature (&supported);
    if (ADLX_SUCCEEDED (res))
    {
        //qDebug() << "GPU intake temperature support status: " << supported;
        if (supported)
        {
            adlx_double temperature = 0;
            res = gpuMetrics->GPUIntakeTemperature (&temperature);
            //if (ADLX_SUCCEEDED (res))
            //    qDebug() << "The GPU intake temperature is: " << temperature << g_degree;
        }
    }
}