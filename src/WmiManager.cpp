#include "WmiManager.h"

#include <comdef.h>
#include <comutil.h>
#include <iostream>
#pragma comment(lib, "wbemuuid.lib")

#include <qDebug>
//#include <Wbemidl.h>

//#include <Wbemidl.h>

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
    init();
}

void WmiManager::readCpuFrequency()
{
    if (m_isWmiFrequencyInfoAvailable)
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

    auto percentProcessorPerformance = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation",
        L"PercentProcessorPerformance", L"NOT Name LIKE '%_Total\'", 16);
    if (percentProcessorPerformance.empty()) {
        return;
    }

    auto percentProcessorUtility = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation",
        L"PercentProcessorUtility", L"NOT Name LIKE '%_Total\'", 16);
    if (percentProcessorUtility.empty()) {
        return;
    }

    uint16_t maxFrequency = 0;

    for (size_t i = 0; i < staticInfo.threadCount; i++)
    {
        const double performance = std::stod(percentProcessorPerformance[i]) / 100;
        double currentFrequency = staticInfo.baseFrequency * performance;
        dynamicInfo.cpuThreadFrequencies[i] = currentFrequency;
        if (currentFrequency > maxFrequency)
        {
            maxFrequency = currentFrequency;
        }

        const double usage = std::stod(percentProcessorUtility[i]);// *100;
        dynamicInfo.cpuThreadUsages[i] = usage;
    }

    dynamicInfo.cpuMaxFrequency = maxFrequency;
}

void WmiManager::readFanSpeed()
{
    if (m_isWmiFanInfoAvailable)
    {
        return;
    }

    auto availability = query(L"Win32_Fan", L"Availability");
    if (!availability.empty()) {

    }

    auto activeCooling = query(L"Win32_Fan", L"ActiveCooling");
    if (!activeCooling.empty()) {

    }

    auto desiredSpeed = query(L"Win32_Fan", L"DesiredSpeed");
    if (!desiredSpeed.empty()) {

    }

    auto configManagerErrorCode = query(L"Win32_Fan", L"ConfigManagerErrorCode");
    if (!configManagerErrorCode.empty()) {

    }
}

void WmiManager::readCpuInfo()
{
    auto cpuName = query(L"Win32_Processor", L"Name");
    if (!cpuName.empty()) {

    }

    auto cpuManufacturer = query(L"Win32_Processor", L"Manufacturer");
    if (!cpuManufacturer.empty()) {

    }

    auto cpuNumberOfCores = query(L"Win32_Processor", L"NumberOfCores");
    if (!cpuNumberOfCores.empty()) {

    }

    auto cpuNumberOfLogicalProcessors = query(L"Win32_Processor", L"NumberOfLogicalProcessors");
    if (!cpuNumberOfLogicalProcessors.empty()) {

    }

    auto percentofMaximumFrequency = query(L"Win32_Processor", L"MaxClockSpeed");
    if (!percentofMaximumFrequency.empty()) {

    }

    uint32_t baseFrequency = std::stoi(percentofMaximumFrequency[0]);
    staticInfo.baseFrequency = baseFrequency;
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

    // auto percentProcessorPerformance = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation", L"PercentProcessorPerformance");
    // auto percentProcessorUtility = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation", L"PercentProcessorUtility");
    // if (!percentProcessorPerformance.empty() && !percentProcessorUtility.empty()) {
    //     m_isWmiFrequencyInfoAvailable = true;
    // }

    // auto fanInfo = query(L"Win32_Fan", L"*");
    // if (!fanInfo.empty()) {
    //     m_isWmiFanInfoAvailable = true;
    // }

    return true;
}

void WmiManager::readQuery(const std::string className, const std::string parameter)
{
    //L"Win32_Processor", L"Name"
    auto result = query(QString(className.c_str()).toStdWString(), QString(parameter.c_str()).toStdWString());
    if (!result.empty()) {

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

inline std::string wstring_to_std_string(const std::wstring& ws) {
    std::string str_locale = setlocale(LC_ALL, "");
    const wchar_t* wch_src = ws.c_str();

#ifdef _MSC_VER
    size_t n_dest_size;
    wcstombs_s(&n_dest_size, nullptr, 0, wch_src, 0);
    n_dest_size++;  // Increase by one for null terminator

    char* ch_dest = new char[n_dest_size];
    memset(ch_dest, 0, n_dest_size);

    size_t n_convert_size;
    wcstombs_s(&n_convert_size, ch_dest, n_dest_size, wch_src,
        n_dest_size - 1);  // subtract one to ignore null terminator

    std::string result_text = ch_dest;
    delete[] ch_dest;
#else
    size_t n_dest_size = wcstombs(NULL, wch_src, 0) + 1;
    char* ch_dest = new char[n_dest_size];
    memset(ch_dest, 0, n_dest_size);
    wcstombs(ch_dest, wch_src, n_dest_size);
    std::string result_text = ch_dest;
    delete[] ch_dest;
#endif

    setlocale(LC_ALL, str_locale.c_str());
    return result_text;
}

//template <>
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
        long timeout = WBEM_INFINITE;//WBEM_INFINITE
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
        IWbemClassObject* obj[16];

        hRes = m_enumerator->Next(WBEM_INFINITE, 16, obj, &uReturned);

        if (SUCCEEDED(hRes))
        {
            // Do something with the objects.
            //ProcessObjects( uReturned,  apObj );

            for (ULONG n = 0; n < uReturned; n++)
            {
                VARIANT vt_prop;
                HRESULT hr = obj[n]->Get(field.c_str(), 0, &vt_prop, nullptr, nullptr);

                if (SUCCEEDED(hr)) {
                    if (vt_prop.vt == VT_BSTR)
                    {
                        //result.push_back(wstring_to_std_string(vt_prop.bstrVal));
                        //assert(bs != nullptr);
                        //std::wstring ws(vt_prop.bstrVal, SysStringLen(vt_prop.bstrVal));
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
        }    // If Enum succeeded...
    }

    return result;
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