#include "RyzenMasterSdkHandler.h"

#include "../../Globals.h"

#include <QDebug>

#ifdef HAS_RYZEN_MASTER_SDK
#include "external/RyzenMasterMonitoringSDK/include/ICPUEx.h"
#include "external/RyzenMasterMonitoringSDK/include/IPlatform.h"
#include "external/RyzenMasterMonitoringSDK/include/IDeviceManager.h"
#include "external/RyzenMasterMonitoringSDK/include/IBIOSEx.h"
#endif

RyzenMasterSdkHandler::RyzenMasterSdkHandler()
{
    qDebug() << __FUNCTION__;
}

RyzenMasterSdkHandler::~RyzenMasterSdkHandler()
{
    qDebug() << __FUNCTION__;
}

bool RyzenMasterSdkHandler::init()
{
#ifdef HAS_RYZEN_MASTER_SDK
    bool bRetCode = false;
    IPlatform& rPlatform = GetPlatform();
    bRetCode = rPlatform.Init();
    if (!bRetCode)
    {
        qWarning() << __FUNCTION__ " : " << "Platform init failed.";
        return false;
    }

    IDeviceManager& rDeviceManager = rPlatform.GetIDeviceManager();
    m_cpuDevice = (ICPUEx*)rDeviceManager.GetDevice(dtCPU, 0);
    m_cpuBiosDevice = (IBIOSEx*)rDeviceManager.GetDevice(dtBIOS, 0);

    if (!m_cpuDevice)
    {
        qWarning() << __FUNCTION__ " : " << "Could not init amd devices.";
    }
    else
    {
        m_initialized = true;
    }
#endif

    m_functionsSupportStatus["IPlatform::GetPlatform"] = m_initialized;
    m_functionsSupportStatus["IDeviceManager::GetIDeviceManager"] = m_initialized;
    m_functionsSupportStatus["IDeviceManager::GetDevice"] = m_initialized;

    return m_initialized;
}

