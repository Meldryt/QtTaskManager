#include "CpuInfo.h"

#include <QElapsedTimer>
#include <QDebug>
#include <QVariant>
#include <QObject>

#ifdef _WIN32
//#include <intrin.h>
//#include <powerbase.h>
//#include <processthreadsapi.h>
//#include <winternl.h>
#include <PowrProf.h>
#pragma comment(lib, "Powrprof.lib")
#endif

//#include <iphlpapi.h>
//#include <wtsapi32.h>
//#include <Winsvc.h>
//#include <PowrProf.h>

// Link with Iphlpapi.lib
//#pragma comment(lib, "IPHLPAPI.lib")

#include <comdef.h>
#include <comutil.h>
#include <iostream>
#pragma comment(lib, "wbemuuid.lib")

//#include <Wbemidl.h>

class QuerySink : public IWbemObjectSink
{
    LONG m_lRef;
    bool bDone;
    CRITICAL_SECTION threadLock; // for thread safety

    IEnumWbemClassObject* enumerator;

public:
    QuerySink(IEnumWbemClassObject* e) {
        enumerator = e;
        m_lRef = 0;
        bDone = false;
        InitializeCriticalSection(&threadLock);
    }

    ~QuerySink() {
        bDone = true;
        DeleteCriticalSection(&threadLock);
    }

    virtual ULONG STDMETHODCALLTYPE AddRef()
    {
        return InterlockedIncrement(&m_lRef);
    }

    virtual ULONG STDMETHODCALLTYPE Release()
    {
        LONG lRef = InterlockedDecrement(&m_lRef);
        if (lRef == 0) { delete this; }
        return lRef;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv)
    {
        if (riid == IID_IUnknown || riid == IID_IWbemObjectSink) {
            *ppv = (IWbemObjectSink*)this;
            AddRef();
            return WBEM_S_NO_ERROR;
        }
        else return E_NOINTERFACE;
    }

    virtual HRESULT STDMETHODCALLTYPE Indicate(LONG lObjectCount, IWbemClassObject** apObjArray)
    {
        HRESULT hres = S_OK;
        std::cout << "Indicate called with " << lObjectCount << " results" << std::endl;

        for (int i = 0; i < lObjectCount; i++) {
            VARIANT vt_prop;
            _bstr_t field = L"PercentProcessorPerformance";
            hres = apObjArray[i]->Get(field,
                0, &vt_prop, 0, 0);
            if (!FAILED(hres))
            {
                if (vt_prop.vt == VT_BSTR)
                {
                    //result.push_back(wstring_to_std_string(vt_prop.bstrVal));
                    //assert(bs != nullptr);
                    //std::wstring ws(vt_prop.bstrVal, SysStringLen(vt_prop.bstrVal));
                    char* text = _com_util::ConvertBSTRToString(vt_prop.bstrVal);
                    std::string str = text;
                    qDebug() << text;
                }
                else if (vt_prop.vt == VT_I4)
                {
                    std::string str = std::to_string(vt_prop.uintVal);
                }
                else
                {
                    std::string str = std::to_string(vt_prop.ullVal);
                }
                VariantClear(&vt_prop);
            }
 /*           if (FAILED(hres)) {
                std::cout << "Failed to get the data from the query" << " Error code = 0x" << std::hex << hres << std::endl; return WBEM_E_FAILED;
            }*/

            printf("Name: %ls\n", V_BSTR(&vt_prop));
        }

        return WBEM_S_NO_ERROR;
    }

    virtual HRESULT STDMETHODCALLTYPE SetStatus(LONG lFlags, HRESULT hResult, BSTR strParam, IWbemClassObject __RPC_FAR* pObjParam)
    {
        if (lFlags == WBEM_STATUS_COMPLETE) {
            printf("Call complete.\n");

            EnterCriticalSection(&threadLock);
            bDone = true;
            LeaveCriticalSection(&threadLock);
        }
        else if (lFlags == WBEM_STATUS_PROGRESS) {
            printf("Call in progress.\n");
        }

        return WBEM_S_NO_ERROR;
    }

    bool IsDone()
    {
        bool done = true;

        EnterCriticalSection(&threadLock);
        done = bDone;
        LeaveCriticalSection(&threadLock);

        return done;
    }    // end of QuerySink.cpp
};

QuerySink* sink;

typedef struct _MIB_UDP6TABLE_OWNER_PID {
} MIB_UDP6TABLE_OWNER_PID, * PMIB_UDP6TABLE_OWNER_PID;

