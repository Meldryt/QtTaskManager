#include "CpuInfo.h"

#include <QElapsedTimer>
#include <QDebug>
#include <QVariant>
#include <QObject>

#ifdef HAS_RYZEN_MASTER_SDK
#include "external/RyzenMasterMonitoringSDK/include/ICPUEx.h"
#include "external/RyzenMasterMonitoringSDK/include/IPlatform.h"
#include "external/RyzenMasterMonitoringSDK/include/IDeviceManager.h"
#include "external/RyzenMasterMonitoringSDK/include/IBIOSEx.h"
#endif

#include <iostream>

#include <system_error>
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
#include <PdhMsg.h>
#include <intrin.h>

// #include <comdef.h>
// #include <comutil.h>
// #pragma comment(lib, "wbemuuid.lib")

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
    qDebug() << __FUNCTION__;

#ifdef HAS_RYZEN_MASTER_SDK
    m_useRyzenCpuParameters = true;
#endif
}

CpuInfo::~CpuInfo()
{
    qDebug() << __FUNCTION__;
}

void CpuInfo::init()
{
    readStaticInfo();
}

const QMap<uint8_t,QVariant>& CpuInfo::staticInfo() const
{
    return m_staticInfo;
}

const QMap<uint8_t,QVariant>& CpuInfo::dynamicInfo() const
{
    return m_dynamicInfo;
}

void CpuInfo::update()
{
    if (m_useRyzenCpuParameters)
    {
        readDynamicInfoRyzenMaster(); 
    }
    else if (m_useIntelCpuParameters)
    {
        //@todo: implement intel sdk functions
    }

    readPdhFrequency();

    readDynamicInfo();
}

void CpuInfo::initPdh()
{
    //todo: necessary to split? move this to function.
#ifdef _WIN32
    PdhOpenQuery(NULL, 0, &totalCPUQuery);
    std::string text = "\\Processor(_Total)\\% Processor Time";
    std::wstring ws = std::wstring(text.begin(), text.end());
    LPCWSTR sw = ws.c_str();
    // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
    PdhAddEnglishCounter(totalCPUQuery, sw, 0, &totalCPUCounter);
    PdhCollectQueryData(totalCPUQuery);

    for (uint8_t i = 0; i < m_cpuProcessorCount; ++i)
    {
        PDH_HQUERY singleCPUQuery;
        PDH_HCOUNTER singleCPUCounter;
        text = "\\Processor(" + std::to_string(i) + ")\\% Processor Time";
        ws = std::wstring(text.begin(), text.end());
        sw = ws.c_str();

        PdhOpenQuery(NULL, 0, &singleCPUQuery);
        // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
        PdhAddEnglishCounter(singleCPUQuery, sw, 0, &singleCPUCounter);
        PdhCollectQueryData(singleCPUQuery);

        singleCPUQueries.push_back(singleCPUQuery);
        singleCPUCounters.push_back(singleCPUCounter);
    }

    m_cpuCoreUsages.resize(m_cpuProcessorCount);
    m_cpuCoreFrequencies.resize(m_cpuProcessorCount);
    //SYSTEM_INFO siSysInfo;
    //GetNativeSystemInfo(&siSysInfo);
    //m_cpuCount = siSysInfo.dwNumberOfProcessors;

    //std::wstring kProcessorFrequency =
    //    L"\\Processor Information(_Total)\\Processor Frequency";
    //std::wstring kProcessorPerformance =
    //    L"\\Processor Information(_Total)\\% Processor Performance";
    //L"\\Processor Information(0,0)\\Processor Frequency",

    if (PdhOpenQueryW(nullptr, 0, &m_cpuQueryFreq) == ERROR_SUCCESS)
    {
        if (PdhAddEnglishCounterW(m_cpuQueryFreq, L"\\Processor Information(_Total)\\Processor Frequency",
            0, &m_cpuFreqCounter) != ERROR_SUCCESS)
        {
            m_cpuFreqCounter = nullptr;
        }
    }
    else
    {
        m_cpuQueryFreq = nullptr;
    }

    if (PdhOpenQueryW(nullptr, 0, &m_cpuQueryPerformance) == ERROR_SUCCESS)
    {
        if (PdhAddEnglishCounterW(m_cpuQueryPerformance, L"\\Processor Information(_Total)\\% Processor Performance",
            0, &m_cpuPerformanceCounter) != ERROR_SUCCESS)
        {
            m_cpuPerformanceCounter = nullptr;
        }
    }
    else
    {
        m_cpuQueryPerformance = nullptr;
    }


#else
#endif
}

