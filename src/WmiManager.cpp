#include "WmiManager.h"

#include <comdef.h>
#include <comutil.h>
#include <iostream>
#pragma comment(lib, "wbemuuid.lib")

#include <qDebug>
//#include <Wbemidl.h>

//@todo: implement async calls
// 
// class QuerySink : public IWbemObjectSink
// {
//     LONG m_lRef;
//     bool bDone;
//     CRITICAL_SECTION threadLock; // for thread safety

//     IEnumWbemClassObject* enumerator;

// public:
//     QuerySink(IEnumWbemClassObject* e) {
//         enumerator = e;
//         m_lRef = 0;
//         bDone = false;
//         InitializeCriticalSection(&threadLock);
//     }

//     ~QuerySink() {
//         bDone = true;
//         DeleteCriticalSection(&threadLock);
//     }

//     virtual ULONG STDMETHODCALLTYPE AddRef()
//     {
//         return InterlockedIncrement(&m_lRef);
//     }

//     virtual ULONG STDMETHODCALLTYPE Release()
//     {
//         LONG lRef = InterlockedDecrement(&m_lRef);
//         if (lRef == 0) { delete this; }
//         return lRef;
//     }

//     virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv)
//     {
//         if (riid == IID_IUnknown || riid == IID_IWbemObjectSink) {
//             *ppv = (IWbemObjectSink*)this;
//             AddRef();
//             return WBEM_S_NO_ERROR;
//         }
//         else return E_NOINTERFACE;
//     }

//     virtual HRESULT STDMETHODCALLTYPE Indicate(LONG lObjectCount, IWbemClassObject** apObjArray)
//     {
//         HRESULT hres = S_OK;
//         std::cout << "Indicate called with " << lObjectCount << " results" << std::endl;

//         for (int i = 0; i < lObjectCount; i++) {
//             VARIANT vt_prop;
//             _bstr_t field = L"PercentProcessorPerformance";
//             hres = apObjArray[i]->Get(field,
//                 0, &vt_prop, 0, 0);
//             if (!FAILED(hres))
//             {
//                 if (vt_prop.vt == VT_BSTR)
//                 {
//                     //result.push_back(wstring_to_std_string(vt_prop.bstrVal));
//                     //assert(bs != nullptr);
//                     //std::wstring ws(vt_prop.bstrVal, SysStringLen(vt_prop.bstrVal));
//                     char* text = _com_util::ConvertBSTRToString(vt_prop.bstrVal);
//                     std::string str = text;
//                     qDebug() << text;
//                 }
//                 else if (vt_prop.vt == VT_I4)
//                 {
//                     std::string str = std::to_string(vt_prop.uintVal);
//                 }
//                 else
//                 {
//                     std::string str = std::to_string(vt_prop.ullVal);
//                 }
//                 VariantClear(&vt_prop);
//             }
//  /*           if (FAILED(hres)) {
//                 std::cout << "Failed to get the data from the query" << " Error code = 0x" << std::hex << hres << std::endl; return WBEM_E_FAILED;
//             }*/

//             printf("Name: %ls\n", V_BSTR(&vt_prop));
//         }

//         return WBEM_S_NO_ERROR;
//     }

//     virtual HRESULT STDMETHODCALLTYPE SetStatus(LONG lFlags, HRESULT hResult, BSTR strParam, IWbemClassObject __RPC_FAR* pObjParam)
//     {
//         if (lFlags == WBEM_STATUS_COMPLETE) {
//             printf("Call complete.\n");

//             EnterCriticalSection(&threadLock);
//             bDone = true;
//             LeaveCriticalSection(&threadLock);
//         }
//         else if (lFlags == WBEM_STATUS_PROGRESS) {
//             printf("Call in progress.\n");
//         }

//         return WBEM_S_NO_ERROR;
//     }

//     bool IsDone()
//     {
//         bool done = true;

//         EnterCriticalSection(&threadLock);
//         done = bDone;
//         LeaveCriticalSection(&threadLock);

//         return done;
//     }    // end of QuerySink.cpp
// };

//QuerySink* m_sink;

WmiManager::WmiManager()
{
    //m_sink = new QuerySink(enumerator);
}

//@note: check available wmi classes with WMI Explorer (https://github.com/vinaypamnani/wmie2)
//normally network source is "ROOT\\CIMV2" or "ROOT\\WMI"
//some classes might not be available like "Win32_FanSpeed"
//also using wmi + query is very slow (measured ~250ms per call)

