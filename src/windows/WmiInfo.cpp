#include "WmiInfo.h"

#include "../Globals.h"

#include <comdef.h>
#include <comutil.h>
#include <iostream>
#pragma comment(lib, "wbemuuid.lib")

#include <qDebug>

#include <D3d12.h>
#include <D3d9.h>
//#include <Wbemidl.h>

WmiInfo::WmiInfo() : BaseInfo("WmiInfo", InfoType::Wmi)
{
    qDebug() << __FUNCTION__;

#ifdef HAS_RYZEN_MASTER_SDK
    m_readCpuParameters = false;
#endif
}

WmiInfo::~WmiInfo()
{
    qDebug() << __FUNCTION__;
}

//@note: check available wmi classes with WMI Explorer (https://github.com/vinaypamnani/wmie2)
//normally network source is "ROOT\\CIMV2" or "ROOT\\WMI"
//some classes might not be available like "Win32_FanSpeed"
//also using wmi + query is very slow (measured ~250ms per call)

void WmiInfo::init()
{
    HRESULT hr;
    //hr = CoInitialize(NULL);
    //if (!SUCCEEDED(hr)) {
    //    qWarning() << "CoInitialize with COINIT_MULTITHREADED failed!";
    //}
    hr = CoInitializeEx(0, COINIT_MULTITHREADED); //COINIT_APARTMENTTHREADED
    if (!SUCCEEDED(hr)) {
        qWarning() << "CoInitializeEx failed! reason: " << std::system_category().message(hr).c_str();
    }
    hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    if (!SUCCEEDED(hr)) {
        qWarning() << "CoInitializeSecurity failed! reason: " << std::system_category().message(hr).c_str();
    }
    hr = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&m_locator);
    if (!SUCCEEDED(hr)) {
        qWarning() << "CoCreateInstance failed! reason: " << std::system_category().message(hr).c_str();
    }
    hr = m_locator->ConnectServer(_bstr_t("ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &m_service);//_bstr_t("ROOT\\CIMV2") //;_bstr_t("ROOT\\WMI")
    if (!SUCCEEDED(hr)) {
        qWarning() << "ConnectServer failed! reason: " << std::system_category().message(hr).c_str();
    }
    hr = CoSetProxyBlanket(m_service, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (!SUCCEEDED(hr)) {
        qWarning() << "CoSetProxyBlanket failed! reason: " << std::system_category().message(hr).c_str();
    }

    readStaticInfo();
}

void WmiInfo::checkSupportedFunctions()
{
    std::vector<std::string> queryResult;
    queryResult = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation", L"Name");
    m_functionsSupportStatus["Win32_PerfFormattedData_Counters_ProcessorInformation"] = !queryResult.empty();

    queryResult = query(L"Win32_Fan", L"Name");
    m_functionsSupportStatus["Win32_Fan"] = !queryResult.empty();

    queryResult = query(L"Win32_Processor", L"Name");
    m_functionsSupportStatus["Win32_Processor"] = !queryResult.empty();

    queryResult = query(L"Win32_TemperatureProbe", L"Name");
    m_functionsSupportStatus["Win32_TemperatureProbe"] = !queryResult.empty();

    queryResult = query(L"Win32_VideoController", L"Name");
    m_functionsSupportStatus["Win32_VideoController"] = !queryResult.empty();

    queryResult = query(L"Win32_PerfFormattedData_Tcpip_NetworkInterface", L"Name");
    m_functionsSupportStatus["Win32_PerfFormattedData_Tcpip_NetworkInterface"] = !queryResult.empty();

    queryResult = query(L"CIM_PowerSupply", L"Name");
    m_functionsSupportStatus["CIM_PowerSupply"] = !queryResult.empty();
   
    queryResult = query(L"MSAcpi_ThermalZoneTemperature", L"CurrentTemperature");
    m_functionsSupportStatus["MSAcpi_ThermalZoneTemperature"] = !queryResult.empty();
}

void WmiInfo::readStaticInfo()
{
    checkSupportedFunctions();

    readCpuInfo();
    readGpuInfo();

    setStaticValue(Globals::SysInfoAttr::Key_Cpu_Static_Brand, QString::fromStdString(m_cpuBrand));
    setStaticValue(Globals::SysInfoAttr::Key_Cpu_Static_CoreCount, m_cpuCoreCount);
    setStaticValue(Globals::SysInfoAttr::Key_Cpu_Static_ThreadCount, m_cpuThreadCount);
    setStaticValue(Globals::SysInfoAttr::Key_Cpu_Static_BaseFrequency, m_cpuBaseFrequency);
    setStaticValue(Globals::SysInfoAttr::Key_Cpu_Static_MaxTurboFrequency, m_cpuMaxTurboFrequency);
    //setStaticValue(Globals::SysInfoAttr::Key_Cpu_L1CacheSize, m_cpuL1CacheSize);
    //setStaticValue(Globals::SysInfoAttr::Key_Cpu_L2CacheSize, m_cpuL2CacheSize);
    //setStaticValue(Globals::SysInfoAttr::Key_Cpu_L3CacheSize, m_cpuL3CacheSize);
    setStaticValue(Globals::SysInfoAttr::Key_Gpu_Static_PnpString, QString::fromStdString(m_gpuPnpString));
    setStaticValue(Globals::SysInfoAttr::Key_Api_Functions_StatusSupport_Wmi, QVariant::fromValue(m_functionsSupportStatus));
}

void WmiInfo::update()
{
    //if (m_readCpuParameters)
    {
        readGpuUsage();
        readCpuFrequency();
        readTemperature();
        readFanSpeed();
    }

    readNetworkSpeed();

    //readPowerSupply();

    setDynamicValue(Globals::SysInfoAttr::Key_Cpu_Dynamic_CurrentMaxFrequency, m_cpuCurrentMaxFrequency);
    setDynamicValue(Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadFrequencies, QVariant::fromValue(m_cpuThreadFrequencies));
    setDynamicValue(Globals::SysInfoAttr::Key_Cpu_Dynamic_ThreadUsages, QVariant::fromValue(m_cpuThreadUsages));

    setDynamicValue(Globals::SysInfoAttr::Key_Network_Dynamic_Info, QVariant::fromValue(m_networkMap));

    setDynamicValue(Globals::SysInfoAttr::Key_Process_GpuUsages, QVariant::fromValue(m_processGpuUsage));
}

void WmiInfo::readGpuUsage()
{
    m_gpuUsage = 0.0f;

    //auto gpuAdapterMemory = queryArray(L"Win32_PerfFormattedData_GPUPerformanceCounters_GPUAdapterMemory ", { L"Name",L"DedicatedUsage"});

    auto gpuProcessMemory = queryArray(L"Win32_PerfFormattedData_GPUPerformanceCounters_GPUProcessMemory", { L"Name",L"DedicatedUsage"});
    if (!gpuProcessMemory.empty() && !gpuProcessMemory["DedicatedUsage"].empty())
    {
        const std::string suffix = "pid_";
        const std::string prefix = "_luid";

        for (uint8_t i = 0; i < gpuProcessMemory["DedicatedUsage"].size(); ++i)
        {
            std::string pidStr = gpuProcessMemory["Name"][i];
            const uint32_t startIdx = pidStr.find(suffix) + suffix.length();
            const uint32_t count = pidStr.find(prefix) - startIdx;
            pidStr = pidStr.substr(startIdx, count);
            const uint32_t pid = std::stoi(pidStr);
            const uint64_t dedicatedUsage = std::stoul(gpuProcessMemory["DedicatedUsage"][i]);
            m_processGpuUsage[pid].first = 0;
            m_processGpuUsage[pid].second = dedicatedUsage;
        }
    }

    auto gpuUsage = queryArray(L"Win32_PerfFormattedData_GPUPerformanceCounters_GPUEngine", { L"Name",L"UtilizationPercentage" }, L"(name LIKE '%engtype_3D' OR name LIKE '%engtype_Graphics%') AND UtilizationPercentage <> 0");
    if (!gpuUsage.empty() &&  !gpuUsage["UtilizationPercentage"].empty())
    {
        const std::string suffix = "pid_";
        const std::string prefix = "_luid";

        for (uint8_t i = 0; i < gpuUsage["UtilizationPercentage"].size(); ++i)
        {
            std::string pidStr = gpuUsage["Name"][i];
            const uint32_t startIdx = pidStr.find(suffix) + suffix.length();
            const uint32_t count = pidStr.find(prefix) - startIdx;
            pidStr = pidStr.substr(startIdx, count);
            const uint32_t pid = std::stoi(pidStr);
            const uint8_t utilizationPercentage = std::stoi(gpuUsage["UtilizationPercentage"][i]);
            if (m_processGpuUsage.find(pid) != m_processGpuUsage.end())
            {
                m_gpuUsage += utilizationPercentage;
                m_processGpuUsage[pid].first = utilizationPercentage;
            }
        }
    }
    //qDebug() << __FUNCTION__ << " Gpu Total Usage: " << m_gpuUsage;
}
/*
* read CPU frequency.
*/
//@note: CallNtPowerInformation does not give current frequency anymore since Windows 10 21H1 (19043)
void WmiInfo::readCpuFrequency()
{
    if (!m_functionsSupportStatus["Win32_PerfFormattedData_Counters_ProcessorInformation"])
    {
        return;
    }
    //auto percentProcessorPerformanceTotal = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation",
    //    L"PercentProcessorPerformance", L"Name = '_Total'");
    //if (percentProcessorPerformanceTotal.empty()) {
    //    return;
    //}

    //auto percentProcessorUtility = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation",
    //    L"PercentProcessorUtility");
    //if (!percentProcessorUtility.empty()) {
    //}

    //auto percentProcessorUtilityTotal = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation",
    //    L"PercentProcessorUtility", L"Name = '_Total'");
    //if (!percentProcessorUtilityTotal.empty()) {
    //}

    const std::vector<std::wstring> fields = { L"PercentProcessorPerformance",L"PercentProcessorUtility"};
    std::map<std::string, std::vector<std::string>> fieldMap = queryArray(L"Win32_PerfFormattedData_Counters_ProcessorInformation", fields, L"NOT Name LIKE '%_Total\'", 16);

    if (fieldMap["PercentProcessorPerformance"].empty() || fieldMap["PercentProcessorUtility"].empty()) {
        return;
    }

    uint16_t currentMaxFrequency = 0;

    for (size_t i = 0; i < m_cpuThreadCount; i++)
    {
        const double performance = std::stod(fieldMap["PercentProcessorPerformance"][i]) / 100;
        double currentFrequency = m_cpuBaseFrequency * performance;
        m_cpuThreadFrequencies[i] = currentFrequency;
        if (currentFrequency > currentMaxFrequency)
        {
            currentMaxFrequency = currentFrequency;
        }

        const double usage = std::stod(fieldMap["PercentProcessorUtility"][i]);// *100;
        m_cpuThreadUsages[i] = usage;
    }

    m_cpuCurrentMaxFrequency = currentMaxFrequency;
}

void WmiInfo::readCpuInfo()
{
    if (!m_functionsSupportStatus["Win32_Processor"])
    {
        return;
    }

    std::vector<std::wstring> fields = { L"Name",L"Manufacturer",L"NumberOfCores",L"NumberOfLogicalProcessors",L"MaxClockSpeed" };
    std::map<std::string, std::vector<std::string>> fieldMap = queryArray(L"Win32_Processor", fields);

    if (!fieldMap["Name"].empty()) {
        m_cpuBrand = fieldMap["Name"][0];
    }

    if (!fieldMap["Manufacturer"].empty()) {
        const std::string manuFacturer = fieldMap["Manufacturer"][0];
    }

    if (!fieldMap["NumberOfCores"].empty()) {
        m_cpuCoreCount = std::stoi(fieldMap["NumberOfCores"][0]);
    }

    if (!fieldMap["NumberOfLogicalProcessors"].empty()) {
        m_cpuThreadCount = std::stoi(fieldMap["NumberOfLogicalProcessors"][0]);
        m_cpuThreadFrequencies.resize(m_cpuThreadCount);
        m_cpuThreadUsages.resize(m_cpuThreadCount);
    }

    if (!fieldMap["MaxClockSpeed"].empty()) {
        m_cpuBaseFrequency = std::stoi(fieldMap["MaxClockSpeed"][0]);
        m_cpuMaxTurboFrequency = m_cpuBaseFrequency; //currently there is no way to get the max turbo frequency in windows
    }
}

void WmiInfo::readFanSpeed()
{
    if (!m_functionsSupportStatus["Win32_Fan"])
    {
        return;
    }

    const std::vector<std::wstring> fields = { L"Availability",L"ActiveCooling",L"DesiredSpeed",L"ConfigManagerErrorCode"};
    std::map<std::string, std::vector<std::string>> fieldMap = queryArray(L"Win32_Fan", fields);

    if (!fieldMap["Availability"].empty()) {

    }

    if (!fieldMap["ActiveCooling"].empty()) {

    }

    if (!fieldMap["DesiredSpeed"].empty()) {

    }

    if (!fieldMap["ConfigManagerErrorCode"].empty()) {

    }
}

//@note: tested, not supported
void WmiInfo::readTemperature()
{
    if (!m_functionsSupportStatus["Win32_TemperatureProbe"])
    {
        return;
    }

    const std::vector<std::wstring> fields = { L"Name",L"CurrentReading"};
    std::map<std::string, std::vector<std::string>> fieldMap = queryArray(L"Win32_TemperatureProbe", fields);

    if (!fieldMap["InstanceName"].empty()) {

    }
    if (!fieldMap["CurrentTemperature"].empty()) {
        //const double temperature = std::stod(fieldMap["CurrentTemperature"]);
        //m_cpuTemperature = temperature;
    }
    //BSTR query = SysAllocString(L"SELECT * FROM MSAcpi_ThermalZoneTemperature");
    //auto thermalZoneTemperature = query(L"MSAcpi_ThermalZoneTemperature", L"InstanceName,CurrentTemperature");
    //auto thermalZoneTemperature = query(L"MSAcpi_ThermalZoneTemperature", L"*");
}

void WmiInfo::readGpuInfo()
{
    if (!m_functionsSupportStatus["Win32_VideoController"])
    {
        return;
    }

    //Win32_PerfFormattedData_GPUPerformanceCounters_GPUAdapterMemory
    //    Caption,
    //    Description,
    //    Name,
    //    Frequency_Object,
    //    Frequency_PerfTime,
    //    Frequency_Sys100NS,
    //    Timestamp_Object,
    //    Timestamp_PerfTime,
    //    Timestamp_Sys100NS,
    //    DedicatedUsage,
    //    SharedUsage,
    //    TotalCommitted
    //}

    //Win32_PerfFormattedData_GPUPerformanceCounters_GPUEngine
    //    Caption,
    //    Description,
    //    Name,
    //    Frequency_Object,
    //    Frequency_PerfTime,
    //    Frequency_Sys100NS,
    //    Timestamp_Object,
    //    Timestamp_PerfTime,
    //    Timestamp_Sys100NS,
    //    RunningTime,
    //    UtilizationPercentage
    //}

    //Win32_PerfFormattedData_GPUPerformanceCounters_GPUProcessMemory
    //Caption,
    //Description,
    //Name,
    //Frequency_Object,
    //Frequency_PerfTime,
    //Frequency_Sys100NS,
    //Timestamp_Object,
    //Timestamp_PerfTime,
    //Timestamp_Sys100NS,
    //DedicatedUsage,
    //LocalUsage,
    //NonLocalUsage,
    //SharedUsage,
    //TotalCommitted

    //Win32_PerfFormattedData_PerfOS_Processor
    //C1TransitionsPersec
    //C2TransitionsPersec
    //C3TransitionsPersec
    //DPCRate
    //DPCsQueuedPersec
    //InterruptsPersec
    //PercentC1Time
    //PercentC2Time
    //PercentC3Time
    //PercentDPCTime
    //PercentIdleTime
    //PercentInterruptTime
    //PercentPrivilegedTime
    //PercentProcessorTime
    //PercentUserTime

    //auto gpuMemoryUsage = query(L"Win32_PerfFormattedData_PerfOS_Processor", L"PercentProcessorTime");

    //auto gpuUsage = queryArray(L"Win32_PerfFormattedData_GPUPerformanceCounters_GPUEngine", { L"Name",L"UtilizationPercentage" }, L"(name LIKE '%engtype_3D' OR name LIKE '%engtype_Graphics%') AND UtilizationPercentage <> 0");
    //auto gpuUsageAll = queryArray(L"Win32_PerfFormattedData_GPUPerformanceCounters_GPUEngine", { L"Name",L"UtilizationPercentage" }, L"(name LIKE '%engtype_3D' OR name LIKE '%engtype_Graphics%')");

    //auto gpuUsageArray = queryArray(L"Win32_PerfFormattedData_GPUPerformanceCounters_GPUEngine", { L"Name",L"UtilizationPercentage" });

    //std::vector<std::wstring> gpuMemorySharedUsageFields = { L"Name",L"DedicatedUsage",L"SharedUsage" };

    //auto gpuMemoryDedicatedUsage = queryArray(L"Win32_PerfFormattedData_GPUPerformanceCounters_GPUProcessMemory", L"DedicatedUsage");
    //auto gpuMemorySharedUsage = queryArray(L"Win32_PerfFormattedData_GPUPerformanceCounters_GPUProcessMemory", { L"Name",L"DedicatedUsage",L"SharedUsage" });
    //auto queryResult = query(L"Win32_VideoController", L"*");

    const std::vector<std::wstring> fields = { L"AdapterRAM",L"Availability",L"Caption",L"CreationClassName",L"CurrentRefreshRate",L"DeviceID",L"DriverDate",
        L"DriverVersion",L"MaxRefreshRate",L"MinRefreshRate",L"PNPDeviceID",L"PowerManagementSupported",L"Status",L"StatusInfo",L"SystemCreationClassName",L"SystemName",L"VideoMemoryType"};
    std::map<std::string, std::vector<std::string>> fieldMap = queryArray(L"Win32_VideoController", fields);

    uint8_t runningGpuIdx = -1;

    if (!fieldMap["Availability"].empty()) {
        for (uint8_t i = 0; i < fieldMap["Availability"].size(); ++i)
        {
            //3==running/fullpower, 8==offline...
            if (fieldMap["Availability"].at(i) == "3")
            {
                runningGpuIdx = i;
                break;
            }
        }
    }

    if (runningGpuIdx == -1)
    {
        return;
    }

    if (!fieldMap["AdapterRAM"].empty()) {
        m_gpuMemorySize = std::stoul(fieldMap["AdapterRAM"][runningGpuIdx])/1000000;
    }

    if (!fieldMap["Caption"].empty()) {
        m_gpuModel = fieldMap["Caption"][runningGpuIdx];
    }

    if (!fieldMap["DriverDate"].empty()) {
        m_gpuDriverDate = fieldMap["DriverDate"][runningGpuIdx];
    }

    if (!fieldMap["DriverVersion"].empty()) {
        m_gpuDriverVersion = fieldMap["DriverVersion"][runningGpuIdx];
    }

    if (!fieldMap["PNPDeviceID"].empty()) {
        m_gpuPnpString = fieldMap["PNPDeviceID"][runningGpuIdx];
    }
}

void WmiInfo::readNetworkSpeed()
{
    std::vector<std::string> networkNames;
    std::vector<uint32_t> networkBytesReceivedPerSec;
    std::vector<uint32_t> networkBytesSentPerSec;
    std::vector<uint32_t> networkBytesTotalPerSec;
    std::vector<uint32_t> networkCurrentBandwidth;

    if (!m_functionsSupportStatus["Win32_PerfFormattedData_Tcpip_NetworkInterface"])
    {
        return;
    }

    const std::vector<std::wstring> fields = { L"Name",L"BytesReceivedPerSec",L"BytesSentPerSec",L"BytesTotalPerSec",L"CurrentBandwidth" };
    std::map<std::string, std::vector<std::string>> fieldMap = queryArray(L"Win32_PerfFormattedData_Tcpip_NetworkInterface", fields);

    if (!fieldMap["Name"].empty()) {
        for (auto&& netWorkInterfaceName : fieldMap["Name"])
        {
            networkNames.push_back(netWorkInterfaceName);
        }
    }

    if (!fieldMap["BytesReceivedPerSec"].empty()) {
        for (auto&& bytesReceivedPerSec : fieldMap["BytesReceivedPerSec"])
        {
            networkBytesReceivedPerSec.push_back(std::stoi(bytesReceivedPerSec));
        }
    }

    if (!fieldMap["BytesSentPerSec"].empty()) {
        for (auto&& bytesSentPerSec : fieldMap["BytesSentPerSec"])
        {
            networkBytesSentPerSec.push_back(std::stoi(bytesSentPerSec));
        }
    }

    if (!fieldMap["BytesTotalPerSec"].empty()) {
        for (auto&& bytesTotalPerSec : fieldMap["BytesTotalPerSec"])
        {
            networkBytesTotalPerSec.push_back(std::stoi(bytesTotalPerSec));
        }
    }

    if (!fieldMap["CurrentBandwidth"].empty()) {
        for (auto&& currentBandwidth : fieldMap["CurrentBandwidth"])
        {
            networkCurrentBandwidth.push_back(std::stoi(currentBandwidth));
        }
    }

    for (uint8_t i = 0; i < networkNames.size(); ++i)
    {
        NetworkInfo::Network network;
        network.name = networkNames.at(i);
        network.bytesReceivedPerSec = networkBytesReceivedPerSec.at(i);
        network.bytesSentPerSec = networkBytesSentPerSec.at(i);
        network.bytesTotalPerSec = networkBytesTotalPerSec.at(i);

        m_networkMap[networkNames.at(i)] = network;
    }
}

//@note: needs ConnectServer(_bstr_t("ROOT\\CIMV2\\power")
//currently unused
void WmiInfo::readPowerSupply()
{
    if (!m_functionsSupportStatus["CIM_PowerSupply"])
    {
        return;
    }

    const std::vector<std::wstring> fields = { L"Name",L"ActiveInputVoltage",L"Status",L"StatusInfo",L"TotalOutputPower" };
    std::map<std::string, std::vector<std::string>> fieldMap = queryArray(L"CIM_PowerSupply", fields);
}


bool WmiInfo::executeQuery(const std::wstring& query) {
    if (m_service == nullptr) return false;
    return SUCCEEDED(m_service->ExecQuery(bstr_t(L"WQL"), bstr_t(std::wstring(query.begin(), query.end()).c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &m_enumerator));
}

std::vector<std::string> WmiInfo::query(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter, const ULONG count) 
{
    std::wstring filter_string;
    if (!filter.empty()) {
        filter_string.append(L" WHERE " + filter);
    }
    std::wstring query_string(L"SELECT " + field + L" FROM " + wmi_class + filter_string);
    bool success = executeQuery(query_string);
    if (!success) {
        return {};
    }

    std::vector<std::string> result;

    if (count == 1)
    {
        ULONG u_return = 0;
        IWbemClassObject* obj = nullptr;
        long timeout = WBEM_INFINITE;
        while (m_enumerator) {
            m_enumerator->Next(timeout, count, &obj, &u_return);

            if (!u_return || !obj) {
                break;
            }
            VARIANT vt_prop;
            VariantInit(&vt_prop);
            CIMTYPE pType;
            long plFlavor;

            HRESULT hr = obj->Get(field.c_str(), 0, &vt_prop, &pType, &plFlavor);
            if (SUCCEEDED(hr)) {
                if (vt_prop.vt == VT_BSTR)
                {
                    char* text = _com_util::ConvertBSTRToString(vt_prop.bstrVal);
                    result.push_back(text);
                }
                else if (vt_prop.vt == VT_I4)
                {
                    std::string str = std::to_string(vt_prop.uintVal);
                    result.push_back(str);
                }
                else if (vt_prop.vt == VT_UI8)
                {
                    std::string str = std::to_string(vt_prop.ullVal);
                    result.push_back(str);
                }
                else if (vt_prop.vt == VT_BOOL)
                {
                    std::string str = std::to_string(vt_prop.boolVal);
                    result.push_back(str);
                }
                else if (vt_prop.vt == VT_NULL)
                {
                    qWarning() << __FUNCTION__ << wmi_class << "::" << field << " : " << " Unhandled Type VT_NULL";
                }
                else
                {
                    qWarning() << __FUNCTION__ << wmi_class << "::" << field << " : " << " Unhandled Type " << vt_prop.vt;
                }
            }
            VariantClear(&vt_prop);
            obj->Release();
        }
    }
    else
    {
        HRESULT    hRes = WBEM_S_NO_ERROR;
        // Final Next will return WBEM_S_FALSE
        ULONG            uReturned;
        IWbemClassObject* obj[256];

        hRes = m_enumerator->Next(WBEM_INFINITE, count, obj, &uReturned);

        if (SUCCEEDED(hRes))
        {
            for (ULONG n = 0; n < uReturned; n++)
            {
                VARIANT vt_prop;
                VariantInit(&vt_prop);
                CIMTYPE pType;
                long plFlavor;

                HRESULT hr = obj[n]->Get(field.c_str(), 0, &vt_prop, &pType, &plFlavor);

                if (SUCCEEDED(hr)) {
                    if (vt_prop.vt == VT_BSTR)
                    {
                        char* text = _com_util::ConvertBSTRToString(vt_prop.bstrVal);
                        result.push_back(text);
                    }
                    else if (vt_prop.vt == VT_I4)
                    {
                        std::string str = std::to_string(vt_prop.uintVal);
                        result.push_back(str);
                    }
                    else if (vt_prop.vt == VT_UI8)
                    {
                        std::string str = std::to_string(vt_prop.ullVal);
                        result.push_back(str);
                    }
                    else if (vt_prop.vt == VT_BOOL)
                    {
                        std::string str = std::to_string(vt_prop.boolVal);
                        result.push_back(str);
                    }
                    else if (vt_prop.vt == VT_NULL)
                    {
                        qWarning() << __FUNCTION__ << wmi_class << "::" << field << " : " << " Unhandled Type VT_NULL";
                    }
                    else
                    {
                        qWarning() << __FUNCTION__ << wmi_class << "::" << field << " : " << " Unhandled Type " << vt_prop.vt;
                    }
                }
                VariantClear(&vt_prop);
                obj[n]->Release();
            }
        }
    }

    return result;
}


std::map<std::string, std::vector<std::string>> WmiInfo::queryArray(const std::wstring& wmi_class, const std::vector<std::wstring>& fields, const std::wstring& filter, const ULONG count)
{
    std::map<std::string, std::vector<std::string>> fieldMap;

    std::wstring filter_string;
    if (!filter.empty()) {
        filter_string.append(L" WHERE " + filter);
    }
    std::wstring fieldsMerged;
    for (auto&& element : fields)
    {
        fieldsMerged += element;
        if (element != fields.back())
        {
            fieldsMerged += L" ,";
        }
    }
    std::wstring query_string(L"SELECT " + fieldsMerged + L" FROM " + wmi_class + filter_string);
    bool success = executeQuery(query_string);
    if (!success) {
        return {};
    }

    if (count == 1)
    {
        ULONG u_return = 0;
        IWbemClassObject* obj = nullptr;
        long timeout = WBEM_INFINITE;
        while (m_enumerator) {
            m_enumerator->Next(timeout, 1, &obj, &u_return);

            if (!u_return || !obj) {
                break;
            }

            for (auto&& field : fields)
            {
                VARIANT vt_prop;
                VariantInit(&vt_prop);
                CIMTYPE pType;
                long plFlavor;

                HRESULT hr = obj->Get(field.c_str(), 0, &vt_prop, &pType, &plFlavor);

                if (SUCCEEDED(hr)) {
                    std::string text;
                    if (vt_prop.vt == VT_BSTR)
                    {
                        text = _com_util::ConvertBSTRToString(vt_prop.bstrVal);
                    }
                    else if (vt_prop.vt == VT_I4)
                    {
                        text = std::to_string(vt_prop.uintVal);
                    }
                    else if (vt_prop.vt == VT_UI8)
                    {
                        text = std::to_string(vt_prop.ullVal);
                    }
                    else if (vt_prop.vt == VT_BOOL)
                    {
                        text = std::to_string(vt_prop.boolVal);
                    }
                    else if (vt_prop.vt == VT_NULL)
                    {
                        qWarning() << __FUNCTION__ << wmi_class << "::" << field << " : " << " Unhandled Type VT_NULL";
                    }
                    else
                    {
                        qWarning() << __FUNCTION__ << wmi_class << "::" << field << " : " << " Unhandled Type " << vt_prop.vt;
                    }

                    fieldMap[std::string(field.begin(), field.end())].push_back(text);
                }
                VariantClear(&vt_prop);
            }

            obj->Release();
        }
    }
    else
    {
        HRESULT    hRes = WBEM_S_NO_ERROR;
        // Final Next will return WBEM_S_FALSE
        ULONG            uReturned;
        IWbemClassObject* obj[256];

        hRes = m_enumerator->Next(WBEM_INFINITE, count, obj, &uReturned);

        if (SUCCEEDED(hRes))
        {
            for (ULONG n = 0; n < uReturned; n++)
            {
                for (auto&& field : fields)
                {
                    VARIANT vt_prop;
                    VariantInit(&vt_prop);
                    CIMTYPE pType;
                    long plFlavor;

                    HRESULT hr = obj[n]->Get(field.c_str(), 0, &vt_prop, &pType, &plFlavor);

                    if (SUCCEEDED(hr)) {
                        std::string text;
                        if (vt_prop.vt == VT_BSTR)
                        {
                            text = _com_util::ConvertBSTRToString(vt_prop.bstrVal);
                        }
                        else if (vt_prop.vt == VT_I4)
                        {
                            text = std::to_string(vt_prop.uintVal);
                        }
                        else if (vt_prop.vt == VT_UI8)
                        {
                            text = std::to_string(vt_prop.ullVal);
                        }
                        else if (vt_prop.vt == VT_BOOL)
                        {
                            text = std::to_string(vt_prop.boolVal);
                        }
                        else if (vt_prop.vt == VT_NULL)
                        {
                            qWarning() << __FUNCTION__ << wmi_class << "::" << field << " : " << " Unhandled Type VT_NULL";
                        }
                        else
                        {
                            qWarning() << __FUNCTION__ << wmi_class << "::" << field << " : " << " Unhandled Type " << vt_prop.vt;
                        }

                        fieldMap[std::string(field.begin(), field.end())].push_back(text);
                    }
                    VariantClear(&vt_prop);
                }

                obj[n]->Release();
            }
        }
    }

    return fieldMap;
}