void RyzenMasterSdkHandler::readStaticInfo()
{
    m_functionsSupportStatus["IPlatform::GetPlatform"] = false;
    m_functionsSupportStatus["IDeviceManager::GetIDeviceManager"] = false;
    m_functionsSupportStatus["IDeviceManager::GetDevice"] = false;

    m_functionsSupportStatus["IBIOSEx::GetVersion"] = false;
    m_functionsSupportStatus["IBIOSEx::GetVendor"] = false;
    m_functionsSupportStatus["IBIOSEx::GetDate"] = false;
    m_functionsSupportStatus["ICPUEx::GetName"] = false;
    m_functionsSupportStatus["ICPUEx::GetDescription"] = false;
    m_functionsSupportStatus["ICPUEx::GetVendor"] = false;
    m_functionsSupportStatus["ICPUEx::GetRole"] = false;
    m_functionsSupportStatus["ICPUEx::GetClassName"] = false;
    m_functionsSupportStatus["ICPUEx::GetType"] = false;
    m_functionsSupportStatus["ICPUEx::GetIndex"] = false;
    m_functionsSupportStatus["ICPUEx::GetL1DataCache"] = false;
    m_functionsSupportStatus["ICPUEx::GetL1InstructionCache"] = false;
    m_functionsSupportStatus["ICPUEx::GetL2Cache"] = false;
    m_functionsSupportStatus["ICPUEx::GetL3Cache"] = false;
    m_functionsSupportStatus["ICPUEx::GetCorePark"] = false;
    m_functionsSupportStatus["ICPUEx::GetPackage"] = false;
    m_functionsSupportStatus["ICPUEx::GetChipsetName"] = false;
    m_functionsSupportStatus["ICPUEx::GetCPUParameters"] = false;

#ifdef HAS_RYZEN_MASTER_SDK
    int iRet;

    if (m_cpuBiosDevice)
    {
        const wchar_t* biosVersion = m_cpuBiosDevice->GetVersion();
        if (biosVersion)
        {
            m_cpuBiosVersion = QString::fromWCharArray(biosVersion).toStdString();
            m_functionsSupportStatus["IBIOSEx::GetVersion"] = true;
        }

        const wchar_t* biosVendor = m_cpuBiosDevice->GetVendor();
        if (biosVendor)
        {
            m_functionsSupportStatus["IBIOSEx::GetVendor"] = true;
        }

        const wchar_t* biosDate = m_cpuBiosDevice->GetVersion();
        if (biosDate)
        {
            m_cpuBiosDate = QString::fromWCharArray(biosDate).toStdString();
            m_functionsSupportStatus["IBIOSEx::GetVersion"] = true;
        }       
    }

    if (m_cpuDevice)
    {
        const wchar_t* cpuName = m_cpuDevice->GetName();
        if (cpuName)
        {
            m_cpuBrand = QString::fromWCharArray(cpuName).toStdString();
            m_functionsSupportStatus["ICPUEx::GetName"] = true;
        }

        const wchar_t* cpuDescription = m_cpuDevice->GetDescription();
        if (cpuDescription)
        {
            m_cpuDescription = QString::fromWCharArray(cpuDescription).toStdString();
            m_functionsSupportStatus["ICPUEx::GetDescription"] = true;
        }

        const wchar_t* cpuVendor = m_cpuDevice->GetVendor();
        if (cpuVendor)
        {
            m_cpuVendor = QString::fromWCharArray(cpuVendor).toStdString();
            m_functionsSupportStatus["ICPUEx::GetVendor"] = true;
        }

        const wchar_t* cpuRole = m_cpuDevice->GetRole();
        if (cpuRole)
        {
            m_functionsSupportStatus["ICPUEx::GetRole"] = true;
        }

        const wchar_t* cpuClassName = m_cpuDevice->GetClassName();
        if (cpuClassName)
        {
            m_functionsSupportStatus["ICPUEx::GetClassName"] = true;
        }

        const AOD_DEVICE_TYPE deviceType = m_cpuDevice->GetType();

        const unsigned long cpuIndex = m_cpuDevice->GetIndex();

        CACHE_INFO l1DataCache;
        iRet = m_cpuDevice->GetL1DataCache(l1DataCache);
        if (iRet == 0)
        {
            m_cpuL1CacheSize = l1DataCache.fSize;
            m_functionsSupportStatus["ICPUEx::GetL1DataCache"] = true;
        }

        CACHE_INFO l1InstructionCache;
        iRet = m_cpuDevice->GetL1InstructionCache(l1InstructionCache);
        if (iRet == 0)
        {
            m_functionsSupportStatus["ICPUEx::GetL1InstructionCache"] = true;
        }

        CACHE_INFO l2Cache;
        iRet = m_cpuDevice->GetL2Cache(l2Cache);
        if (iRet == 0)
        {
            m_cpuL2CacheSize = l2Cache.fSize;
            m_functionsSupportStatus["ICPUEx::GetL2Cache"] = true;
        }

        CACHE_INFO l3Cache;
        iRet = m_cpuDevice->GetL3Cache(l3Cache);
        if (iRet == 0)
        {
            m_cpuL3CacheSize = l3Cache.fSize;
            m_functionsSupportStatus["ICPUEx::GetL3Cache"] = true;
        }

        unsigned int coreCount{ 0 };
        iRet = m_cpuDevice->GetCoreCount(coreCount);
        if (iRet == 0)
        {
            m_cpuProcessorCount = coreCount;
            m_functionsSupportStatus["ICPUEx::GetCoreCount"] = true;
            qDebug() << "CpuInfo::readStaticInfoRyzenMaster(): ICPUEx::GetCoreCount(): " << coreCount;
        }

        unsigned int corePark{ 0 };
        iRet = m_cpuDevice->GetCorePark(corePark);
        if (iRet == 0)
        {
            m_functionsSupportStatus["ICPUEx::GetCorePark"] = true;
        }

        const wchar_t* cpuPackage = m_cpuDevice->GetPackage();
        if (cpuPackage)
        {
            m_cpuSocket = QString::fromWCharArray(cpuPackage).toStdString();
            m_functionsSupportStatus["ICPUEx::GetPackage"] = true;
        }

        std::wstring str;
        iRet = m_cpuDevice->GetChipsetName(str.data());
        if (iRet == 0)
        {
            m_functionsSupportStatus["ICPUEx::GetChipsetName"] = true;
        }

        CPUParameters stData;
        int iRet = m_cpuDevice->GetCPUParameters(stData);
        if (iRet == 0)
        {
            m_cpuBaseFrequency = stData.fCCLK_Fmax;
            m_cpuMaxTurboFrequency = stData.dPeakSpeed;
            m_functionsSupportStatus["ICPUEx::GetCPUParameters"] = true;
        }
    }
#endif
}