bool WmiManager::init()
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

    const std::vector<std::wstring> fields = { L"PercentProcessorPerformance",L"PercentProcessorUtility"};
    std::map<std::string, std::vector<std::string>> fieldMap = queryArray(L"Win32_PerfFormattedData_Counters_ProcessorInformation", fields);

     if (!fieldMap["PercentProcessorPerformance"].empty() && !fieldMap["PercentProcessorUtility"].empty()) {
         m_isWmiFrequencyInfoAvailable = true;
     }

     auto fanInfo = query(L"Win32_Fan", L"*");
     if (!fanInfo.empty()) {
         m_isWmiFanInfoAvailable = true;
     }

    return true;
}

void WmiManager::readStaticInfo()
{
    readCpuInfo();
}

void WmiManager::update()
{
    if (m_readCpuInfos)
    {
        readCpuFrequency();
        readFanSpeed();
    }

    readNetworkSpeed();
}

void WmiManager::disableCpuUpdates()
{
    m_readCpuInfos = false;
}

const Globals::CpuStaticInfo& WmiManager::cpuStaticInfo() const
{
    return m_cpuStaticInfo;
}

const Globals::CpuDynamicInfo& WmiManager::cpuDynamicInfo() const
{
    return m_cpuDynamicInfo;
}

const Globals::NetworkDynamicInfo& WmiManager::networkDynamicInfo() const
{
    return m_networkDynamicInfo;
}

/*
* read CPU frequency.
*/
//@note: CallNtPowerInformation does not give current frequency anymore since Windows 10 21H1 (19043)
void WmiManager::readCpuFrequency()
{
    if (!m_isWmiFrequencyInfoAvailable)
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

    uint16_t maxFrequency = 0;

    for (size_t i = 0; i < m_cpuStaticInfo.threadCount; i++)
    {
        const double performance = std::stod(fieldMap["PercentProcessorPerformance"][i]) / 100;
        double currentFrequency = m_cpuStaticInfo.baseFrequency * performance;
        m_cpuDynamicInfo.cpuThreadFrequencies[i] = currentFrequency;
        if (currentFrequency > maxFrequency)
        {
            maxFrequency = currentFrequency;
        }

        const double usage = std::stod(fieldMap["PercentProcessorUtility"][i]);// *100;
        m_cpuDynamicInfo.cpuThreadUsages[i] = usage;
    }

    m_cpuDynamicInfo.cpuMaxFrequency = maxFrequency;
}

void WmiManager::readCpuInfo()
{
    const std::vector<std::wstring> fields = { L"Name",L"Manufacturer",L"NumberOfCores",L"NumberOfLogicalProcessors",L"MaxClockSpeed" };
    std::map<std::string, std::vector<std::string>> fieldMap = queryArray(L"Win32_Processor", fields);

    if (!fieldMap["Name"].empty()) {

    }

    if (!fieldMap["Manufacturer"].empty()) {

    }

    if (!fieldMap["NumberOfCores"].empty()) {

    }

    if (!fieldMap["NumberOfLogicalProcessors"].empty()) {

    }

    if (!fieldMap["MaxClockSpeed"].empty()) {

    }

    uint32_t baseFrequency = std::stoi(fieldMap["MaxClockSpeed"][0]);
    m_cpuStaticInfo.baseFrequency = baseFrequency;
}