void CpuInfo::initRyzenMaster()
{
#ifdef HAS_RYZEN_MASTER_SDK
    bool bRetCode = false;
    IPlatform& rPlatform = GetPlatform();
    bRetCode = rPlatform.Init();
    if (!bRetCode)
    {
        //_tprintf(_T("Platform init failed\n"));
        return;
    }

    IDeviceManager& rDeviceManager = rPlatform.GetIDeviceManager();
    m_amdCpuDevice = (ICPUEx*)rDeviceManager.GetDevice(dtCPU, 0);
    m_amdCpuBiosDevice = (IBIOSEx*)rDeviceManager.GetDevice(dtBIOS, 0);

    if (!m_amdCpuDevice)
    {
        qDebug() << "Could not init amd devices.";
        return;
    }
    else
    {
        m_useRyzenCpuParameters = true;
    }
#endif
}

void CpuInfo::readStaticInfoRyzenMaster()
{
#ifdef HAS_RYZEN_MASTER_SDK
    int iRet;

    if (m_amdCpuBiosDevice)
    {
        const wchar_t* biosVersion = m_amdCpuBiosDevice->GetVersion();
        if (biosVersion)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): IBIOSEx::GetVersion(): " << QString::fromWCharArray(biosVersion);
        }

        const wchar_t* biosVendor = m_amdCpuBiosDevice->GetVendor();
        if (biosVendor)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): IBIOSEx::GetVendor(): " << QString::fromWCharArray(biosVendor);
        }

        const wchar_t* biosDate = m_amdCpuBiosDevice->GetDate();
        if (biosDate)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): IBIOSEx::GetDate(): " << QString::fromWCharArray(biosDate);
        }
    }

    if (m_amdCpuDevice)
    {
        const wchar_t* cpuName = m_amdCpuDevice->GetName();
        if (cpuName)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetName(): " << QString::fromWCharArray(cpuName);
        }

        const wchar_t* cpuDescription = m_amdCpuDevice->GetDescription();
        if (cpuDescription)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetDescription(): " << QString::fromWCharArray(cpuDescription);
        }

        const wchar_t* cpuVendor = m_amdCpuDevice->GetVendor();
        if (cpuVendor)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetVendor(): " << QString::fromWCharArray(cpuVendor);
        }

        const wchar_t* cpuRole = m_amdCpuDevice->GetRole();
        if (cpuRole)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetRole(): " << QString::fromWCharArray(cpuRole);
        }

        const wchar_t* cpuClassName = m_amdCpuDevice->GetClassName();
        if (cpuClassName)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetClassName(): " << QString::fromWCharArray(cpuClassName);
        }

        const AOD_DEVICE_TYPE deviceType = m_amdCpuDevice->GetType();
        qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetType(): " << static_cast<int>(deviceType);

        const unsigned long cpuIndex = m_amdCpuDevice->GetIndex();
        qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetIndex(): " << cpuIndex;

        CACHE_INFO l1DataCache;
        iRet = m_amdCpuDevice->GetL1DataCache(l1DataCache);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetL1DataCache(): " << l1DataCache.fSize;
        }

        CACHE_INFO l1InstructionCache;
        iRet = m_amdCpuDevice->GetL1InstructionCache(l1InstructionCache);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetL1InstructionCache(): " << l1InstructionCache.fSize;
        }

        CACHE_INFO l2Cache;
        iRet = m_amdCpuDevice->GetL2Cache(l2Cache);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetL2Cache(): " << l2Cache.fSize;
        }

        CACHE_INFO l3Cache;
        iRet = m_amdCpuDevice->GetL3Cache(l3Cache);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetL3Cache(): " << l3Cache.fSize;
        }

        unsigned int coreCount;
        iRet = m_amdCpuDevice->GetCoreCount(coreCount);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetCoreCount(): " << coreCount;
        }

        unsigned int corePark;
        iRet = m_amdCpuDevice->GetCorePark(corePark);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetCorePark(): " << corePark;
        }

        const wchar_t* cpuPackage = m_amdCpuDevice->GetPackage();
        if (cpuPackage)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetPackage(): " << QString::fromWCharArray(cpuPackage);
        }

        std::wstring str;
        iRet = m_amdCpuDevice->GetChipsetName(str.data());
        if (!iRet)
        {
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetChipsetName(): " << QString::fromWCharArray(str.c_str());
        }
    }
