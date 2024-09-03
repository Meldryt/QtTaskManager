
#include "AdlxHandler.h"

#include "../../Globals.h"

#include <QDebug>

#include "external/ADLX/SDK/Include/ISystem1.h"

// ADLXHelper instance
// No outstanding interfaces from ADLX must exist when ADLX is destroyed.
// Use global variables to ensure validity of the interface.
static ADLXHelper g_ADLXHelp;

AdlxHandler::AdlxHandler()
{
    qDebug() << __FUNCTION__;

	m_gpuChipDesigner = "AMD";
}

AdlxHandler::~AdlxHandler()
{
    qDebug() << __FUNCTION__;

    ADLX_RESULT res = g_ADLXHelp.Terminate();
    qDebug() << "Destroy ADLX result: " << res;
}

bool AdlxHandler::init()
{
    ADLX_RESULT res = ADLX_FAIL;

    // Initialize ADLX
    res = g_ADLXHelp.Initialize();

    if (ADLX_FAILED(res))
    {
        qWarning() << "\tg_ADLXHelp initialize failed";
    }
    else
    {
        m_initialized = true;
    }
    setFunctionStatus("ADLXHelper::Initialize", m_initialized, res);

	return true;
}

void AdlxHandler::checkSupportedDynamicFunctions()
{
    adlx_bool supported;
    ADLX_RESULT res;

    IADLXSystemMetricsSupportPtr systemMetricsSupport;
    res = m_perfMonitoringService->GetSupportedSystemMetrics(&systemMetricsSupport);
    setFunctionStatus("IADLXPerformanceMonitoringServices::GetSupportedSystemMetrics", systemMetricsSupport, res);

    if (!systemMetricsSupport || ADLX_FAILED(res))
    {
        return;
    }

    IADLXGPUMetricsSupportPtr gpuMetricsSupport;
    res = m_perfMonitoringService->GetSupportedGPUMetrics(m_oneGPU, &gpuMetricsSupport);
    setFunctionStatus("IADLXPerformanceMonitoringServices::GetSupportedGPUMetrics", systemMetricsSupport, res);

    if (!systemMetricsSupport || ADLX_FAILED(res))
    {
        return;
    }

    res = systemMetricsSupport->IsSupportedCPUUsage(&supported);
    setFunctionStatus("IADLXSystemMetricsSupport::CPUUsage", supported, res);

    res = systemMetricsSupport->IsSupportedSystemRAM(&supported);
    setFunctionStatus("IADLXSystemMetricsSupport::SystemRAM", supported, res);

    res = systemMetricsSupport->IsSupportedSmartShift(&supported);
    setFunctionStatus("IADLXSystemMetricsSupport::SmartShift", supported, res);

    res = gpuMetricsSupport->IsSupportedGPUUsage(&supported);
    setFunctionStatus("IADLXGPUMetricsSupport::GPUUsage", supported, res);

    res = gpuMetricsSupport->IsSupportedGPUClockSpeed(&supported);
    setFunctionStatus("IADLXGPUMetricsSupport::GPUClockSpeed", supported, res);

    res = gpuMetricsSupport->IsSupportedGPUVRAMClockSpeed(&supported);
    setFunctionStatus("IADLXGPUMetricsSupport::GPUVRAMClockSpeed", supported, res);

    res = gpuMetricsSupport->IsSupportedGPUTemperature(&supported);
    setFunctionStatus("IADLXGPUMetricsSupport::GPUTemperature", supported, res);

    res = gpuMetricsSupport->IsSupportedGPUHotspotTemperature(&supported);
    setFunctionStatus("IADLXGPUMetricsSupport::GPUHotspotTemperature", supported, res);

    res = gpuMetricsSupport->IsSupportedGPUPower(&supported);
    setFunctionStatus("IADLXGPUMetricsSupport::GPUPower", supported, res);

    res = gpuMetricsSupport->IsSupportedGPUFanSpeed(&supported);
    setFunctionStatus("IADLXGPUMetricsSupport::GPUFanSpeed", supported, res);

    res = gpuMetricsSupport->IsSupportedGPUVRAM(&supported);
    setFunctionStatus("IADLXGPUMetricsSupport::GPUVRAM", supported, res);

    res = gpuMetricsSupport->IsSupportedGPUVoltage(&supported);
    setFunctionStatus("IADLXGPUMetricsSupport::GPUVoltage", supported, res);

    res = gpuMetricsSupport->IsSupportedGPUTotalBoardPower(&supported);
    setFunctionStatus("IADLXGPUMetricsSupport::GPUTotalBoardPower", supported, res);

    res = gpuMetricsSupport->IsSupportedGPUIntakeTemperature(&supported);
    setFunctionStatus("IADLXGPUMetricsSupport::GPUIntakeTemperature", supported, res);
}