void WmiManager::readFanSpeed()
{
    if (m_isWmiFanInfoAvailable)
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
//void WmiManager::readThermalZoneTemperature()
//{
    //BSTR query = SysAllocString(L"SELECT * FROM MSAcpi_ThermalZoneTemperature");
    //auto thermalZoneTemperature = query(L"MSAcpi_ThermalZoneTemperature", L"InstanceName,CurrentTemperature");
    //auto thermalZoneTemperature = query(L"MSAcpi_ThermalZoneTemperature", L"*");
    //if (!thermalZoneTemperature.empty()) {
    //    //return;
    //    const double temperature = std::stod(thermalZoneTemperature[0]);
    //    dynamicInfo.cpuTemperature = temperature;
    //}
//}

void WmiManager::readNetworkSpeed()
{
    const std::vector<std::wstring> fields = { L"Name",L"BytesReceivedPerSec",L"BytesSentPerSec",L"BytesTotalPerSec",L"CurrentBandwidth" };
    std::map<std::string, std::vector<std::string>> fieldMap = queryArray(L"Win32_PerfFormattedData_Tcpip_NetworkInterface", fields);

    if (!fieldMap["Name"].empty()) {
        m_networkDynamicInfo.names.clear();
        for (auto&& netWorkInterfaceName : fieldMap["Name"])
        {
            m_networkDynamicInfo.names.push_back(netWorkInterfaceName);
        }
    }

    if (!fieldMap["BytesReceivedPerSec"].empty()) {
        m_networkDynamicInfo.bytesReceivedPerSec.clear();
        for (auto&& bytesReceivedPerSec : fieldMap["BytesReceivedPerSec"])
        {
            m_networkDynamicInfo.bytesReceivedPerSec.push_back(std::stoi(bytesReceivedPerSec));
        }
    }

    if (!fieldMap["BytesSentPerSec"].empty()) {
        m_networkDynamicInfo.bytesSentPerSec.clear();
        for (auto&& bytesSentPerSec : fieldMap["BytesSentPerSec"])
        {
            m_networkDynamicInfo.bytesSentPerSec.push_back(std::stoi(bytesSentPerSec));
        }
    }

    if (!fieldMap["BytesTotalPerSec"].empty()) {
        m_networkDynamicInfo.bytesTotalPerSec.clear();
        for (auto&& bytesTotalPerSec : fieldMap["BytesTotalPerSec"])
        {
            m_networkDynamicInfo.bytesTotalPerSec.push_back(std::stoi(bytesTotalPerSec));
        }
    }

    if (!fieldMap["CurrentBandwidth"].empty()) {
        m_networkDynamicInfo.currentBandwidth.clear();
        for (auto&& currentBandwidth : fieldMap["CurrentBandwidth"])
        {
            m_networkDynamicInfo.currentBandwidth.push_back(std::stoi(currentBandwidth));
        }
    }
}


bool WmiManager::executeQuery(const std::wstring& query) {
    if (m_service == nullptr) return false;
    return SUCCEEDED(m_service->ExecQuery(bstr_t(L"WQL"), bstr_t(std::wstring(query.begin(), query.end()).c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &m_enumerator));
}

//bool WmiManager::executeQueryAsync(const std::wstring& query) {
//    if (service == nullptr || m_sink == nullptr) return false;
//    return SUCCEEDED(service->ExecQueryAsync(bstr_t(L"WQL"), bstr_t(std::wstring(query.begin(), query.end()).c_str()),
//        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, sink));
//}

//inline std::string wstring_to_std_string(const std::wstring& ws) {
//    std::string str_locale = setlocale(LC_ALL, "");
//    const wchar_t* wch_src = ws.c_str();
//
//#ifdef _MSC_VER
//    size_t n_dest_size;
//    wcstombs_s(&n_dest_size, nullptr, 0, wch_src, 0);
//    n_dest_size++;  // Increase by one for null terminator
//
//    char* ch_dest = new char[n_dest_size];
//    memset(ch_dest, 0, n_dest_size);
//
//    size_t n_convert_size;
//    wcstombs_s(&n_convert_size, ch_dest, n_dest_size, wch_src,
//        n_dest_size - 1);  // subtract one to ignore null terminator
//
//    std::string result_text = ch_dest;
//    delete[] ch_dest;
//#else
//    size_t n_dest_size = wcstombs(NULL, wch_src, 0) + 1;
//    char* ch_dest = new char[n_dest_size];
//    memset(ch_dest, 0, n_dest_size);
//    wcstombs(ch_dest, wch_src, n_dest_size);
//    std::string result_text = ch_dest;
//    delete[] ch_dest;
//#endif
//
//    setlocale(LC_ALL, str_locale.c_str());
//    return result_text;
//}

std::vector<std::string> WmiManager::query(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter, const ULONG count) 
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
            HRESULT hr = obj->Get(field.c_str(), 0, &vt_prop, nullptr, nullptr);
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
                else
                {
                    qWarning() << "CpuInfo::query: Unhandled Type " << vt_prop.vt;
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
                HRESULT hr = obj[n]->Get(field.c_str(), 0, &vt_prop, nullptr, nullptr);

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
                    else
                    {
                        std::string str = std::to_string(vt_prop.ullVal);
                        result.push_back(str);
                    }
                }
                VariantClear(&vt_prop);
                obj[n]->Release();
            }
        }
    }

    return result;
}


std::map<std::string, std::vector<std::string>> WmiManager::queryArray(const std::wstring& wmi_class, const std::vector<std::wstring>& fields, const std::wstring& filter, const ULONG count)
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

                HRESULT hr = obj->Get(field.c_str(), 0, &vt_prop, nullptr, nullptr);

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
                    else
                    {
                        qWarning() << "CpuInfo::query: Unhandled Type " << vt_prop.vt;
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

                    HRESULT hr = obj[n]->Get(field.c_str(), 0, &vt_prop, nullptr, nullptr);

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
                        else
                        {
                            qWarning() << "CpuInfo::query: Unhandled Type " << vt_prop.vt;
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

//void WmiManager::queryAsync(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter, const ULONG count) 
//{
//    std::wstring filter_string;
//    if (!filter.empty()) {
//        filter_string.append(L" WHERE " + filter);
//    }
//    std::wstring query_string(L"SELECT " + field + L" FROM " + wmi_class + filter_string);
//    bool success = executeQueryAsync(query_string);
//    if (!success) {
//        return;
//    }
//}