#endif
}

void CpuInfo::readDynamicInfoRyzenMaster()
{
    readRyzenDynamicCpuInfo();
    //readRyzenDynamicBiosInfo();
}

//@note: from https://www.amd.com/de/developer/ryzen-master-monitoring-sdk.html
//The API call(GetCPUParameters) included in this SDK should only be called once per second to avoid impacting the load on the SMU.Calls made faster may impact the results.
void CpuInfo::readRyzenDynamicCpuInfo()
{
#ifdef HAS_RYZEN_MASTER_SDK
    if (m_amdCpuDevice)
    {
        CPUParameters stData;
        int iRet = m_amdCpuDevice->GetCPUParameters(stData);
        if (!iRet)
        {
            m_cpuTemperature = stData.dTemperature;
            m_cpuPower = stData.fPPTValue;
            m_cpuSocPower = stData.fVDDCR_SOC_Power;
            m_cpuCoreVoltage = stData.dAvgCoreVoltage;
            m_cpuBaseFrequency = stData.fCCLK_Fmax;
            m_cpuMaxFrequency = stData.dPeakSpeed;

            if (m_cpuCoreFrequencies.empty())
            {
                m_cpuCoreFrequencies.resize(stData.stFreqData.uLength);
            }

            for (unsigned int i = 0; i < stData.stFreqData.uLength; i++)
            {
                if (stData.stFreqData.dFreq[i] != 0)
                {
                    m_cpuCoreFrequencies[i] = stData.stFreqData.dFreq[i];
                }
            }

        }
    }
#endif
}

void CpuInfo::readRyzenDynamicBiosInfo()
{ 
#ifdef HAS_RYZEN_MASTER_SDK
    if (m_amdCpuBiosDevice)
    {
        int iRet;
        unsigned short memVDDIO;
        iRet = m_amdCpuBiosDevice->GetMemVDDIO(memVDDIO);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readRyzenDynamicBiosInfo(): IBIOSEx::GetMemVDDIO(): " << memVDDIO;
        }

        unsigned short currentMemClock;
        iRet = m_amdCpuBiosDevice->GetCurrentMemClock(currentMemClock);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readRyzenDynamicBiosInfo(): IBIOSEx::GetCurrentMemClock(): " << currentMemClock;
        }

        unsigned char memCtrlTcl;
        iRet = m_amdCpuBiosDevice->GetMemCtrlTcl(memCtrlTcl);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readRyzenDynamicBiosInfo(): IBIOSEx::GetMemCtrlTcl(): " << memCtrlTcl;
        }

        unsigned char memCtrlTrcdrd;
        iRet = m_amdCpuBiosDevice->GetMemCtrlTrcdrd(memCtrlTrcdrd);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readRyzenDynamicBiosInfo(): IBIOSEx::GetMemCtrlTrcdrd(): " << memCtrlTrcdrd;
        }

        unsigned char memCtrlTras;
        iRet = m_amdCpuBiosDevice->GetMemCtrlTras(memCtrlTras);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readRyzenDynamicBiosInfo(): IBIOSEx::GetMemCtrlTras(): " << memCtrlTras;
        }

        unsigned char memCtrlTrp;
        iRet = m_amdCpuBiosDevice->GetMemCtrlTrp(memCtrlTrp);
        if (!iRet)
        {
            qDebug() << "CpuInfo::readRyzenDynamicBiosInfo(): IBIOSEx::GetMemCtrlTrp(): " << memCtrlTrp;
        }
    }
#endif
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