void AdlxHandler::setFunctionStatus(const char* key, const bool support, const ADLX_RESULT res) {
    ADLX_RESULT _res = res;
    if (!support)
    {
        _res = ADLX_NOT_SUPPORTED;
    }

    m_functionsSupportStatus[key] = support && ADLX_SUCCEEDED(res);
    m_functionsStatusMessage[key] = QString::fromStdString(AdlxResultMap.at(_res));
}

bool AdlxHandler::readStaticInfo()
{
    if (!m_initialized)
    {
        return false;
    }

    // Get Performance Monitoring services
    ADLX_RESULT res = g_ADLXHelp.GetSystemServices()->GetPerformanceMonitoringServices(&m_perfMonitoringService);
    if (ADLX_SUCCEEDED(res))
    {
        setFunctionStatus("IADLXSystemServices::GetPerformanceMonitoringServices", true, res);

        IADLXGPUListPtr gpus;
        res = g_ADLXHelp.GetSystemServices()->GetGPUs(&gpus);
        if (ADLX_SUCCEEDED(res))
        {
            setFunctionStatus("IADLXSystemServices::GetSystemServices", true, res);
            setFunctionStatus("IADLXSystem::GetGPUs", true, res);

            res = gpus->At(gpus->Begin(), &m_oneGPU);
            if (ADLX_SUCCEEDED(res))
            {
                ShowGPUInfo();
            }
            else
            {
                qWarning() << "\tGet particular GPU failed";
            }

        }
        else
        {
            setFunctionStatus("IADLXSystemServices::GetSystemServices", false, res);
            setFunctionStatus("IADLXSystem::GetGPUs", false, res);

            qWarning() << "\tGet GPU list failed";
        }
    }
    else
    {
        setFunctionStatus("IADLXSystemServices::GetPerformanceMonitoringServices", false, res);

        qWarning() << "\tGet performance monitoring services failed";
    }

    checkSupportedDynamicFunctions();

    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_ChipDesigner] = QString::fromStdString(m_gpuChipDesigner.c_str());
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_CardManufacturer] = QString::fromStdString(m_gpuCardManufacturer);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Model] = QString::fromStdString(m_gpuModel);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_MemorySize] = m_gpuMemorySize;
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_MemoryType] = QString::fromStdString(m_gpuMemoryType);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_PnpString] = QString::fromStdString(m_gpuPnpString);
    m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_StatusSupport_Adlx] = QVariant::fromValue(m_functionsSupportStatus);
    m_staticInfo[Globals::SysInfoAttr::Key_Api_Functions_ErrorMessage_Adlx] = QVariant::fromValue(m_functionsStatusMessage);

	return true;
}

