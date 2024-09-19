#include "CpuInfoWindows.h"

#include "../../Globals.h"

#include <QDebug>

#ifdef _WIN32

#include <PowrProf.h>
#pragma comment(lib, "Powrprof.lib")
#include <PdhMsg.h>
#include <intrin.h>

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

// Helper function to count set bits in the processor mask.
DWORD CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest) ? 1 : 0);
        bitTest /= 2;
    }

    return bitSetCount;
}

#endif

CpuInfoWindows::CpuInfoWindows()
{
    qDebug() << __FUNCTION__;

    for (uint8_t key = Globals::Key_Cpu_Static_Start + 1; key < Globals::Key_Cpu_Static_End; ++key)
    {
        m_staticInfo[key] = Globals::SysInfo_Uninitialized;
    }
    for (uint8_t key = Globals::Key_Cpu_Dynamic_Start + 1; key < Globals::Key_Cpu_Dynamic_End; ++key)
    {
        m_dynamicInfo[key] = Globals::SysInfo_Uninitialized;
    }
}

CpuInfoWindows::~CpuInfoWindows()
{
    qDebug() << __FUNCTION__;
}

const QMap<uint8_t, QVariant>& CpuInfoWindows::staticInfo() const
{
    return m_staticInfo;
}

const QMap<uint8_t, QVariant>& CpuInfoWindows::dynamicInfo() const
{
    return m_dynamicInfo;
}

void CpuInfoWindows::init()
{
    for (uint8_t i = Globals::Key_Cpu_Static_Start + 1; i < Globals::Key_Cpu_Static_End; ++i)
    {
        m_staticInfo[i] = Globals::SysInfo_Uninitialized;
    }

    for (uint8_t i = Globals::Key_Cpu_Dynamic_Start + 1; i < Globals::Key_Cpu_Dynamic_End; ++i)
    {
        m_dynamicInfo[i] = Globals::SysInfo_Uninitialized;
    }

    readCpuIdBrand();
}

void CpuInfoWindows::readStaticInfo()
{
    readLogicalProcessorInfo();

    initPdh();
    readPdhBaseFrequency();

    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Brand] = QString::fromStdString(m_cpuBrand);
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_Socket] = Globals::SysInfo_Uninitialized;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_CoreCount] = m_cpuCoreCount;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_ThreadCount] = m_cpuThreadCount;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_BaseFrequency] = m_cpuBaseFrequency;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_L1CacheSize] = m_cpuL1CacheSize;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_L2CacheSize] = m_cpuL2CacheSize;
    m_staticInfo[Globals::SysInfoAttr::Key_Cpu_L3CacheSize] = m_cpuL3CacheSize;
}

void CpuInfoWindows::readDynamicInfo()
{
    readPdhCoreUsage();
    readPdhCurrentMaxFrequency();

    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_TotalUsage] = m_cpuTotalUsage;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CoreUsages] = QVariant::fromValue(m_cpuCoreUsages);
    m_dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CurrentMaxFrequency] = m_cpuCurrentMaxFrequency;
}

void CpuInfoWindows::readCpuIdBrand()
{
#ifdef _WIN32
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
#endif 
}

void CpuInfoWindows::readLogicalProcessorInfo()
{
#ifdef _WIN32
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

    m_cpuCoreCount = processorCoreCount;
    m_cpuThreadCount = logicalProcessorCount;
    m_cpuL1CacheSize = processorL1CacheSize/1024;
    m_cpuL2CacheSize = processorL2CacheSize/1024;
    m_cpuL3CacheSize = processorL3CacheSize/1024;
#endif
}