typedef struct _PROCESSOR_POWER_INFORMATION {
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, * PPROCESSOR_POWER_INFORMATION;

CpuInfo::CpuInfo()
{
    sink = new QuerySink(enumerator);
}

void CpuInfo::init()
{
    setupWmi();
    fetchStaticInfo();

//todo: necessary to split? move this to function.
#ifdef _WIN32
    PdhOpenQuery(NULL, 0, &totalCPUQuery);
    std::string text = "\\Processor(_Total)\\% Processor Time";
    std::wstring ws = std::wstring(text.begin(), text.end());
    LPCWSTR sw = ws.c_str();
    // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
    PdhAddEnglishCounter(totalCPUQuery, sw, 0, &totalCPUCounter);
    PdhCollectQueryData(totalCPUQuery);

    for(uint8_t i=0;i<staticInfo.processorCount;++i)
    {
        PDH_HQUERY singleCPUQuery;
        PDH_HCOUNTER singleCPUCounter;
        text = "\\Processor("+std::to_string(i)+")\\% Processor Time";
        ws = std::wstring(text.begin(), text.end());
        sw = ws.c_str();

        PdhOpenQuery(NULL, 0, &singleCPUQuery);
        // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
        PdhAddEnglishCounter(singleCPUQuery, sw, 0, &singleCPUCounter);
        PdhCollectQueryData(singleCPUQuery);

        singleCPUQueries.push_back(singleCPUQuery);
        singleCPUCounters.push_back(singleCPUCounter);
        dynamicInfo.coreUsages.push_back(0.0);
    }
    
#else
#endif
}

const Globals::CpuStaticInfo& CpuInfo::getStaticInfo() const
{
    return staticInfo;
}

const Globals::CpuDynamicInfo& CpuInfo::getDynamicInfo() const
{
    return dynamicInfo;
}

void CpuInfo::update()
{
    readFrequency();
    fetchDynamicInfo();
}

#ifdef _WIN32
// Helper function to count set bits in the processor mask.
DWORD CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}
#endif

void CpuInfo::fetchStaticInfo()
{
#ifdef _WIN32
    fetchStaticInfoWindows();
#else
    fetchStaticInfoLinux();
#endif

    auto cpuName = query(L"Win32_Processor", L"Name");
    if (cpuName.empty()) {
        return;
    }

    auto cpuManufacturer = query(L"Win32_Processor", L"Manufacturer");
    if (cpuManufacturer.empty()) {
        return;
    }

    auto cpuNumberOfCores = query(L"Win32_Processor", L"NumberOfCores");
    if (cpuNumberOfCores.empty()) {
        return;
    }

    auto cpuNumberOfLogicalProcessors = query(L"Win32_Processor", L"NumberOfLogicalProcessors");
    if (cpuNumberOfLogicalProcessors.empty()) {
        return;
    }

    auto percentofMaximumFrequency = query(L"Win32_Processor", L"MaxClockSpeed");
    if (percentofMaximumFrequency.empty()) {
        return;
    }

    uint32_t baseFrequency = std::stoi(percentofMaximumFrequency[0]);
    staticInfo.baseFrequency = baseFrequency;
}