bool AdlxHandler::readDynamicInfo()
{
    if (!m_initialized)
    {
        return false;
    }

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
void AdlxHandler::ShowGPUInfo()
{
    // Display GPU info
    const char* vendorId = nullptr;
    ADLX_RESULT ret = m_oneGPU->VendorId(&vendorId);
    setFunctionStatus("IADLXGPU::VendorId", ADLX_SUCCEEDED(ret), ret);

    ADLX_ASIC_FAMILY_TYPE asicFamilyType = ASIC_UNDEFINED;
    ret = m_oneGPU->ASICFamilyType(&asicFamilyType);
    setFunctionStatus("IADLXGPU::ASICFamilyType", ADLX_SUCCEEDED(ret), ret);

    ADLX_GPU_TYPE gpuType = GPUTYPE_UNDEFINED;
    ret = m_oneGPU->Type(&gpuType);
    setFunctionStatus("IADLXGPU::Type", ADLX_SUCCEEDED(ret), ret);

    adlx_bool isExternal = false;
    ret = m_oneGPU->IsExternal(&isExternal);
    setFunctionStatus("IADLXGPU::IsExternal", ADLX_SUCCEEDED(ret), ret);

    const char* gpuName = nullptr;
    ret = m_oneGPU->Name(&gpuName);
    setFunctionStatus("IADLXGPU::Name", ADLX_SUCCEEDED(ret), ret);

    const char* driverPath = nullptr;
    ret = m_oneGPU->DriverPath(&driverPath);
    setFunctionStatus("IADLXGPU::DriverPath", ADLX_SUCCEEDED(ret), ret);

    const char* pnpString = nullptr;
    ret = m_oneGPU->PNPString(&pnpString);
    setFunctionStatus("IADLXGPU::PNPString", ADLX_SUCCEEDED(ret), ret);

    adlx_bool hasDesktops = false;
    ret = m_oneGPU->HasDesktops(&hasDesktops);
    setFunctionStatus("IADLXGPU::HasDesktops", ADLX_SUCCEEDED(ret), ret);

    adlx_uint totalVRAM;
    ret = m_oneGPU->TotalVRAM(&totalVRAM);
    setFunctionStatus("IADLXGPU::TotalVRAM", ADLX_SUCCEEDED(ret), ret);

    const char* vramTypeString = nullptr;
    ret = m_oneGPU->VRAMType(&vramTypeString);
    setFunctionStatus("IADLXGPU::VRAMType", ADLX_SUCCEEDED(ret), ret);

    const char* partNumber = nullptr;
    const char* biosVersion = nullptr;
    const char* biosDate = nullptr;
    ret = m_oneGPU->BIOSInfo(&partNumber, &biosVersion, &biosDate);
    setFunctionStatus("IADLXGPU::BIOSInfo", ADLX_SUCCEEDED(ret), ret);

    const char* deviceId;
    ret = m_oneGPU->DeviceId(&deviceId);
    setFunctionStatus("IADLXGPU::DeviceId", ADLX_SUCCEEDED(ret), ret);

    adlx_int uid;
    ret = m_oneGPU->UniqueId(&uid);
    setFunctionStatus("IADLXGPU::UniqueId", ADLX_SUCCEEDED(ret), ret);

    const char* subSystemIdString = nullptr;
    ret = m_oneGPU->SubSystemId(&subSystemIdString);
    setFunctionStatus("IADLXGPU::SubSystemId", ADLX_SUCCEEDED(ret), ret);

    const char* subSystemVendorIdString = nullptr;
    ret = m_oneGPU->SubSystemVendorId(&subSystemVendorIdString);
    setFunctionStatus("IADLXGPU::SubSystemVendorId", ADLX_SUCCEEDED(ret), ret);

    const int subSystemVendorId = std::stoi(subSystemVendorIdString, 0, 16);

    const char* revisionId = nullptr;
    ret = m_oneGPU->RevisionId(&revisionId);
    setFunctionStatus("IADLXGPU::RevisionId", ADLX_SUCCEEDED(ret), ret);

    IADLXGPU1Ptr gpu1(m_oneGPU);
    if (gpu1)
    {
        const char* productName = nullptr;
        ret = gpu1->ProductName(&productName);
        setFunctionStatus("IADLXGPU1::ProductName", ADLX_SUCCEEDED(ret), ret);

        ADLX_MGPU_MODE mode = MGPU_NONE;
        ret = gpu1->MultiGPUMode(&mode);
        setFunctionStatus("IADLXGPU1::MultiGPUMode", ADLX_SUCCEEDED(ret), ret);

        ADLX_PCI_BUS_TYPE busType = UNDEFINED;
        ret = gpu1->PCIBusType(&busType);
        setFunctionStatus("IADLXGPU1::PCIBusType", ADLX_SUCCEEDED(ret), ret);

        adlx_uint laneWidth = 0;
        ret = gpu1->PCIBusLaneWidth(&laneWidth);
        setFunctionStatus("IADLXGPU1::PCIBusLaneWidth", ADLX_SUCCEEDED(ret), ret);
    }

    if (MapVendorIdName.find(subSystemVendorId) != MapVendorIdName.end())
    {
        m_gpuCardManufacturer = MapVendorIdName.at(subSystemVendorId);
    }

    m_gpuModel = gpuName;
    m_gpuMemorySize = totalVRAM;
    m_gpuMemoryType = vramTypeString;
    m_gpuPnpString = pnpString;
}

// Show current all metrics
void AdlxHandler::ShowCurrentAllMetrics(IADLXPerformanceMonitoringServicesPtr perfMonitoringServices, IADLXGPUPtr oneGPU)
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
void AdlxHandler::GetTimeStamp(IADLXSystemMetricsPtr systemMetrics)
{
    adlx_int64 timeStamp = 0;
    ADLX_RESULT res = systemMetrics->TimeStamp(&timeStamp);
    //if (ADLX_SUCCEEDED(res))
        //qDebug() << "The system time stamp is: " << timeStamp << "ms";
}

// Show CPU usage(in %)
void AdlxHandler::ShowCPUUsage(IADLXSystemMetricsSupportPtr systemMetricsSupport, IADLXSystemMetricsPtr systemMetrics)
{
    // Display CPU usage support status
    if (m_functionsSupportStatus["IADLXSystemMetricsSupport::CPUUsage"])
    {
        adlx_double cpuUsage = 0;
        ADLX_RESULT res = systemMetrics->CPUUsage(&cpuUsage);
        //if (ADLX_SUCCEEDED(res))
            //qDebug() << "The CPU usage is: " << cpuUsage << "%";
    }
}

// Display system RAM (in MB)
void AdlxHandler::ShowSystemRAM(IADLXSystemMetricsSupportPtr systemMetricsSupport, IADLXSystemMetricsPtr systemMetrics)
{
    // Display system RAM usage support status
    if (m_functionsSupportStatus["IADLXSystemMetricsSupport::SystemRAM"])
    {
        adlx_int systemRAM = 0;
        ADLX_RESULT res = systemMetrics->SystemRAM(&systemRAM);
        //if (ADLX_SUCCEEDED(res))
            //qDebug() << "The system RAM is: " << systemRAM << "MB";
    }
}

// Display SmartShift
void AdlxHandler::ShowSmartShift(IADLXSystemMetricsSupportPtr systemMetricsSupport, IADLXSystemMetricsPtr systemMetrics)
{
    // Display SmartShift support status
    if (m_functionsSupportStatus["IADLXSystemMetricsSupport::SmartShift"])
    {
        adlx_int smartShift;
        ADLX_RESULT res = systemMetrics->SmartShift(&smartShift);
        //if (ADLX_SUCCEEDED(res))
            //qDebug() << "The SmartShift is: " << smartShift;
    }
}

// Display the GPU metrics time stamp (in ms)
void AdlxHandler::GetTimeStamp(IADLXGPUMetricsPtr gpuMetrics)
{
    adlx_int64 timeStamp = 0;
    ADLX_RESULT res = gpuMetrics->TimeStamp(&timeStamp);
    //if (ADLX_SUCCEEDED(res))
        //qDebug() << "The GPU time stamp is: " << timeStamp << "ms";
}

// Display GPU usage (in %)
void AdlxHandler::ShowGPUUsage(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    // Display GPU usage support status
    if (m_functionsSupportStatus["IADLXGPUMetricsSupport::GPUUsage"])
    {
        adlx_double usage = 0;
        ADLX_RESULT res = gpuMetrics->GPUUsage(&usage);
        if (ADLX_SUCCEEDED(res))
        {
            m_gpuUsage = usage;
        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }
    }
}

// Display GPU clock speed (in MHz)
void AdlxHandler::ShowGPUClockSpeed(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    // Display GPU clock speed support status
    if (m_functionsSupportStatus["IADLXGPUMetricsSupport::GPUClockSpeed"])
    {
        adlx_int gpuClock = 0;
        ADLX_RESULT res = gpuMetrics->GPUClockSpeed(&gpuClock);
        if (ADLX_SUCCEEDED(res))
        {
            m_gpuClockSpeed = gpuClock;
        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }

        //adlx_int minValue = 0, maxValue = 0;
        //res = gpuMetricsSupport->GetGPUClockSpeedRange(&minValue, &maxValue);
        //if (ADLX_SUCCEEDED(res))
        //{
        //}
    }
}

// Show GPU VRAM clock speed(MHz)
void AdlxHandler::ShowGPUVRAMClockSpeed(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    // Get if the GPU VRAM clock speed is supported
    if (m_functionsSupportStatus["IADLXGPUMetricsSupport::GPUVRAMClockSpeed"])
    {
        adlx_int memoryClock = 0;
        ADLX_RESULT res = gpuMetrics->GPUVRAMClockSpeed(&memoryClock);
        if (ADLX_SUCCEEDED(res))
        {
            m_gpuVramClockSpeed = memoryClock;
        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }

        //adlx_int minValue = 0, maxValue = 0;
        //res = gpuMetricsSupport->GetGPUVRAMRange(&minValue, &maxValue);
        //if (ADLX_SUCCEEDED(res))
        //{
        //}
    }
}

// Show GPU temperature(�C)
void AdlxHandler::ShowGPUTemperature(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    // Get if the GPU tempetature is supported
    if (m_functionsSupportStatus["IADLXGPUMetricsSupport::GPUTemperature"])
    {
        adlx_double temperature = 0;
        ADLX_RESULT res = gpuMetrics->GPUTemperature(&temperature);
        if (ADLX_SUCCEEDED(res))
        {
            m_gpuTemperature = temperature;
        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }
    }
}

// Show GPU hotspot temperature(�C)
void AdlxHandler::ShowGPUHotspotTemperature(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    // Get if the GPU hotspot temperature is supported
    if (m_functionsSupportStatus["IADLXGPUMetricsSupport::GPUHotspotTemperature"])
    {
        adlx_double hotspotTemperature = 0;
        ADLX_RESULT res = gpuMetrics->GPUHotspotTemperature(&hotspotTemperature);
        if (ADLX_SUCCEEDED(res))
        {
            m_gpuHotspotTemperature = hotspotTemperature;
        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }
    }
}

// Show GPU power(W)
void AdlxHandler::ShowGPUPower(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    // Get if the GPU power is supported
    if (m_functionsSupportStatus["IADLXGPUMetricsSupport::GPUPower"])
    {
        adlx_double power = 0;
        ADLX_RESULT res = gpuMetrics->GPUPower(&power);
        if (ADLX_SUCCEEDED(res))
        {
            m_gpuPower = power;
        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }
    }
}

// Show GPU fan speed(RPM)
void AdlxHandler::ShowGPUFanSpeed(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    // Get if the GPU fan speed is supported
    if (m_functionsSupportStatus["IADLXGPUMetricsSupport::GPUFanSpeed"])
    {
        adlx_int fanSpeed = 0;
        ADLX_RESULT res = gpuMetrics->GPUFanSpeed(&fanSpeed);
        if (ADLX_SUCCEEDED(res))
        {
            m_gpuFanSpeed = fanSpeed;
        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }

        adlx_int minValue = 0, maxValue = 0;
        res = gpuMetricsSupport->GetGPUFanSpeedRange(&minValue, &maxValue);
        if (ADLX_SUCCEEDED(res))
        {
            m_gpuFanSpeedUsage = 100.0 * double(fanSpeed) / maxValue;
        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }
    }
}

// Show GPU VRAM(MB)
void AdlxHandler::ShowGPUVRAM(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    // Get if the GPU VRAM is supported
    if (m_functionsSupportStatus["IADLXGPUMetricsSupport::GPUVRAM"])
    {
        adlx_int VRAM = 0;
        ADLX_RESULT res = gpuMetrics->GPUVRAM(&VRAM);
        if (ADLX_SUCCEEDED(res))
        {
            m_gpuVramUsed = VRAM;
            m_gpuVramUsage = (static_cast<double>(m_gpuVramUsed) / m_gpuMemorySize) * 100.0;
        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }
    }
}

// Show GPU Voltage(mV)
void AdlxHandler::ShowGPUVoltage(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    // Get if the GPU voltage is supported
    if (m_functionsSupportStatus["IADLXGPUMetricsSupport::GPUVoltage"])
    {
        adlx_int voltage = 0;
        ADLX_RESULT res = gpuMetrics->GPUVoltage(&voltage);
        if (ADLX_SUCCEEDED(res))
        {
            m_gpuVoltage = voltage;
        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }
    }
}

// Show GPU Total Board Power(W)
void AdlxHandler::ShowGPUTotalBoardPower(IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    // Get if the GPU voltage is supported
    if (m_functionsSupportStatus["IADLXGPUMetricsSupport::GPUTotalBoardPower"])
    {
        adlx_double power = 0;
        ADLX_RESULT res = gpuMetrics->GPUTotalBoardPower(&power);
        if (ADLX_SUCCEEDED(res))
        {
            m_gpuTotalBoardPower = power;
        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }
    }
}

// Display GPU intake temperature(in °C)
void AdlxHandler::ShowGPUIntakeTemperature (IADLXGPUMetricsSupportPtr gpuMetricsSupport, IADLXGPUMetricsPtr gpuMetrics)
{
    // Display the GPU temperature support status
    if (m_functionsSupportStatus["IADLXGPUMetricsSupport::GPUIntakeTemperature"])
    {
        adlx_double temperature = 0;
        ADLX_RESULT res = gpuMetrics->GPUIntakeTemperature(&temperature);
        if (ADLX_SUCCEEDED(res))
        {

        }
        else
        {
            qDebug() << __FUNCTION__ << " failed! reason: " << AdlxResultMap.at(res).c_str();
        }
    }
}