void CpuInfo::readStaticInfo()
{
    readSystemInfo();

    //if (m_useWmi)
    //{
    //    m_wmiManager = new WmiManager();
    //    m_wmiManager->init();
    //    m_wmiManager->readStaticInfo();
    //    
    //    if (m_useRyzenCpuParameters)
    //    {
    //        //m_wmiManager->disableCpuUpdates();
    //    }
    //}

    initPdh();
    readPdhBaseFrequency();

    initRyzenMaster();
    readStaticInfoRyzenMaster();

    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Brand] = QString::fromStdString(m_cpuBrand);
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_ProcessorCount] = m_cpuProcessorCount;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_ThreadCount] = m_cpuThreadCount;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_BaseFrequency] = m_cpuBaseFrequency;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_MaxFrequency] = m_cpuMaxFrequency;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_L1CacheSize] = m_cpuL1CacheSize;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_L2CacheSize] = m_cpuL2CacheSize;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_L3CacheSize] = m_cpuL3CacheSize;
}

#ifdef _WIN32
void CpuInfo::readSystemInfo()
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
    m_cpuBrand = CPUBrandString;
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

    m_cpuProcessorCount = processorCoreCount;
    m_cpuThreadCount = logicalProcessorCount;
    m_cpuL1CacheSize = processorL1CacheSize/1024;
    m_cpuL2CacheSize = processorL2CacheSize/1024;
    m_cpuL3CacheSize = processorL3CacheSize/1024;
}
#else
void CpuInfo::readStaticInfoLinux()
{

}
#endif


void CpuInfo::readDynamicInfo()
{
//    PdhOpenQuery(NULL, NULL, &cpuQuery);
//    // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
//    PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
//    PdhCollectQueryData(cpuQuery);

#ifdef _WIN32
    readDynamicInfoWindows();
#else
    readDynamicInfoLinux();
#endif

    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_TotalUsage] = m_cpuTotalUsage;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CoreUsages] = QVariant::fromValue(m_cpuCoreUsages);
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CoreFrequencies] = QVariant::fromValue(m_cpuCoreFrequencies);
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CurrentMaxFrequency] = m_cpuCurrentMaxFrequency;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CoreVoltage] = m_cpuCoreVoltage;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Power] = m_cpuPower;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_SocPower] = m_cpuSocPower;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Temperature] = m_cpuTemperature;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Fanspeed] = m_cpuFanSpeed;
}

#ifdef _WIN32
void CpuInfo::readDynamicInfoWindows()
{
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(totalCPUQuery);
    PdhGetFormattedCounterValue(totalCPUCounter, PDH_FMT_DOUBLE, NULL, &counterVal);
    m_cpuTotalUsage = counterVal.doubleValue;

    for(uint8_t i=0;i< m_cpuProcessorCount;++i)
    {
        PdhCollectQueryData(singleCPUQueries[i]);
        PdhGetFormattedCounterValue(singleCPUCounters[i], PDH_FMT_DOUBLE, NULL, &counterVal);
        m_cpuCoreUsages[i] = counterVal.doubleValue;
    }
}
#else
void CpuInfo::readDynamicInfoLinux()
{

}
#endif

void CpuInfo::readPdhBaseFrequency()
{
    // Get CPU frequency, scaled to MHz.
    if (m_cpuFreqCounter && PdhCollectQueryData(m_cpuQueryFreq) == ERROR_SUCCESS)
    {
        PDH_RAW_COUNTER cnt;
        DWORD cntType;
        if (PdhGetRawCounterValue(m_cpuFreqCounter, &cntType, &cnt) == ERROR_SUCCESS &&
            (cnt.CStatus == PDH_CSTATUS_VALID_DATA || cnt.CStatus == PDH_CSTATUS_NEW_DATA))
        {
            m_cpuBaseFrequency = (cnt.FirstValue);
            m_cpuMaxFrequency = m_cpuBaseFrequency;
        }
    }
}

void CpuInfo::readPdhFrequency()
{
    if (m_cpuPerformanceCounter && PdhCollectQueryData(m_cpuQueryPerformance) == ERROR_SUCCESS)
    {
        PDH_FMT_COUNTERVALUE cnt;
        DWORD cntType;

        if (PdhGetFormattedCounterValue(m_cpuPerformanceCounter, PDH_FMT_DOUBLE, &cntType, &cnt) == ERROR_SUCCESS &&
            (cnt.CStatus == PDH_CSTATUS_VALID_DATA || cnt.CStatus == PDH_CSTATUS_NEW_DATA))
        {
            m_cpuCurrentMaxFrequency = m_cpuBaseFrequency * (cnt.doubleValue / 100.0);
        }
    }
}