#include "CPUInfo.h"

#include <intrin.h>

CPUInfo::CPUInfo()
{
}

void CPUInfo::init()
{
    fetchStaticInfo();

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
        dynamicInfo.singleCoreLoads.push_back(0.0);
    }
}

void CPUInfo::update()
{
    fetchDynamicInfo();
}

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

void CPUInfo::fetchStaticInfo()
{
    int cpuInfo[4] = {-1};
    unsigned nExIds, i =  0;
    char CPUBrandString[0x40];
    // Get the information associated with each extended ID.
    __cpuid(cpuInfo, 0x80000000);
    nExIds = cpuInfo[0];
    for (i=0x80000000; i<=nExIds; ++i)
    {
        __cpuid(cpuInfo, i);
        // Interpret CPU brand string
        if  (i == 0x80000002)
        {
            memcpy(CPUBrandString, cpuInfo, sizeof(cpuInfo));
        }
        else if  (i == 0x80000003)
        {
            memcpy(CPUBrandString + 16, cpuInfo, sizeof(cpuInfo));
        }
        else if  (i == 0x80000004)
        {
            memcpy(CPUBrandString + 32, cpuInfo, sizeof(cpuInfo));
        }
    }

    cpuInfo[0] = 0;
    cpuInfo[1] = 0;
    cpuInfo[2] = 0;
    cpuInfo[3] = 0;

    __cpuid(cpuInfo, 0);
    if (cpuInfo[0] >= 0x16)
    {
        __cpuid(cpuInfo, 0x16);
        //qDebug() << "EAX: 0x%08x EBX: 0x%08x ECX: %08x\r" << cpuInfo[0] << cpuInfo[1] << cpuInfo[2];
        //qDebug() << "Processor Base Frequency:  %04d MHz\r" << cpuInfo[0];
        //qDebug() << "Maximum Frequency:         %04d MHz\r" << cpuInfo[1];
        //qDebug() << "Bus (Reference) Frequency: %04d MHz\r" << cpuInfo[2];
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
}

void CPUInfo::fetchDynamicInfo()
{
//    PdhOpenQuery(NULL, NULL, &cpuQuery);
//    // You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
//    PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
//    PdhCollectQueryData(cpuQuery);

    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(totalCPUQuery);
    PdhGetFormattedCounterValue(totalCPUCounter, PDH_FMT_DOUBLE, NULL, &counterVal);
    dynamicInfo.cpuTotalLoad = counterVal.doubleValue;

    for(uint8_t i=0;i<staticInfo.processorCount;++i)
    {
        PdhCollectQueryData(singleCPUQueries[i]);
        PdhGetFormattedCounterValue(singleCPUCounters[i], PDH_FMT_DOUBLE, NULL, &counterVal);
        dynamicInfo.singleCoreLoads[i] = counterVal.doubleValue;
    }
}

const CPUInfo::CPU_StaticInfo &CPUInfo::getStaticInfo() const
{
    return staticInfo;
}

const CPUInfo::CPU_DynamicInfo &CPUInfo::getDynamicInfo() const
{
    return dynamicInfo;
}