void CpuInfoWindows::initPdh()
{
    //todo: necessary to split? move this to function.
#ifdef _WIN32
    PdhOpenQuery(NULL, 0, &m_pdhTotalCpuQuery);
    std::string text = "\\Processor(_Total)\\% Processor Time";
    std::wstring ws = std::wstring(text.begin(), text.end());
    LPCWSTR sw = ws.c_str();
    // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
    PdhAddEnglishCounter(m_pdhTotalCpuQuery, sw, 0, &m_pdhTotalCpuCounter);
    PdhCollectQueryData(m_pdhTotalCpuQuery);

    for (uint8_t i = 0; i < m_cpuCoreCount; ++i)
    {
        PDH_HQUERY singleCpuQuery;
        PDH_HCOUNTER singleCpuCounter;
        text = "\\Processor(" + std::to_string(i) + ")\\% Processor Time";
        ws = std::wstring(text.begin(), text.end());
        sw = ws.c_str();

        PdhOpenQuery(NULL, 0, &singleCpuQuery);
        // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
        PdhAddEnglishCounter(singleCpuQuery, sw, 0, &singleCpuCounter);
        PdhCollectQueryData(singleCpuQuery);

        m_pdhSingleCpuQueries.push_back(singleCpuQuery);
        m_pdhSingleCpuCounters.push_back(singleCpuCounter);
    }

    m_cpuCoreUsages.resize(m_cpuCoreCount);

    if (PdhOpenQueryW(nullptr, 0, &m_pdhCpuQueryFreq) == ERROR_SUCCESS)
    {
        if (PdhAddEnglishCounterW(m_pdhCpuQueryFreq, L"\\Processor Information(_Total)\\Processor Frequency",
            0, &m_pdhCpuFreqCounter) != ERROR_SUCCESS)
        {
            m_pdhCpuFreqCounter = nullptr;
        }
    }
    else
    {
        m_pdhCpuQueryFreq = nullptr;
    }

    if (PdhOpenQueryW(nullptr, 0, &m_pdhCpuQueryPerformance) == ERROR_SUCCESS)
    {
        if (PdhAddEnglishCounterW(m_pdhCpuQueryPerformance, L"\\Processor Information(_Total)\\% Processor Performance",
            0, &m_pdhCpuPerformanceCounter) != ERROR_SUCCESS)
        {
            m_pdhCpuPerformanceCounter = nullptr;
        }
    }
    else
    {
        m_pdhCpuQueryPerformance = nullptr;
    }
#endif
}

void CpuInfoWindows::readPdhBaseFrequency()
{
#ifdef _WIN32
    // Get CPU frequency, scaled to MHz.
    if (m_pdhCpuFreqCounter && PdhCollectQueryData(m_pdhCpuQueryFreq) == ERROR_SUCCESS)
    {
        PDH_RAW_COUNTER cnt;
        DWORD cntType;
        if (PdhGetRawCounterValue(m_pdhCpuFreqCounter, &cntType, &cnt) == ERROR_SUCCESS &&
            (cnt.CStatus == PDH_CSTATUS_VALID_DATA || cnt.CStatus == PDH_CSTATUS_NEW_DATA))
        {
            m_cpuBaseFrequency = (cnt.FirstValue);
        }
    }
#endif
}

void CpuInfoWindows::readPdhCoreUsage()
{
#ifdef _WIN32
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(m_pdhTotalCpuQuery);
    PdhGetFormattedCounterValue(m_pdhTotalCpuCounter, PDH_FMT_DOUBLE, NULL, &counterVal);
    m_cpuTotalUsage = counterVal.doubleValue;

    for(uint8_t i=0;i< m_cpuCoreCount;++i)
    {
        PdhCollectQueryData(m_pdhSingleCpuQueries[i]);
        PdhGetFormattedCounterValue(m_pdhSingleCpuCounters[i], PDH_FMT_DOUBLE, NULL, &counterVal);
        m_cpuCoreUsages[i] = counterVal.doubleValue;
    }
#endif
}

void CpuInfoWindows::readPdhCurrentMaxFrequency()
{
#ifdef _WIN32
    if (m_pdhCpuPerformanceCounter && PdhCollectQueryData(m_pdhCpuQueryPerformance) == ERROR_SUCCESS)
    {
        PDH_FMT_COUNTERVALUE cnt;
        DWORD cntType;

        if (PdhGetFormattedCounterValue(m_pdhCpuPerformanceCounter, PDH_FMT_DOUBLE, &cntType, &cnt) == ERROR_SUCCESS &&
            (cnt.CStatus == PDH_CSTATUS_VALID_DATA || cnt.CStatus == PDH_CSTATUS_NEW_DATA))
        {
            m_cpuCurrentMaxFrequency = m_cpuBaseFrequency * (cnt.doubleValue / 100.0);
        }
    }
#endif
}