#ifdef _WIN32
void CpuInfo::fetchStaticInfoWindows()
{
    int CpuInfo[4] = {-1};
    unsigned nExIds, i =  0;
    char CPUBrandString[0x40];
    // Get the information associated with each extended ID.
    __cpuid(CpuInfo, 0x80000000);
    nExIds = CpuInfo[0];
    for (i=0x80000000; i<=nExIds; ++i)
    {
        __cpuid(CpuInfo, i);
        // Interpret CPU brand string
        if  (i == 0x80000002)
        {
            memcpy(CPUBrandString, CpuInfo, sizeof(CpuInfo));
        }
        else if  (i == 0x80000003)
        {
            memcpy(CPUBrandString + 16, CpuInfo, sizeof(CpuInfo));
        }
        else if  (i == 0x80000004)
        {
            memcpy(CPUBrandString + 32, CpuInfo, sizeof(CpuInfo));
        }
    }

    CpuInfo[0] = 0;
    CpuInfo[1] = 0;
    CpuInfo[2] = 0;
    CpuInfo[3] = 0;

    __cpuid(CpuInfo, 0);
    if (CpuInfo[0] >= 0x16)
    {
        __cpuid(CpuInfo, 0x16);
        //qDebug() << "EAX: 0x%08x EBX: 0x%08x ECX: %08x\r" << CpuInfo[0] << CpuInfo[1] << CpuInfo[2];
        //qDebug() << "Processor Base Frequency:  %04d MHz\r" << CpuInfo[0];
        //qDebug() << "Maximum Frequency:         %04d MHz\r" << CpuInfo[1];
        //qDebug() << "Bus (Reference) Frequency: %04d MHz\r" << CpuInfo[2];
    }

    //string includes manufacturer, model and clockspeed
    staticInfo.cpuBrand = CPUBrandString;
    //qDebug() << "CPU Type: " << staticSystemInfo.cpuBrand.c_str();

    //SYSTEM_INFO sysInfo;
    //GetSystemInfo(&sysInfo);
    //staticInfo.processorCount = sysInfo.dwNumberOfProcessors;
    //qDebug() << "Number of Cores: " << staticSystemInfo.processorCount;

    typedef SYSTEM_LOGICAL_PROCESSOR_INFORMATION Info;

    std::vector<Info> info;
    DWORD size = 0;
    GetLogicalProcessorInformation(0, &size);
    info.resize(size / sizeof(Info));
    GetLogicalProcessorInformation(info.data(), &size);

    DWORD logicalProcessorCount = 0;
    DWORD numaNodeCount = 0;
    DWORD processorCoreCount = 0;
    DWORD processorL1CacheCount = 0;
    DWORD processorL2CacheCount = 0;
    DWORD processorL3CacheCount = 0;
    DWORD processorPackageCount = 0;
    DWORD processorL1CacheSize = 0;
    DWORD processorL2CacheSize = 0;
    DWORD processorL3CacheSize = 0;

    for (size_t i = 0; i < info.size(); ++i)
    {
        switch (info[i].Relationship)
        {
        case RelationNumaNode:
            // Non-NUMA systems report a single record of this type.
            numaNodeCount++;
            continue;
        case RelationProcessorCore:
            processorCoreCount++;
            // A hyperthreaded core supplies more than one logical processor.
            logicalProcessorCount += CountSetBits(info[i].ProcessorMask);
            continue;
        case RelationCache:
            // Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache.
            if (info[i].Cache.Level == 1)
            {
                processorL1CacheCount++;
                processorL1CacheSize += info[i].Cache.Size;
            }
            else if (info[i].Cache.Level == 2)
            {
                processorL2CacheCount++;
                processorL2CacheSize += info[i].Cache.Size;
            }
            else if (info[i].Cache.Level == 3)
            {
                processorL3CacheCount++;
                processorL3CacheSize += info[i].Cache.Size;
            }
            continue;
        case RelationProcessorPackage:
            // Logical processors share a physical package.
            processorPackageCount++;
            continue;
        default:
            continue;
        }
    }

    staticInfo.processorCount = processorCoreCount;
    staticInfo.threadCount = logicalProcessorCount;
    staticInfo.l1CacheSize = processorL1CacheSize/1024;
    staticInfo.l2CacheSize = processorL2CacheSize/1024;
    staticInfo.l3CacheSize = processorL3CacheSize/1024;
    dynamicInfo.cpuFrequencies.resize(logicalProcessorCount);
    dynamicInfo.cpuThreadUsages.resize(logicalProcessorCount);
}
#else
void CpuInfo::fetchStaticInfoLinux()
{

}
#endif


void CpuInfo::fetchDynamicInfo()
{
//    PdhOpenQuery(NULL, NULL, &cpuQuery);
//    // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
//    PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
//    PdhCollectQueryData(cpuQuery);

#ifdef _WIN32
    fetchDynamicInfoWindows();
#else
    fetchDynamicInfoLinux();
#endif
}

#ifdef _WIN32
void CpuInfo::fetchDynamicInfoWindows()
{
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(totalCPUQuery);
    PdhGetFormattedCounterValue(totalCPUCounter, PDH_FMT_DOUBLE, NULL, &counterVal);
    dynamicInfo.cpuTotalUsage = counterVal.doubleValue;

    for(uint8_t i=0;i<staticInfo.processorCount;++i)
    {
        PdhCollectQueryData(singleCPUQueries[i]);
        PdhGetFormattedCounterValue(singleCPUCounters[i], PDH_FMT_DOUBLE, NULL, &counterVal);
        dynamicInfo.coreUsages[i] = counterVal.doubleValue;
    }
}
#else
void CpuInfo::fetchDynamicInfoLinux()
{

}
#endif

/*
* Return CPU frequency.
*/
//@note: CallNtPowerInformation does not give current frequency anymore since Windows 10 21H1 (19043)

void CpuInfo::readFrequency()
{

    //auto percentProcessorPerformanceTotal = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation",
    //    L"PercentProcessorPerformance", L"Name = '_Total'");
    //if (percentProcessorPerformanceTotal.empty()) {
    //    return;
    //}

    auto percentProcessorPerformance = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation",
        L"PercentProcessorPerformance", L"NOT Name LIKE '%_Total\'", 16);
    //if (percentProcessorPerformance.empty()) {
    //    //return;
    //}

    //auto percentProcessorUtility = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation",
    //    L"PercentProcessorUtility");
    //if (percentProcessorUtility.empty()) {
    //    return;
    //}

    //auto percentProcessorUtilityTotal = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation",
    //    L"PercentProcessorUtility", L"Name = '_Total'");
    //if (percentProcessorUtilityTotal.empty()) {
    //    return;
    //}

    auto percentProcessorUtility = query(L"Win32_PerfFormattedData_Counters_ProcessorInformation",
        L"PercentProcessorUtility", L"NOT Name LIKE '%_Total\'", 16);
    if (percentProcessorUtility.empty()) {
        return;
    }

    uint16_t maxFrequency = 0;

    for (size_t i = 0; i < staticInfo.threadCount; i++)
    {
        const double performance = std::stod(percentProcessorPerformance[i]) / 100;
        uint16_t currentFrequency = std::round(staticInfo.baseFrequency * performance);
        dynamicInfo.cpuFrequencies[i] = currentFrequency;
        if (currentFrequency > maxFrequency)
        {
            maxFrequency = currentFrequency;
        }

        const double usage = std::stod(percentProcessorUtility[i]) * 100;
        dynamicInfo.cpuThreadUsages[i] = usage;
    }

    dynamicInfo.cpuCurrentMaxFrequency = maxFrequency;
}