void RyzenMasterSdkHandler::readDynamicInfo()
{
    if (!m_initialized)
    {
        return;
    }

    readCpuInfo();
}

//@note: from https://www.amd.com/de/developer/ryzen-master-monitoring-sdk.html
//The API call(GetCPUParameters) included in this SDK should only be called once per second to avoid impacting the load on the SMU.Calls made faster may impact the results.
void RyzenMasterSdkHandler::readCpuInfo()
{
#ifdef HAS_RYZEN_MASTER_SDK
    if (m_functionsSupportStatus["ICPUEx::GetCPUParameters"])
    {
        CPUParameters stData;
        int iRet = m_cpuDevice->GetCPUParameters(stData);
        if (!iRet)
        {
            m_cpuTemperature = stData.dTemperature;
            m_cpuPower = stData.fPPTValue;
            m_cpuSocPower = stData.fVDDCR_SOC_Power;
            m_cpuCoreVoltage = stData.dAvgCoreVoltage;
            m_cpuCurrentMaxFrequency = stData.dPeakSpeed;

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

void RyzenMasterSdkHandler::readBiosInfo()
{ 
#ifdef HAS_RYZEN_MASTER_SDK
    if (m_cpuBiosDevice)
    {
        int iRet;
        unsigned short memVDDIO;
        iRet = m_cpuBiosDevice->GetMemVDDIO(memVDDIO);
        if (!iRet)
        {
            qDebug() << "RyzenMasterSdkHandler::readRyzenDynamicBiosInfo(): IBIOSEx::GetMemVDDIO(): " << memVDDIO;
        }

        unsigned short currentMemClock;
        iRet = m_cpuBiosDevice->GetCurrentMemClock(currentMemClock);
        if (!iRet)
        {
            qDebug() << "RyzenMasterSdkHandler::readRyzenDynamicBiosInfo(): IBIOSEx::GetCurrentMemClock(): " << currentMemClock;
        }

        unsigned char memCtrlTcl;
        iRet = m_cpuBiosDevice->GetMemCtrlTcl(memCtrlTcl);
        if (!iRet)
        {
            qDebug() << "RyzenMasterSdkHandler::readRyzenDynamicBiosInfo(): IBIOSEx::GetMemCtrlTcl(): " << memCtrlTcl;
        }

        unsigned char memCtrlTrcdrd;
        iRet = m_cpuBiosDevice->GetMemCtrlTrcdrd(memCtrlTrcdrd);
        if (!iRet)
        {
            qDebug() << "RyzenMasterSdkHandler::readRyzenDynamicBiosInfo(): IBIOSEx::GetMemCtrlTrcdrd(): " << memCtrlTrcdrd;
        }

        unsigned char memCtrlTras;
        iRet = m_cpuBiosDevice->GetMemCtrlTras(memCtrlTras);
        if (!iRet)
        {
            qDebug() << "RyzenMasterSdkHandler::readRyzenDynamicBiosInfo(): IBIOSEx::GetMemCtrlTras(): " << memCtrlTras;
        }

        unsigned char memCtrlTrp;
        iRet = m_cpuBiosDevice->GetMemCtrlTrp(memCtrlTrp);
        if (!iRet)
        {
            qDebug() << "RyzenMasterSdkHandler::readRyzenDynamicBiosInfo(): IBIOSEx::GetMemCtrlTrp(): " << memCtrlTrp;
        }
    }
#endif
}