bool CpuInfo::executeQuery(const std::wstring& query) {
    if (service == nullptr) return false;
    return SUCCEEDED(service->ExecQuery(bstr_t(L"WQL"), bstr_t(std::wstring(query.begin(), query.end()).c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &enumerator));
}

bool CpuInfo::executeQueryAsync(const std::wstring& query) {
    if (service == nullptr || sink == nullptr) return false;
    return SUCCEEDED(service->ExecQueryAsync(bstr_t(L"WQL"), bstr_t(std::wstring(query.begin(), query.end()).c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, sink));
}

bool CpuInfo::setupWmi()
{
    auto res = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
        nullptr, EOAC_NONE, nullptr);
    //if (!SUCCEEDED(res)) {
    //    throw std::runtime_error("error initializing WMI");
    //}
    res &= CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    //if (!SUCCEEDED(res)) {
    //    throw std::runtime_error("error initializing WMI");
    //}
    res &= CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&locator);
    if (!SUCCEEDED(res)) {
        throw std::runtime_error("error initializing WMI");
    }
    if (locator) {
        res &= locator->ConnectServer(_bstr_t("ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &service);
        if (service)
            res &= CoSetProxyBlanket(service, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL,
                RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    }

    if (!SUCCEEDED(res)) {
        throw std::runtime_error("error initializing WMI");
    }
}

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
std::vector<std::string> CpuInfo::query(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter, const ULONG count) {

    QElapsedTimer elapsedTimer;
    qint64 elapsedTime;

    elapsedTimer.start();

    std::wstring filter_string;
    if (!filter.empty()) {
        filter_string.append(L" WHERE " + filter);
    }
    std::wstring query_string(L"SELECT " + field + L" FROM " + wmi_class + filter_string);
    bool success = executeQuery(query_string);
    if (!success) {
        return {};
    }

    elapsedTime = elapsedTimer.elapsed();

    qDebug() << "CpuWorker::query()1: " << elapsedTime;

    elapsedTimer.start();

    std::vector<std::string> result;

    //ManagementObjectSearcher managementObjectSearcher = new ManagementObjectSearcher("SELECT Caption FROM Win32_OperatingSystem");

    if (count == 1)
    {
        ULONG u_return = 0;
        IWbemClassObject* obj = nullptr;
        long timeout = WBEM_INFINITE;//WBEM_INFINITE
        while (enumerator) {
            //elapsedTimer.start();
            enumerator->Next(timeout, count, &obj, &u_return);
            //elapsedTime = elapsedTimer.elapsed();

            //qDebug() << "CpuWorker::query()2: " << elapsedTime;
            if (!u_return) {
                break;
            }
            VARIANT vt_prop;
            HRESULT hr = obj->Get(field.c_str(), 0, &vt_prop, nullptr, nullptr);
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
            obj->Release();
        }
    }
    else
    {
        HRESULT    hRes = WBEM_S_NO_ERROR;
        // Final Next will return WBEM_S_FALSE
        ULONG            uReturned;
        IWbemClassObject* obj[16];

        hRes = enumerator->Next(WBEM_INFINITE, 16, obj, &uReturned);

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

    elapsedTime = elapsedTimer.elapsed();

    qDebug() << "CpuWorker::query()2: " << elapsedTime;

    return result;
}

void CpuInfo::queryAsync(const std::wstring& wmi_class, const std::wstring& field, const std::wstring& filter, const ULONG count) {

    QElapsedTimer elapsedTimer;
    qint64 elapsedTime;

    elapsedTimer.start();

    std::wstring filter_string;
    if (!filter.empty()) {
        filter_string.append(L" WHERE " + filter);
    }
    std::wstring query_string(L"SELECT " + field + L" FROM " + wmi_class + filter_string);
    bool success = executeQueryAsync(query_string);
    if (!success) {
        return;
    }

    elapsedTime = elapsedTimer.elapsed();

    qDebug() << "CpuWorker::query()1: " << elapsedTime;
}