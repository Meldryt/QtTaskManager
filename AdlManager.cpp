
#include "AdlManager.h"
#include <string>
#include <iostream>

#include <QDebug>

//Memory allocating callback for ADL
void* __stdcall ADL_Main_Memory_Alloc(int iSize)
{
	void* lpBuffer = malloc(iSize);
	return lpBuffer;
}

// Optional Memory de-allocation function
void __stdcall ADL_Main_Memory_Free(void* lpBuffer)
{
	if (NULL != lpBuffer)
	{
		free(lpBuffer);
		lpBuffer = NULL;
	}
}

//Memory freeing callback for ADL
void __stdcall ADL_Main_Memory_Free(void** lpBuffer_)
{
	if (NULL != lpBuffer_ && NULL != *lpBuffer_)
	{
		free(*lpBuffer_);
		*lpBuffer_ = NULL;
	}
}

#if defined (LINUX)
// equivalent functions in linux
void* GetProcAddress(void* pLibrary, const char* name)
{
	return dlsym(pLibrary, name);
}

#endif

AdlManager::AdlManager()
{

}

AdlManager::~AdlManager()
{
	destroyAdl();
}

bool AdlManager::init()
{
	if (!loadLibrary())
	{
		return false;
	}

	setupMainControl();
	setupAdapter();
	setupDisplay();
	setupGraphics();
	setupDesktop();
	setupOthers();

	// Initialize legacy ADL transaction.Note that applications still can mix ADL and ADL2 API providing that only single 
	// transaction that uses legacy ADL APIs exists at any given time in the process. Numer of ADL2 transactions is not limited.  
	// The second parameter is 1, which means:
	// retrieve adapter information only for adapters that are physically present and enabled in the system
	if (ADL2_Main_Control_Create(ADL_Main_Memory_Alloc, 1, &m_context) != ADL_OK)
	{
		qWarning() << "ADL Initialization Error!";
		return false;
	}

	setupOverdrive();

	return true;
}

bool AdlManager::fetchStaticInfo()
{
	readAdapterInfo();
	readMemoryInfo();
	readGraphicsDriverInfo();
	readBiosInfo();

	return true;
}

bool AdlManager::fetchDynamicInfo()
{
	readOverdriveInfo();

	return true;
}

bool AdlManager::loadLibrary()
{
	//SetDllDirectoryW(L"..\\lib");

#if defined (LINUX)
	m_hDLL = dlopen("libatiadlxx.so", RTLD_LAZY | RTLD_GLOBAL);
#else
	m_hDLL = LoadLibraryA("atiadlxx.dll");
	if (m_hDLL == NULL)
		// A 32 bit calling application on 64 bit OS will fail to LoadLIbrary.
		// Try to load the 32 bit library (atiadlxy.dll) instead
		m_hDLL = LoadLibraryA("atiadlxy.dll");
#endif

	if (NULL == m_hDLL)
	{
		qWarning() << "ADL library not found!";
		return false;
	}

	return true;
}

//Initialize ADL. Retrieves and initialize ADL API pointers.
//Returns false if ADL initialization failed or some of the expected ADL APIs can't be found
bool AdlManager::setupMainControl()
{
	ADL2_Main_Control_Create = (ADL2_MAIN_CONTROL_CREATE)GetProcAddress(m_hDLL, "ADL2_Main_Control_Create");
	ADL2_Main_Control_Destroy = (ADL2_MAIN_CONTROL_DESTROY)GetProcAddress(m_hDLL, "ADL2_Main_Control_Destroy");

	if (!ADL2_Main_Control_Create ||
		!ADL2_Main_Control_Destroy)
	{
		qWarning() << "ADL's API is missing!";
		return false;
	}

	return true;
}

bool AdlManager::setupAdapter()
{
	ADL2_Adapter_NumberOfAdapters_Get = (ADL2_ADAPTER_NUMBEROFADAPTERS_GET)GetProcAddress(m_hDLL, "ADL2_Adapter_NumberOfAdapters_Get");
	ADL2_Adapter_Active_Get = (ADL2_ADAPTER_ACTIVE_GET)GetProcAddress(m_hDLL, "ADL2_Adapter_Active_Get");
	ADL2_Adapter_Primary_Get = (ADL2_ADAPTER_PRIMARY_GET)GetProcAddress(m_hDLL, "ADL2_Adapter_Primary_Get");
	ADL2_Adapter_AdapterInfo_Get = (ADL2_ADAPTER_ADAPTERINFO_GET)GetProcAddress(m_hDLL, "ADL2_Adapter_AdapterInfo_Get");
	ADL2_Adapter_MemoryInfo3_Get = (ADL2_ADAPTER_MEMORYINFO3_GET)GetProcAddress(m_hDLL, "ADL2_Adapter_MemoryInfo3_Get");
	ADL2_Adapter_VideoBiosInfo_Get = (ADL2_ADAPTER_VIDEOBIOSINFO_GET)GetProcAddress(m_hDLL, "ADL2_Adapter_VideoBiosInfo_Get");

	ADL2_Adapter_PMLog_Support_Get = (ADL2_ADAPTER_PMLOG_SUPPORT_GET)GetProcAddress(m_hDLL, "ADL2_Adapter_PMLog_Support_Get");
	ADL2_Adapter_PMLog_Start = (ADL2_ADAPTER_PMLOG_START)GetProcAddress(m_hDLL, "ADL2_Adapter_PMLog_Start");
	ADL2_Adapter_PMLog_Stop = (ADL2_ADAPTER_PMLOG_STOP)GetProcAddress(m_hDLL, "ADL2_Adapter_PMLog_Stop");

    ADL2_Adapter_RegValueInt_Get = (ADL2_ADAPTER_REGVALUEINT_GET)GetProcAddress(m_hDLL, "ADL2_Adapter_RegValueInt_Get");

	if (!ADL2_Adapter_NumberOfAdapters_Get ||
		!ADL2_Adapter_Active_Get ||
		!ADL2_Adapter_Primary_Get ||
		!ADL2_Adapter_AdapterInfo_Get ||
		!ADL2_Adapter_MemoryInfo3_Get ||
		!ADL2_Adapter_VideoBiosInfo_Get ||
		!ADL2_Adapter_PMLog_Support_Get ||
		!ADL2_Adapter_PMLog_Start ||
        !ADL2_Adapter_PMLog_Stop ||
        !ADL2_Adapter_RegValueInt_Get)
	{
		qWarning() << "ADL's API is missing!";
        return false;
	}

    return true;
}

bool AdlManager::setupDisplay()
{
	ADL2_Display_Modes_Get = (ADL2_DISPLAY_MODES_GET)GetProcAddress(m_hDLL, "ADL2_Display_Modes_Get");
	ADL2_Display_DisplayInfo_Get = (ADL2_DISPLAY_DISPLAYINFO_GET)GetProcAddress(m_hDLL, "ADL2_Display_DisplayInfo_Get");

	ADL2_Display_Property_Get = (ADL2_DISPLAY_PROPERTY_GET)GetProcAddress(m_hDLL, "ADL2_Display_Property_Get");
	ADL2_Display_DCE_Get = (ADL2_DISPLAY_DCE_GET)GetProcAddress(m_hDLL, "ADL2_Display_DCE_Get");

	if (!ADL2_Display_Modes_Get ||
		!ADL2_Display_DisplayInfo_Get ||
		!ADL2_Display_Property_Get ||
		!ADL2_Display_DCE_Get)
	{
		qWarning() << "ADL's API is missing!";
        return false;
	}

    return true;
}

bool AdlManager::setupGraphics()
{
	ADL2_Graphics_VersionsX2_Get = (ADL2_GRAPHICS_VERSIONSX2_GET)GetProcAddress(m_hDLL, "ADL2_Graphics_VersionsX2_Get");

	if (!ADL2_Graphics_VersionsX2_Get)
	{
		qWarning() << "ADL's API is missing!";
		return false;
	}

	return true;
}

bool AdlManager::setupOverdrive()
{
	ADL_Overdrive_Caps = (ADL_OVERDRIVE_CAPS)GetProcAddress(m_hDLL, "ADL_Overdrive_Caps");
	ADL2_Overdrive_Caps = (ADL2_OVERDRIVE_CAPS)GetProcAddress(m_hDLL, "ADL2_Overdrive_Caps");

	if (!ADL_Overdrive_Caps ||
		!ADL2_Overdrive_Caps)
	{
		qWarning() << "ADL's API is missing!";
		return false;
	}

	readOverdriveVersion();

	if (m_adlOverdriveVersion == 1)
	{
		if (m_overdriveVersion == 5)
		{
			ADL_Overdrive5_ThermalDevices_Enum = (ADL_OVERDRIVE5_THERMALDEVICES_ENUM)GetProcAddress(m_hDLL, "ADL_Overdrive5_ThermalDevices_Enum");
			ADL_Overdrive5_ODParameters_Get = (ADL_OVERDRIVE5_ODPARAMETERS_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_ODParameters_Get");
			ADL_Overdrive5_Temperature_Get = (ADL_OVERDRIVE5_TEMPERATURE_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_Temperature_Get");
			ADL_Overdrive5_FanSpeed_Get = (ADL_OVERDRIVE5_FANSPEED_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_FanSpeed_Get");
			ADL_Overdrive5_FanSpeedInfo_Get = (ADL_OVERDRIVE5_FANSPEEDINFO_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_FanSpeedInfo_Get");
			ADL_Overdrive5_ODPerformanceLevels_Get = (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_ODPerformanceLevels_Get");
			ADL_Overdrive5_CurrentActivity_Get = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_CurrentActivity_Get");
			ADL_Overdrive5_PowerControl_Caps = (ADL_OVERDRIVE5_POWERCONTROL_CAPS)GetProcAddress(m_hDLL, "ADL_Overdrive5_PowerControl_Caps");
			ADL_Overdrive5_PowerControlInfo_Get = (ADL_OVERDRIVE5_POWERCONTROLINFO_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_PowerControlInfo_Get");
			ADL_Overdrive5_PowerControl_Get = (ADL_OVERDRIVE5_POWERCONTROL_GET)GetProcAddress(m_hDLL, "ADL_Overdrive5_PowerControl_Get");

			if (!ADL_Overdrive5_ThermalDevices_Enum ||
				!ADL_Overdrive5_Temperature_Get ||
				!ADL_Overdrive5_FanSpeedInfo_Get ||
				!ADL_Overdrive5_ODPerformanceLevels_Get ||
				!ADL_Overdrive5_ODParameters_Get ||
				!ADL_Overdrive5_CurrentActivity_Get ||
				!ADL_Overdrive5_PowerControl_Caps ||
				!ADL_Overdrive5_PowerControlInfo_Get ||
				!ADL_Overdrive5_PowerControl_Get)
			{
				qWarning() << "ADL's API is missing!";
				return false;
			}
		}
		else if (m_overdriveVersion == 6)
		{
			ADL_Overdrive6_FanSpeed_Get = (ADL_OVERDRIVE6_FANSPEED_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_FanSpeed_Get");
			ADL_Overdrive6_ThermalController_Caps = (ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS)GetProcAddress(m_hDLL, "ADL_Overdrive6_ThermalController_Caps");
			ADL_Overdrive6_Temperature_Get = (ADL_OVERDRIVE6_TEMPERATURE_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_Temperature_Get");
			ADL_Overdrive6_Capabilities_Get = (ADL_OVERDRIVE6_CAPABILITIES_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_Capabilities_Get");
			ADL_Overdrive6_StateInfo_Get = (ADL_OVERDRIVE6_STATEINFO_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_StateInfo_Get");
			ADL_Overdrive6_CurrentStatus_Get = (ADL_OVERDRIVE6_CURRENTSTATUS_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_CurrentStatus_Get");
			ADL_Overdrive6_PowerControl_Caps = (ADL_OVERDRIVE6_POWERCONTROL_CAPS)GetProcAddress(m_hDLL, "ADL_Overdrive6_PowerControl_Caps");
			ADL_Overdrive6_PowerControlInfo_Get = (ADL_OVERDRIVE6_POWERCONTROLINFO_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_PowerControlInfo_Get");
			ADL_Overdrive6_PowerControl_Get = (ADL_OVERDRIVE6_POWERCONTROL_GET)GetProcAddress(m_hDLL, "ADL_Overdrive6_PowerControl_Get");

			if (!ADL_Overdrive6_FanSpeed_Get ||
				!ADL_Overdrive6_ThermalController_Caps ||
				!ADL_Overdrive6_Temperature_Get ||
				!ADL_Overdrive6_Capabilities_Get ||
				!ADL_Overdrive6_StateInfo_Get ||
				!ADL_Overdrive6_CurrentStatus_Get ||
				!ADL_Overdrive6_PowerControl_Caps ||
				!ADL_Overdrive6_PowerControlInfo_Get ||
				!ADL_Overdrive6_PowerControl_Get)
			{
				qWarning() << "ADL's API is missing!";
				return false;
			}
		}
	}
	else if (m_adlOverdriveVersion == 2)
	{
		ADL2_WS_Overdrive_Caps = (ADL2_WS_OVERDRIVE_CAPS)GetProcAddress(m_hDLL, "ADL2_WS_Overdrive_Caps");

		if (!ADL2_WS_Overdrive_Caps)
		{
			qWarning() << "ADL's API is missing!";
			return false;
		}

		if (m_overdriveVersion == 5)
		{
			ADL2_Overdrive5_ThermalDevices_Enum = (ADL2_OVERDRIVE5_THERMALDEVICES_ENUM)GetProcAddress(m_hDLL, "ADL2_Overdrive5_ThermalDevices_Enum");
			ADL2_Overdrive5_ODParameters_Get = (ADL2_OVERDRIVE5_ODPARAMETERS_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive5_ODParameters_Get");
			ADL2_Overdrive5_Temperature_Get = (ADL2_OVERDRIVE5_TEMPERATURE_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive5_Temperature_Get");
			ADL2_Overdrive5_FanSpeed_Get = (ADL2_OVERDRIVE5_FANSPEED_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive5_FanSpeed_Get");
			ADL2_Overdrive5_FanSpeedInfo_Get = (ADL2_OVERDRIVE5_FANSPEEDINFO_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive5_FanSpeedInfo_Get");
			ADL2_Overdrive5_ODPerformanceLevels_Get = (ADL2_OVERDRIVE5_ODPERFORMANCELEVELS_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive5_ODPerformanceLevels_Get");
			ADL2_Overdrive5_CurrentActivity_Get = (ADL2_OVERDRIVE5_CURRENTACTIVITY_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive5_CurrentActivity_Get");
			ADL2_Overdrive5_PowerControl_Caps = (ADL2_OVERDRIVE5_POWERCONTROL_CAPS)GetProcAddress(m_hDLL, "ADL2_Overdrive5_PowerControl_Caps");
			ADL2_Overdrive5_PowerControlInfo_Get = (ADL2_OVERDRIVE5_POWERCONTROLINFO_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive5_PowerControlInfo_Get");
			ADL2_Overdrive5_PowerControl_Get = (ADL2_OVERDRIVE5_POWERCONTROL_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive5_PowerControl_Get");

			if (!ADL2_Overdrive5_ThermalDevices_Enum ||
				!ADL2_Overdrive5_Temperature_Get ||
				!ADL2_Overdrive5_FanSpeedInfo_Get ||
				!ADL2_Overdrive5_ODPerformanceLevels_Get ||
				!ADL2_Overdrive5_ODParameters_Get ||
				!ADL2_Overdrive5_CurrentActivity_Get ||
				!ADL2_Overdrive5_PowerControl_Caps ||
				!ADL2_Overdrive5_PowerControlInfo_Get ||
				!ADL2_Overdrive5_PowerControl_Get)
			{
				qWarning() << "ADL's API is missing!";
				return false;
			}
        }
        else if (m_overdriveVersion == 6)
        {
        }
        else if (m_overdriveVersion == 8)
		{
            ADL2_Overdrive8_Init_Setting_Get = (ADL2_OVERDRIVE8_INIT_SETTING_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive8_Init_Setting_Get");
			ADL2_Overdrive8_Init_SettingX2_Get = (ADL2_OVERDRIVE8_INIT_SETTINGX2_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive8_Init_SettingX2_Get");
            ADL2_Overdrive8_Current_Setting_Get = (ADL2_OVERDRIVE8_CURRENT_SETTING_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive8_Current_Setting_Get");
			ADL2_Overdrive8_Current_SettingX2_Get = (ADL2_OVERDRIVE8_CURRENT_SETTINGX2_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive8_Current_SettingX2_Get");

            if (!ADL2_Overdrive8_Init_Setting_Get ||
                !ADL2_Overdrive8_Init_SettingX2_Get ||
                !ADL2_Overdrive8_Current_Setting_Get ||
				!ADL2_Overdrive8_Current_SettingX2_Get)
			{
				qWarning() << "ADL's API is missing!";
				return false;
			}
		}
	}

	return true;
}

bool AdlManager::setupDesktop()
{
	ADL2_Desktop_Device_Create = (ADL2_DESKTOP_DEVICE_CREATE)GetProcAddress(m_hDLL, "ADL2_Desktop_Device_Create");
	ADL2_Desktop_Device_Destroy = (ADL2_DESKTOP_DEVICE_DESTROY)GetProcAddress(m_hDLL, "ADL2_Desktop_Device_Destroy");

	if (!ADL2_Desktop_Device_Create ||
		!ADL2_Desktop_Device_Destroy)
	{
		qWarning() << "ADL's API is missing!";
		return false;
	}

	return true;
}

bool AdlManager::setupOthers()
{
    ADL2_New_QueryPMLogData_Get = (ADL2_NEW_QUERYPMLOGDATA_GET)GetProcAddress(m_hDLL, "ADL2_New_QueryPMLogData_Get");

    if (!ADL2_New_QueryPMLogData_Get)
    {
		qWarning() << "ADL's API is missing!";
        return false;
    }

    return true;
}

//Destroy ADL. ALD calls can't be called after the method is invoked;
void AdlManager::destroyAdl()
{
	if (ADL2_Main_Control_Destroy)
	{
		ADL2_Main_Control_Destroy(m_context);
	}

	if (m_hDLL)
	{
		FreeLibrary(m_hDLL);
	}
}

std::string AdlManager::getGpuVramNameFromId(int vramVendorRevId)
{
	switch (vramVendorRevId)
	{
	case ADLvRamVendor_SAMSUNG:
		return "Samsung";
	case ADLvRamVendor_INFINEON:
		return "Infineon";
	case ADLvRamVendor_ELPIDA:
		return "Elpida";
	case ADLvRamVendor_ETRON:
		return "Etron";
	case ADLvRamVendor_NANYA:
		return "Nanya";
	case ADLvRamVendor_HYNIX:
		return "Hynix";
	case ADLvRamVendor_MOSEL:
		return "Mosel";
	case ADLvRamVendor_WINBOND:
		return "Winbond";
	case ADLvRamVendor_ESMT:
		return "Esmt";
	case ADLvRamVendor_MICRON:
		return "Micron";
	default:
		return "Undefine";
	}
}

void AdlManager::readAdapterInfo()
{
	int numberAdapters = 0;

	if (ADL2_Adapter_NumberOfAdapters_Get(m_context, &numberAdapters) == ADL_OK)
	{
		int primary = -1;
		if (ADL2_Adapter_Primary_Get(m_context, &primary))
		{
			AdapterInfo* infos = new AdapterInfo[numberAdapters];
			if (ADL2_Adapter_AdapterInfo_Get(m_context, infos, sizeof(AdapterInfo) * numberAdapters) == ADL_OK)
			{
				for (uint32_t adapterIdx = 0; adapterIdx < static_cast<uint32_t>(numberAdapters); ++adapterIdx)
				{
					int active = 0;
					if (ADL2_Adapter_Active_Get(m_context, adapterIdx, &active) == ADL_OK && active/* && primary == adapterIdx*/)
					{
						m_adapterInfo = infos[adapterIdx];
						m_adapterIndex = m_adapterInfo.iAdapterIndex;
						m_staticInfo.gpuModel = m_adapterInfo.strAdapterName;
						break;
					}
				}
			}
			delete[] infos;
		}
	}
}

void AdlManager::readMemoryInfo()
{
	ADLMemoryInfo3 adlMemInfo3;
	if (ADL2_Adapter_MemoryInfo3_Get(m_context, m_adapterInfo.iAdapterIndex, &adlMemInfo3) == ADL_OK)
	{
		m_staticInfo.memorySize = adlMemInfo3.iMemorySize / 1024 / 1024 / 1000;
		m_staticInfo.memoryType = adlMemInfo3.strMemoryType;
		m_staticInfo.memoryBandwidth = adlMemInfo3.iMemoryBandwidth / 1024;

		if (adlMemInfo3.iVramVendorRevId == ADLvRamVendor_Unsupported == adlMemInfo3.iVramVendorRevId)
		{
            qWarning() << "GPU Vedio RAM vendor ID Unsupport, only support AMD dGPU now.";
		}
		else
		{
			m_staticInfo.memoryVendor = getGpuVramNameFromId(adlMemInfo3.iVramVendorRevId);
		}
	}
}

void AdlManager::readGraphicsDriverInfo()
{
	if (ADL2_Graphics_VersionsX2_Get) 
	{
		ADLVersionsInfoX2 versionsInfo;
		int ADLResult = ADL2_Graphics_VersionsX2_Get(m_context, &versionsInfo);
		if (ADLResult == ADL_OK || ADLResult == ADL_OK_WARNING)
		{
			m_staticInfo.driverInfo = versionsInfo.strDriverVer;

			std::string catalystVersion = std::string(versionsInfo.strCatalystVersion);
			if (catalystVersion.empty())
			{
				m_staticInfo.driverVersion = versionsInfo.strCrimsonVersion;
			}
			else
			{
				m_staticInfo.driverVersion = catalystVersion;
			}	
		}
	}
	else if (ADL2_Graphics_Versions_Get) 
	{
		ADLVersionsInfo versionsInfo;
		int ADLResult = ADL2_Graphics_Versions_Get(m_context, &versionsInfo);
		if (ADLResult == ADL_OK || ADLResult == ADL_OK_WARNING)
		{
			m_staticInfo.driverInfo = versionsInfo.strDriverVer;
			m_staticInfo.driverVersion = versionsInfo.strCatalystVersion;
		}
	}
}

void AdlManager::readBiosInfo()
{
	ADLBiosInfo biosInfo;
	if (ADL2_Adapter_VideoBiosInfo_Get(m_context, m_adapterIndex, &biosInfo) == ADL_OK) {
        //printf("\tBIOS\n"
		//	"\t\tPart#: %s\n"
		//	"\t\tVersion: %s\n"
		//	"\t\tDate: %s\n",
		//	biosInfo.strPartNumber,
		//	biosInfo.strVersion,
		//	biosInfo.strDate
		//);
	}
}

void AdlManager::readOverdriveVersion()
{
	int iOverdriveSupported = 0;
	int iOverdriveEnabled = 0;
	int	iOverdriveVersion = 0;

	if (ADL_Overdrive_Caps(m_adapterIndex, &iOverdriveSupported, &iOverdriveEnabled, &iOverdriveVersion) != ADL_OK)
	{
		if (ADL2_Overdrive_Caps(m_context, m_adapterIndex, &iOverdriveSupported, &iOverdriveEnabled, &iOverdriveVersion) == ADL_OK)
		{
			m_adlOverdriveVersion = 2;
			qDebug() << "Found ADL2_Overdrive_Caps";
		}
		else
		{
			qWarning() << "ADL_Overdrive_Caps and ADL2_Overdrive_Caps failed";
		}
	}
	else
	{
		m_adlOverdriveVersion = 1;
		qDebug() << "Found ADL_Overdrive_Caps";
	}

	if (!iOverdriveSupported)
	{
		qWarning() << "Overdrive is not supported";
	}

	m_overdriveSupported = (iOverdriveSupported == 1);
	m_overdriveEnabled = (iOverdriveEnabled == 1);
	m_overdriveVersion = iOverdriveVersion;
}

void AdlManager::readOverdriveInfo()
{
	if (!m_overdriveSupported)
	{
		return;
	}

	if (m_adlOverdriveVersion == 1)
	{
		if (m_overdriveVersion == 5)
		{
			readAdlOverdrive5Info();
		}
		else if (m_overdriveVersion == 6)
		{
			readAdlOverdrive6Info();
		}
	}
	else if (m_adlOverdriveVersion == 2)
	{
		if (m_overdriveVersion == 5)
		{
			readAdl2Overdrive5Info();
		}
		else if (m_overdriveVersion == 6)
		{
			readAdl2Overdrive6Info();
		}
		else if (m_overdriveVersion == 8)
		{
			readAdl2Overdrive8Info();
		}
	}
}

bool AdlManager::readAdlOverdrive5Info()
{
	readAdlOverdrive5Temperature();
	readAdlOverdrive5FanSpeed();
	readAdlOverdrive5PowerControl();
	readAdlOverdrive5ODParameters();
	readAdlOverdrive5PerformanceLevel();

	return true;
}

#define ADL_WARNING_NO_DATA      -100

bool AdlManager::readAdlOverdrive5Temperature()
{
	ADLThermalControllerInfo termalControllerInfo = { 0 };
	termalControllerInfo.iSize = sizeof(ADLThermalControllerInfo);

	for (int iThermalControllerIndex = 0; iThermalControllerIndex < 10; iThermalControllerIndex++)
	{
		int result = ADL2_Overdrive5_ThermalDevices_Enum(m_context, m_adapterIndex, iThermalControllerIndex, &termalControllerInfo);

		if (result == ADL_WARNING_NO_DATA)
		{
            qWarning() << "Failed to enumerate thermal devices";
			return false;
		}

		if (termalControllerInfo.iThermalDomain == ADL_DL_THERMAL_DOMAIN_GPU)
		{
			ADLTemperature adlTemperature = { 0 };
			adlTemperature.iSize = sizeof(ADLTemperature);
			if (ADL2_Overdrive5_Temperature_Get(m_context, m_adapterIndex, iThermalControllerIndex, &adlTemperature) != ADL_OK)
			{
				qWarning() << "Failed to get thermal devices temperature";
				return false;
			}

			m_thermalDomainControllerIndex = iThermalControllerIndex;
			m_dynamicInfo.gpuTemperature = adlTemperature.iTemperature / 1000; // The temperature is returned in millidegrees Celsius.
			return true;
		}
	}

	return true;
}

bool AdlManager::readAdlOverdrive5FanSpeed()
{
	ADLFanSpeedInfo fanSpeedInfo;
	fanSpeedInfo.iSize;

	if (ADL_Overdrive5_FanSpeedInfo_Get(m_adapterIndex, m_thermalDomainControllerIndex, &fanSpeedInfo) != ADL_OK)
	{
		qWarning() << "Failed to get fan caps";
		return false;
	}

	int fanSpeedReportingMethod = ((fanSpeedInfo.iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_READ) == ADL_DL_FANCTRL_SUPPORTS_RPM_READ) ? ADL_DL_FANCTRL_SPEED_TYPE_RPM : ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;

	ADLFanSpeedValue fanSpeedValue = { 0 };
	
	//Set to ADL_DL_FANCTRL_SPEED_TYPE_RPM or to ADL_DL_FANCTRL_SPEED_TYPE_PERCENT to request fan speed to be returned in rounds per minute or in percentage points.
	//Note that the call might fail if requested fan speed reporting method is not supported by the GPU.
	fanSpeedValue.iSpeedType = fanSpeedReportingMethod;
	if (ADL_Overdrive5_FanSpeed_Get(m_adapterIndex, m_thermalDomainControllerIndex, &fanSpeedValue) != ADL_OK)
	{
		qWarning() << "Failed to get fan speed";
		return false;
	}

	if (fanSpeedReportingMethod == ADL_DL_FANCTRL_SPEED_TYPE_RPM)
	{
		m_dynamicInfo.gpuFanSpeed = fanSpeedValue.iFanSpeed;
		m_dynamicInfo.gpuFanSpeedPercent = 100 * (fanSpeedValue.iFanSpeed - fanSpeedInfo.iMinRPM) / (fanSpeedInfo.iMaxRPM - fanSpeedInfo.iMinRPM) ;
	}
	else
	{
		m_dynamicInfo.gpuFanSpeedPercent = fanSpeedValue.iFanSpeed;
	}

	return true;
}

bool AdlManager::readAdlOverdrive5PowerControl()
{
	int powerControlSupported = 0;
	
	if (ADL_Overdrive5_PowerControl_Caps(m_adapterIndex, &powerControlSupported) != ADL_OK)
	{
		qWarning() << "Failed to get Power Controls support";
		return false;
	}

	if (powerControlSupported)
	{
		ADLPowerControlInfo powerControlInfo = { 0 };

		if (ADL_Overdrive5_PowerControlInfo_Get(m_adapterIndex, &powerControlInfo) != ADL_OK)
		{
			qWarning() << "Failed to get Power Controls Info";
			return false;
		}

		int powerControlCurrent = 0;
		int powerControlDefault = 0;

		if (ADL_Overdrive5_PowerControl_Get(m_adapterIndex, &powerControlCurrent, &powerControlDefault) != ADL_OK)
		{
			qWarning() << "Failed to get Power Control current value";
			return false;
		}

        //printf("The Power Control threshold range is %d to %d with step of %d \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue, powerControlInfo.iStepValue);
        //printf("Current value of Power Control threshold is %d \n", powerControlCurrent);
	}

	return true;
}

bool AdlManager::readAdlOverdrive5ODParameters()
{
	if (!m_overdriveParameters)
	{
		m_overdriveParameters = new ADLODParameters();
		m_overdriveParameters->iSize = sizeof(ADLODParameters);

		if (ADL_Overdrive5_ODParameters_Get(m_adapterIndex, m_overdriveParameters) != ADL_OK)
		{
			qWarning() << "Failed to get overdrive parameters";
			return false;
		}
	}

  //  printf("The GPU  Engine clock range is %d..%d MHz with step of %d Mhz \n",
		//m_overdriveParameters->sEngineClock.iMin / 100,
		//m_overdriveParameters->sEngineClock.iMax / 100,
		//m_overdriveParameters->sEngineClock.iStep / 100);

  //  printf("The GPU  Memory clock range is %d..%d MHz with step of %d MHz\n",
		//m_overdriveParameters->sMemoryClock.iMin / 100,
		//m_overdriveParameters->sMemoryClock.iMax / 100,
		//m_overdriveParameters->sMemoryClock.iStep);


  //  printf("The GPU  Core Voltage range is %d..%d with step of %d \n",
		//m_overdriveParameters->sVddc.iMin,
		//m_overdriveParameters->sVddc.iMax,
		//m_overdriveParameters->sVddc.iStep);

	return true;
}

bool AdlManager::readAdlOverdrive5PerformanceLevel()
{
	ADLODPerformanceLevels* performanceLevels = 0;

	if (m_overdriveParameters->iNumberOfPerformanceLevels > 0)
	{
		//Overdrive5 GPUs support few performance levels. Application can set distinct engine clocks, memory clocks, core voltage 
		//for each of the performance levels. Performance level with index 0 corresponds to lowest performance system state. 
		//Performance level with highest index corresponds to highest performance system state � 3D game playing for example.
		//Users are usually interested in overclocking highest index performance level.

        printf("The GPU supports %d performance levels: \n", m_overdriveParameters->iNumberOfPerformanceLevels);

		int size = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (m_overdriveParameters->iNumberOfPerformanceLevels - 1);
		void* performanceLevelsBuffer = malloc(size);
		memset(performanceLevelsBuffer, 0, size);
		performanceLevels = (ADLODPerformanceLevels*)performanceLevelsBuffer;
		performanceLevels->iSize = size;


		if (ADL_Overdrive5_ODPerformanceLevels_Get(m_adapterIndex, 1/*Getting default values first*/, performanceLevels) != ADL_OK)
		{
			qWarning() << "Failed to get information about supported performance levels.";
			return false;
		}

		for (int i = 0; i < m_overdriveParameters->iNumberOfPerformanceLevels; i++)
		{
            printf("Performance level %d - Default Engine Clock:%d MHz, Default Memory Clock:%d MHz, Default Core Voltage:%d \n",
				i,
				performanceLevels->aLevels[i].iEngineClock / 100,
				performanceLevels->aLevels[i].iMemoryClock / 100,
				performanceLevels->aLevels[i].iVddc);
		}

		memset(performanceLevelsBuffer, 0, size);
		performanceLevels->iSize = size;

		if (ADL_Overdrive5_ODPerformanceLevels_Get(m_adapterIndex, 0/*Getting current values first*/, performanceLevels) != ADL_OK)
		{
			qWarning() << "Failed to get information about supported performance levels.";
			return false;
		}

		for (int i = 0; i < m_overdriveParameters->iNumberOfPerformanceLevels; i++)
		{
            printf("Performance level %d - Current Engine Clock:%d MHz, Current Memory Clock:%d MHz, Current Core Voltage:%d \n",
				i,
				performanceLevels->aLevels[i].iEngineClock / 100,
				performanceLevels->aLevels[i].iMemoryClock / 100,
				performanceLevels->aLevels[i].iVddc);
		}
	}

	return true;
}

bool AdlManager::readAdlOverdrive5CurrentActivity()
{
	//Getting real current values for clocks, performance levels, voltage effective in the system.
	ADLPMActivity activity = { 0 };
	activity.iSize = sizeof(ADLPMActivity);
	if (ADL_Overdrive5_CurrentActivity_Get(m_adapterIndex, &activity) != ADL_OK)
	{
		qWarning() << "Failed to get current GPU activity.";
		return false;
	}

	if (m_overdriveParameters->iActivityReportingSupported)
	{
		m_dynamicInfo.gpuGraphicsUsage = activity.iActivityPercent;
	}

    m_dynamicInfo.gpuGraphicsClock = activity.iEngineClock / 100;
    m_dynamicInfo.gpuMemoryClock = activity.iMemoryClock / 100;
	m_dynamicInfo.gpuGraphicsVoltage = activity.iVddc;

	return true;
}

bool AdlManager::readAdlOverdrive6Info()
{
	readAdlOverdrive6Temperature();
	readAdlOverdrive6FanSpeed();
	readAdlOverdrive6CurrentStatus();
	readAdlOverdrive6PowerControl();

	return true;
}

bool AdlManager::readAdlOverdrive6Temperature()
{
	if (!m_thermalControllerCaps)
	{
		m_thermalControllerCaps = new ADLOD6ThermalControllerCaps();

		if (ADL_Overdrive6_ThermalController_Caps(m_adapterIndex, m_thermalControllerCaps) != ADL_OK)
		{
			qWarning() << "Failed to get thermal controller capabilities";
			return false;
		}
	}

	if (ADL_OD6_TCCAPS_THERMAL_CONTROLLER == (m_thermalControllerCaps->iCapabilities & ADL_OD6_TCCAPS_THERMAL_CONTROLLER)) //Verifies that thermal controller exists on the GPU.
	{
		int temperature = 0;

		if (ADL_Overdrive6_Temperature_Get(m_adapterIndex, &temperature) != ADL_OK)
		{
			qWarning() << "Failed to get GPU temperature";
			return false;
		}

		m_dynamicInfo.gpuTemperature = temperature / 1000; //The temperature is returned in mili-degree of Celsius
	}

	return true;
}

bool AdlManager::readAdlOverdrive6FanSpeed()
{
	ADLOD6FanSpeedInfo fanSpeedInfo = { 0 };
	ADLOD6FanSpeedValue fanSpeedValue = { 0 };

	if (ADL_OD6_TCCAPS_FANSPEED_CONTROL == (m_thermalControllerCaps->iCapabilities & ADL_OD6_TCCAPS_FANSPEED_CONTROL)) //Verifies that fan speed controller exists on the GPU.
	{
		if (ADL_OD6_TCCAPS_FANSPEED_PERCENT_READ == (m_thermalControllerCaps->iCapabilities & ADL_OD6_TCCAPS_FANSPEED_PERCENT_READ) ||
			ADL_OD6_TCCAPS_FANSPEED_RPM_READ == (m_thermalControllerCaps->iCapabilities & ADL_OD6_TCCAPS_FANSPEED_RPM_READ))
		{
			/*fanSpeedInfo.iSpeedType = ((thermalControllerCaps.iCapabilities & ADL_OD6_TCCAPS_FANSPEED_RPM_READ) == ADL_OD6_TCCAPS_FANSPEED_RPM_READ)?
										ADL_OD6_FANSPEED_TYPE_RPM :
										ADL_OD6_FANSPEED_TYPE_PERCENT;*/

			if (ADL_Overdrive6_FanSpeed_Get(m_adapterIndex, &fanSpeedInfo) != ADL_OK)
			{
                printf("Failed to get fan speed info\n");
				return false;
			}

			if (ADL_OD6_TCCAPS_FANSPEED_RPM_READ == (m_thermalControllerCaps->iCapabilities & ADL_OD6_TCCAPS_FANSPEED_RPM_READ))
			{
                printf("Fan speed range: %d..%d rpm\n", m_thermalControllerCaps->iFanMinRPM, m_thermalControllerCaps->iFanMaxRPM);
			}
			else
			{
                printf("Fan speed range: %d..%d percent\n", m_thermalControllerCaps->iFanMinPercent, m_thermalControllerCaps->iFanMaxPercent);
			}


			if (ADL_OD6_FANSPEED_TYPE_RPM == (fanSpeedInfo.iSpeedType & ADL_OD6_FANSPEED_TYPE_RPM))
                printf("Current fan speed: %d rpm \n", fanSpeedInfo.iFanSpeedRPM);
			else
                printf("Current fan speed: %d percent \n", fanSpeedInfo.iFanSpeedPercent);

            m_dynamicInfo.gpuFanSpeed = fanSpeedInfo.iFanSpeedRPM;
            m_dynamicInfo.gpuFanSpeedPercent = fanSpeedInfo.iFanSpeedPercent;
		}
	}

	return true;
}

bool AdlManager::readAdlOverdrive6CurrentStatus()
{
	ADLOD6Capabilities od6Capabilities = { 0 };
	ADLOD6CurrentStatus currentStatus = { 0 };

	if (ADL_Overdrive6_Capabilities_Get(m_adapterIndex, &od6Capabilities) != ADL_OK)
	{
        printf("Failed to get Overdrive capabilities\n");
		return false;
	}

	//Overdrive 6 uses performance levels to communicate effective minimum and maximum values for memory and GPU clocks 
	//either requested by user or preconfigured by the driver defaults. Thus only 2 performance levels  should be returned.
	if (od6Capabilities.iNumberOfPerformanceLevels != 2)
	{
        printf("Unexpected number of performance levels\n");
		return false;
	}

	//Calculating buffer needed to contain performance levels information returned by ADL_Overdrive6_StateInfo_Get. 
	//The buffer contains instance of ADLOD6StateInfo at the beginning and trailing space needed to include extra ADLOD6PerformanceLevel structures. 
	//Note that single ADLOD6PerformanceLevel structure is included in ADLOD6StateInfo itself. So the trailing space is needed to be large enough 
	//to hold only one extra ADLOD6PerformanceLevel
	int size = sizeof(ADLOD6StateInfo) + sizeof(ADLOD6PerformanceLevel);
	ADLOD6StateInfo* defaultStateInfo = (ADLOD6StateInfo*)malloc(size);
	memset((void*)defaultStateInfo, 0, size);
	defaultStateInfo->iNumberOfPerformanceLevels = 2;

	//Getting default effective minimum and maximum values for memory and GPU clocks.
	//The only state supported by Overdrive6 is "Performance". 
	if (ADL_Overdrive6_StateInfo_Get(m_adapterIndex, ADL_OD6_GETSTATEINFO_DEFAULT_PERFORMANCE, defaultStateInfo) != ADL_OK)
	{
        printf("Failed to get default performance levels info\n");
		return false;
	}

	ADLOD6StateInfo* customStateInfo = (ADLOD6StateInfo*)malloc(size);
	memset((void*)customStateInfo, 0, size);
	customStateInfo->iNumberOfPerformanceLevels = 2;

	//Getting default effective minimum and maximum values for memory and GPU clocks.
	//The only state supported by Overdrive6 is "Performance". 
	if (ADL_Overdrive6_StateInfo_Get(m_adapterIndex, ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE, customStateInfo) != ADL_OK)
	{
        printf("Failed to get custom performance levels info\n");
		return false;
	}

	if (ADL_OD6_CAPABILITY_SCLK_CUSTOMIZATION == (od6Capabilities.iCapabilities & ADL_OD6_CAPABILITY_SCLK_CUSTOMIZATION))
	{
        printf("Range of clock supported by GPU core: %d...%d MHz with step of %d MHz \n",
			od6Capabilities.sEngineClockRange.iMin / 100,
			od6Capabilities.sEngineClockRange.iMax / 100,
			od6Capabilities.sEngineClockRange.iStep / 100);

        printf("Default effective range of GPU core clock: %d .. %d \n", defaultStateInfo->aLevels[0].iEngineClock / 100, defaultStateInfo->aLevels[1].iEngineClock / 100);
        printf("Custom effective range of GPU core clock: %d .. %d \n", customStateInfo->aLevels[0].iEngineClock / 100, customStateInfo->aLevels[1].iEngineClock / 100);
	}

	if (ADL_OD6_CAPABILITY_MCLK_CUSTOMIZATION == (od6Capabilities.iCapabilities & ADL_OD6_CAPABILITY_MCLK_CUSTOMIZATION))
	{
        printf("Range of supported memory clock: %d...%d MHz with step of %d MHz \n",
			od6Capabilities.sMemoryClockRange.iMin / 100,
			od6Capabilities.sMemoryClockRange.iMax / 100,
			od6Capabilities.sMemoryClockRange.iStep / 100);

        printf("Default effective range of GPU memory clock: %d .. %d \n", defaultStateInfo->aLevels[0].iMemoryClock / 100, defaultStateInfo->aLevels[1].iMemoryClock / 100);
        printf("Custom effective range of GPU memory clock: %d .. %d \n", customStateInfo->aLevels[0].iMemoryClock / 100, customStateInfo->aLevels[1].iMemoryClock / 100);
	}

	if (ADL_Overdrive6_CurrentStatus_Get(m_adapterIndex, &currentStatus) != ADL_OK)
	{
        printf("Failed to get custom performance levels info\n");
		return false;
	}

    printf("Current GPU core clock: %d MHz \n", currentStatus.iEngineClock / 100);
    printf("Current GPU memory clock: %d MHz \n", currentStatus.iMemoryClock / 100);
    printf("Current number of PCI bus lanes: %d \n", currentStatus.iCurrentBusLanes);
    printf("Current PCI bus speed: %d \n", currentStatus.iCurrentBusSpeed);

	//First we need to verify that ASIC supports monitoring of its current activities before we attempt to retrieve its current clock
	if ((od6Capabilities.iCapabilities & ADL_OD6_CAPABILITY_GPU_ACTIVITY_MONITOR) == ADL_OD6_CAPABILITY_GPU_ACTIVITY_MONITOR)
	{
        printf("Current GPU activity level: %d percent \n", currentStatus.iActivityPercent);
	}

	return true;
}

bool AdlManager::readAdlOverdrive6PowerControl()
{
	int powerControlSupported = 0;
	ADLOD6PowerControlInfo powerControlInfo = { 0 };
	int powerControlCurrent = 0;
	int powerControlDefault = 0;

	if (ADL_Overdrive6_PowerControl_Caps(m_adapterIndex, &powerControlSupported) != ADL_OK)
	{
        printf("Failed to get power control capabilities\n");
		return false;
	}

	if (powerControlSupported)
	{
		if (ADL_Overdrive6_PowerControlInfo_Get(m_adapterIndex, &powerControlInfo) != ADL_OK)
		{
            printf("Failed to get power control information\n");
			return false;
		}

		if (ADL_Overdrive6_PowerControl_Get(m_adapterIndex, &powerControlCurrent, &powerControlDefault) != ADL_OK)
		{
            printf("Failed to get power control current and default settings\n");
			return false;
		}

        printf("Power Control range: %d...%d with step of %d \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue, powerControlInfo.iStepValue);
        printf("Power Control current level: %d \n", powerControlCurrent);
        printf("Power Control default level: %d \n", powerControlDefault);
	}

	return true;
}

bool AdlManager::readAdl2Overdrive5Info()
{
	readAdl2Overdrive5Temperature();
	readAdl2Overdrive5FanSpeed();
	readAdl2Overdrive5PowerControl();
	readAdl2Overdrive5ODParameters();
	readAdl2Overdrive5PerformanceLevel();

	return true;
}

bool AdlManager::readAdl2Overdrive5Temperature()
{
	ADLThermalControllerInfo termalControllerInfo = { 0 };
	termalControllerInfo.iSize = sizeof(ADLThermalControllerInfo);

	int result;

	for (int iThermalControllerIndex = 0; iThermalControllerIndex < 10; iThermalControllerIndex++)
	{
		result = ADL2_Overdrive5_ThermalDevices_Enum(m_context, m_adapterIndex, iThermalControllerIndex, &termalControllerInfo);

		if (result == ADL_WARNING_NO_DATA)
		{
			m_maxThermalControllerIndex = iThermalControllerIndex - 1;
			break;
		}

		if (result == ADL_WARNING_NO_DATA)
		{
            printf("Failed to enumerate thermal devices\n");
			return false;
		}

		if (termalControllerInfo.iThermalDomain == ADL_DL_THERMAL_DOMAIN_GPU)
		{
			ADLTemperature adlTemperature = { 0 };
			adlTemperature.iSize = sizeof(ADLTemperature);
			if (ADL2_Overdrive5_Temperature_Get(m_context, m_adapterIndex, iThermalControllerIndex, &adlTemperature) != ADL_OK)
			{
                printf("Failed to get thermal devices temperature\n");
				return false;
			}
			int temperatureInDegreesCelsius = adlTemperature.iTemperature / 1000; // The temperature is returned in millidegrees Celsius.

            printf("Thermal controller id:%d \n", iThermalControllerIndex);
            printf("Current temperature: %d\n", temperatureInDegreesCelsius);

			m_thermalDomainControllerIndex = iThermalControllerIndex;
		}
	}

	return true;
}

bool AdlManager::readAdl2Overdrive5FanSpeed()
{
	ADLFanSpeedInfo fanSpeedInfo;
	fanSpeedInfo.iSize = sizeof(ADLFanSpeedInfo);

	if (ADL_Overdrive5_FanSpeedInfo_Get(m_adapterIndex, m_thermalDomainControllerIndex, &fanSpeedInfo) != ADL_OK)
	{
		printf("Failed to get fan caps\n");
		return false;
	}

	int fanSpeedReportingMethod = ((fanSpeedInfo.iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_READ) == ADL_DL_FANCTRL_SUPPORTS_RPM_READ) ? ADL_DL_FANCTRL_SPEED_TYPE_RPM : ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;

	ADLFanSpeedValue fanSpeedValue = { 0 };

	//Set to ADL_DL_FANCTRL_SPEED_TYPE_RPM or to ADL_DL_FANCTRL_SPEED_TYPE_PERCENT to request fan speed to be returned in rounds per minute or in percentage points.
	//Note that the call might fail if requested fan speed reporting method is not supported by the GPU.
	fanSpeedValue.iSpeedType = fanSpeedReportingMethod;
	if (ADL_Overdrive5_FanSpeed_Get(m_adapterIndex, m_thermalDomainControllerIndex, &fanSpeedValue) != ADL_OK)
	{
        printf("Failed to get fan speed\n");
		return false;
	}

	if (fanSpeedReportingMethod == ADL_DL_FANCTRL_SPEED_TYPE_RPM)
	{
        printf("Current fan speed: %d rpm\n", fanSpeedValue.iFanSpeed);
        printf("Minimum fan speed: %d rpm\n", fanSpeedInfo.iMinRPM);
        printf("Maximum fan speed: %d rpm\n", fanSpeedInfo.iMaxRPM);
	}
	else
	{
        printf("Current fan speed: %d percent\n", fanSpeedValue.iFanSpeed);
        printf("Minimum fan speed: %d percent\n", fanSpeedInfo.iMinPercent);
        printf("Maximum fan speed: %d percent\n", fanSpeedInfo.iMaxPercent);
	}

	return true;
}

bool AdlManager::readAdl2Overdrive5PowerControl()
{
	int powerControlSupported = 0;
	ADLPowerControlInfo powerControlInfo = { 0 };
	int powerControlCurrent = 0;
	int powerControlDefault = 0;

	if (ADL2_Overdrive5_PowerControl_Caps(m_context, m_adapterIndex, &powerControlSupported) != ADL_OK)
	{
        printf("Failed to get Power Controls support\n");
		return false;
	}

	if (powerControlSupported)
	{
		if (ADL2_Overdrive5_PowerControlInfo_Get(m_context, m_adapterIndex, &powerControlInfo) != ADL_OK)
		{
            printf("Failed to get Power Controls Info\n");
			return false;
		}

		if (ADL2_Overdrive5_PowerControl_Get(m_context, m_adapterIndex, &powerControlCurrent, &powerControlDefault) != ADL_OK)
		{
            printf("Failed to get Power Control current value\n");
			return false;
		}

        printf("The Power Control threshold range is %d to %d with step of %d \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue, powerControlInfo.iStepValue);
        printf("Current value of Power Control threshold is %d \n", powerControlCurrent);
	}

	return true;
}

bool AdlManager::readAdl2Overdrive5ODParameters()
{
	if (!m_overdriveParameters)
	{
		m_overdriveParameters = new ADLODParameters();
		m_overdriveParameters->iSize = sizeof(ADLODParameters);

		if (ADL2_Overdrive5_ODParameters_Get(m_context, m_adapterIndex, m_overdriveParameters) != ADL_OK)
		{
            printf("Failed to get overdrive parameters\n");
			return false;
		}
	}

    printf("The GPU  Engine clock range is %d..%d MHz with step of %d Mhz \n",
		m_overdriveParameters->sEngineClock.iMin / 100,
		m_overdriveParameters->sEngineClock.iMax / 100,
		m_overdriveParameters->sEngineClock.iStep / 100);

    printf("The GPU  Memory clock range is %d..%d MHz with step of %d MHz\n",
		m_overdriveParameters->sMemoryClock.iMin / 100,
		m_overdriveParameters->sMemoryClock.iMax / 100,
		m_overdriveParameters->sMemoryClock.iStep);


    printf("The GPU  Core Voltage range is %d..%d with step of %d \n",
		m_overdriveParameters->sVddc.iMin,
		m_overdriveParameters->sVddc.iMax,
		m_overdriveParameters->sVddc.iStep);

	return true;
}

bool AdlManager::readAdl2Overdrive5PerformanceLevel()
{
	ADLODPerformanceLevels* performanceLevels = 0;

	if (m_overdriveParameters->iNumberOfPerformanceLevels > 0)
	{
		//Overdrive5 GPUs support few performance levels. Application can set distinct engine clocks, memory clocks, core voltage 
		//for each of the performance levels. Performance level with index 0 corresponds to lowest performance system state. 
		//Performance level with highest index corresponds to highest performance system state � 3D game playing for example.
		//Users are usually interested in overclocking highest index performance level.

        printf("The GPU supports %d performance levels: \n", m_overdriveParameters->iNumberOfPerformanceLevels);

		int size = sizeof(ADLODPerformanceLevels) + sizeof(ADLODPerformanceLevel) * (m_overdriveParameters->iNumberOfPerformanceLevels - 1);
		void* performanceLevelsBuffer = malloc(size);
		memset(performanceLevelsBuffer, 0, size);
		performanceLevels = (ADLODPerformanceLevels*)performanceLevelsBuffer;
		performanceLevels->iSize = size;


		if (ADL2_Overdrive5_ODPerformanceLevels_Get(m_context, m_adapterIndex, 1/*Getting default values first*/, performanceLevels) != ADL_OK)
		{
            printf("Failed to get information about supported performance levels.  \n");
			return 0;
		}

		for (int i = 0; i < m_overdriveParameters->iNumberOfPerformanceLevels; i++)
		{
            printf("Performance level %d - Default Engine Clock:%d MHz, Default Memory Clock:%d MHz, Default Core Voltage:%d \n",
				i,
				performanceLevels->aLevels[i].iEngineClock / 100,
				performanceLevels->aLevels[i].iMemoryClock / 100,
				performanceLevels->aLevels[i].iVddc);
		}

		memset(performanceLevelsBuffer, 0, size);
		performanceLevels->iSize = size;

		if (ADL2_Overdrive5_ODPerformanceLevels_Get(m_context, m_adapterIndex, 0/*Getting current values first*/, performanceLevels) != ADL_OK)
		{
            printf("Failed to get information about supported performance levels.  \n");
			return 0;
		}

		for (int i = 0; i < m_overdriveParameters->iNumberOfPerformanceLevels; i++)
		{
            printf("Performance level %d - Current Engine Clock:%d MHz, Current Memory Clock:%d MHz, Current Core Voltage:%d \n",
				i,
				performanceLevels->aLevels[i].iEngineClock / 100,
				performanceLevels->aLevels[i].iMemoryClock / 100,
				performanceLevels->aLevels[i].iVddc);
		}
	}
}

bool AdlManager::readAdl2Overdrive5CurrentActivity()
{
	if (!ADL2_Overdrive5_CurrentActivity_Get)
	{
		return false;
	}

	//Getting real current values for clocks, performance levels, voltage effective in the system.
	ADLPMActivity activity = { 0 };
	activity.iSize = sizeof(ADLPMActivity);
	if (ADL2_Overdrive5_CurrentActivity_Get(m_context, m_adapterIndex, &activity) != ADL_OK)
	{
		qWarning() << "Failed to get current GPU activity.";
		return false;
	}

	if (m_overdriveParameters->iActivityReportingSupported)
	{
		m_dynamicInfo.gpuGraphicsClock = activity.iEngineClock / 100;
		m_dynamicInfo.gpuMemoryClock = activity.iMemoryClock / 100;
		m_dynamicInfo.gpuGraphicsVoltage = activity.iVddc;
		m_dynamicInfo.gpuGraphicsUsage = activity.iActivityPercent;
		//activity.iCurrentPerformanceLevel
		return true;
	}
	else
	{
		return false;
	}
}

bool AdlManager::readAdl2Overdrive6Info()
{
	return false;
}

bool AdlManager::readAdl2Overdrive8Info()
{
    readAdl2Overdrive8InitSetting();
    readAdl2Overdrive8CurrentSetting();

	if (!ADL2_New_QueryPMLogData_Get)
	{
		return false;
	}

	ADLPMLogDataOutput odlpDataOutput;
	memset(&odlpDataOutput, 0, sizeof(ADLPMLogDataOutput));

	if (ADL2_New_QueryPMLogData_Get(m_context, m_adapterIndex, &odlpDataOutput) == ADL_OK)
	{
		if (odlpDataOutput.sensors[PMLOG_INFO_ACTIVITY_GFX].supported)
		{
			m_dynamicInfo.gpuGraphicsUsage = odlpDataOutput.sensors[PMLOG_INFO_ACTIVITY_GFX].value;
		}
		if (odlpDataOutput.sensors[PMLOG_INFO_ACTIVITY_MEM].supported)
		{
			m_dynamicInfo.gpuMemoryUsage = odlpDataOutput.sensors[PMLOG_INFO_ACTIVITY_MEM].value;
		}

		if (odlpDataOutput.sensors[PMLOG_CLK_GFXCLK].supported)
		{
			m_dynamicInfo.gpuGraphicsClock = odlpDataOutput.sensors[PMLOG_CLK_GFXCLK].value;
		}
		if (odlpDataOutput.sensors[PMLOG_CLK_MEMCLK].supported)
		{
			m_dynamicInfo.gpuMemoryClock = odlpDataOutput.sensors[PMLOG_CLK_MEMCLK].value;
		}
		
		if (odlpDataOutput.sensors[PMLOG_TEMPERATURE_EDGE].supported)
		{
			m_dynamicInfo.gpuTemperature = odlpDataOutput.sensors[PMLOG_TEMPERATURE_EDGE].value;
		}
		if (odlpDataOutput.sensors[PMLOG_TEMPERATURE_HOTSPOT].supported)
		{
			m_dynamicInfo.gpuTemperatureHotspot = odlpDataOutput.sensors[PMLOG_TEMPERATURE_HOTSPOT].value;
		}

		//PMLOG_TEMPERATURE_MEM
		//PMLOG_TEMPERATURE_VRVDDC
		//PMLOG_TEMPERATURE_VRMVDD
		//PMLOG_TEMPERATURE_LIQUID
		//PMLOG_TEMPERATURE_PLX
		//PMLOG_TEMPERATURE_VRSOC = 24,
		//PMLOG_TEMPERATURE_VRMVDD0 = 25,
		//PMLOG_TEMPERATURE_VRMVDD1 = 26,
		//PMLOG_TEMPERATURE_GFX = 28,
		//PMLOG_TEMPERATURE_SOC = 29,

		if (odlpDataOutput.sensors[PMLOG_FAN_RPM].supported)
		{
			m_dynamicInfo.gpuFanSpeed = odlpDataOutput.sensors[PMLOG_FAN_RPM].value;
		}
		if (odlpDataOutput.sensors[PMLOG_FAN_PERCENTAGE].supported)
		{
			m_dynamicInfo.gpuFanSpeedPercent = odlpDataOutput.sensors[PMLOG_FAN_PERCENTAGE].value;
		}

		//PMLOG_SOC_VOLTAGE = 16,
		//PMLOG_SOC_POWER = 17,
		//PMLOG_SOC_CURRENT = 18,

		if (odlpDataOutput.sensors[PMLOG_GFX_POWER].supported)
		{
			m_dynamicInfo.gpuGraphicsPower = odlpDataOutput.sensors[PMLOG_GFX_POWER].value;
		}

		if (odlpDataOutput.sensors[PMLOG_ASIC_POWER].supported)
		{
			m_dynamicInfo.gpuAsicPower = odlpDataOutput.sensors[PMLOG_ASIC_POWER].value;
		}

		if (odlpDataOutput.sensors[PMLOG_GFX_VOLTAGE].supported)
		{
			m_dynamicInfo.gpuGraphicsVoltage = odlpDataOutput.sensors[PMLOG_GFX_VOLTAGE].value;
		}

		if (odlpDataOutput.sensors[PMLOG_MEM_VOLTAGE].supported)
		{
			m_dynamicInfo.gpuMemoryVoltage = odlpDataOutput.sensors[PMLOG_MEM_VOLTAGE].value;
		}
		//PMLOG_GFX_CURRENT
		return true;
	}
	else
	{
		return false;
	}
    //readAdl2Overdrive8OneRange();
    //readAdl2Overdrive8GPUClocksParameters();
    //readAdl2Overdrive8GPUVoltageParameters();
    //readAdl2Overdrive8MemoryClocksParameters();
    //readAdl2Overdrive8TemperatureSettingParameters();
    //readAdl2Overdrive8FanSettingParameters();
    //readAdl2Overdrive8MemoryTimingSettingParameters();
}

bool AdlManager::readAdl2Overdrive8InitSetting()
{
    memset(&m_od8InitSetting, 0, sizeof(ADLOD8InitSetting));
    m_od8InitSetting.count = OD8_COUNT;

    if (ADL2_Overdrive8_Init_SettingX2_Get)
    {
		int overdrive8Capabilities;
		int numberOfFeatures = OD8_COUNT;
		ADLOD8SingleInitSetting* lpInitSettingList = nullptr;

        if (ADL2_Overdrive8_Init_SettingX2_Get(m_context, m_adapterIndex, &overdrive8Capabilities, &numberOfFeatures, &lpInitSettingList) == ADL_OK)
        {
            m_od8InitSetting.count = numberOfFeatures > OD8_COUNT ? OD8_COUNT : numberOfFeatures;
            m_od8InitSetting.overdrive8Capabilities = overdrive8Capabilities;
            for (int i = 0; i < m_od8InitSetting.count; i++)
            {
                m_od8InitSetting.od8SettingTable[i].defaultValue = lpInitSettingList[i].defaultValue;
                m_od8InitSetting.od8SettingTable[i].featureID = lpInitSettingList[i].featureID;
                m_od8InitSetting.od8SettingTable[i].maxValue = lpInitSettingList[i].maxValue;
                m_od8InitSetting.od8SettingTable[i].minValue = lpInitSettingList[i].minValue;
            }
            ADL_Main_Memory_Free((void**)&lpInitSettingList);
        }
        else
        {
			qWarning() << "ADL2_Overdrive8_Init_SettingX2_Get is failed";
            ADL_Main_Memory_Free((void**)&lpInitSettingList);
            return false;
        }
    }
    else if (ADL2_Overdrive8_Init_Setting_Get)
    {
        if (ADL2_Overdrive8_Init_Setting_Get(m_context, m_adapterIndex, &m_od8InitSetting) != ADL_OK)
        {
			qWarning() << "ADL2_Overdrive8_Init_Setting_Get is failed";
            return false;
        }
    }
	else
	{
		return false;
	}

    return true;
}

bool AdlManager::readAdl2Overdrive8CurrentSetting()
{
    memset(&m_od8CurrentSetting, 0, sizeof(ADLOD8CurrentSetting));
    m_od8CurrentSetting.count = OD8_COUNT;

    if (ADL2_Overdrive8_Current_SettingX2_Get)
    {
		int numberOfFeaturesCurrent = OD8_COUNT;
		int* lpCurrentSettingList = nullptr;
        if (ADL2_Overdrive8_Current_SettingX2_Get(m_context, m_adapterIndex, &numberOfFeaturesCurrent, &lpCurrentSettingList) == ADL_OK)
        {
            m_od8CurrentSetting.count = numberOfFeaturesCurrent > OD8_COUNT ? OD8_COUNT : numberOfFeaturesCurrent;
            for (int i = 0; i < m_od8CurrentSetting.count; i++)
            {
                m_od8CurrentSetting.Od8SettingTable[i] = lpCurrentSettingList[i];
            }
            ADL_Main_Memory_Free((void**)&lpCurrentSettingList);
        }
        else
        {
			qWarning() << "ADL2_Overdrive8_Current_SettingX2_Get is failed";
            ADL_Main_Memory_Free((void**)&lpCurrentSettingList);
            return false;
        }
    }
    else if (ADL2_Overdrive8_Current_Setting_Get)
    {
        if (ADL2_Overdrive8_Current_Setting_Get(m_context, m_adapterIndex, &m_od8CurrentSetting))
        {
			qWarning() << "ADL2_Overdrive8_Current_Setting_Get is failed";
            return false;
        }
    }
    return true;
}

bool AdlManager::readAdl2Overdrive8OneRange()
{
    bool RangeSupport_ = (m_od8InitSetting.overdrive8Capabilities & ADL_OD8_FAN_CURVE) ? true : false;
    return RangeSupport_;
}

bool AdlManager::readAdl2Overdrive8GPUClocksParameters()
{
    int i;
    int ret = -1;

    //ADL2_Overdrive_Caps(m_context, m_adapterIndex, &iSupported, &iEnabled, &iVersion);
    if (m_overdriveVersion == 8)
    {
        //OD8 initial Status
//        ADLOD8InitSetting m_od8InitSetting;
//        if (ADL_OK != GetOD8InitSetting(m_adapterIndex, m_od8InitSetting))
//        {
//            printf("Get Init Setting failed.\n");
//            return false;
//        }

//        //OD8 Current Status
//        ADLOD8CurrentSetting m_od8CurrentSetting;
//        if (ADL_OK != GetOD8CurrentSetting(m_adapterIndex, m_od8CurrentSetting))
//        {
//            printf("Get Current Setting failed.\n");
//            return false;
//        }

        ADLPMLogDataOutput odlpDataOutput;
        memset(&odlpDataOutput, 0, sizeof(ADLPMLogDataOutput));
        ret = ADL2_New_QueryPMLogData_Get(m_context, m_adapterIndex, &odlpDataOutput);
        if (0 == ret)
        {
            if ((m_od8InitSetting.overdrive8Capabilities & ADL_OD8_GFXCLK_LIMITS) == ADL_OD8_GFXCLK_LIMITS &&
                (m_od8InitSetting.overdrive8Capabilities & ADL_OD8_GFXCLK_CURVE) == ADL_OD8_GFXCLK_CURVE)
            {
                //GPU clocks
                //OverdriveRangeDataStruct oneRangeData;
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_GFXCLK_FREQ1, ADL_OD8_GFXCLK_CURVE);
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_GFXCLK_FREQ2, ADL_OD8_GFXCLK_CURVE);
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_GFXCLK_FREQ3, ADL_OD8_GFXCLK_CURVE);
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_GFXCLK_FMIN, ADL_OD8_GFXCLK_CURVE);
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_GFXCLK_FMAX, ADL_OD8_GFXCLK_CURVE);

                printf("ADLSensorType: PMLOG_CLK_GFXCLK\n");
                printf("PMLOG_CLK_GFXCLK.supported:%d\n", odlpDataOutput.sensors[PMLOG_CLK_GFXCLK].supported);
                printf("PMLOG_CLK_GFXCLK.value:%d\n", odlpDataOutput.sensors[PMLOG_CLK_GFXCLK].value);
                printf("-----------------------------------------\n");
                printf("ADLSensorType: PMLOG_INFO_ACTIVITY_GFX-GPU activity percentage value\n");
                printf("PMLOG_INFO_ACTIVITY_GFX.supported:%d\n", odlpDataOutput.sensors[PMLOG_INFO_ACTIVITY_GFX].supported);
                printf("PMLOG_INFO_ACTIVITY_GFX.value:%d\n", odlpDataOutput.sensors[PMLOG_INFO_ACTIVITY_GFX].value);
                printf("-----------------------------------------\n");

            }
            else if ((m_od8InitSetting.overdrive8Capabilities & ADL_OD8_GFXCLK_LIMITS) == ADL_OD8_GFXCLK_LIMITS &&
                     (m_od8InitSetting.overdrive8Capabilities & ADL_OD8_GFXCLK_CURVE) != ADL_OD8_GFXCLK_CURVE) {
                //GPU clocks
                //OverdriveRangeDataStruct oneRangeData;
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_GFXCLK_FMIN, ADL_OD8_GFXCLK_CURVE);
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_GFXCLK_FMAX, ADL_OD8_GFXCLK_CURVE);

                printf("ADLSensorType: PMLOG_CLK_GFXCLK\n");
                printf("PMLOG_CLK_GFXCLK.supported:%d\n", odlpDataOutput.sensors[PMLOG_CLK_GFXCLK].supported);
                printf("PMLOG_CLK_GFXCLK.value:%d\n", odlpDataOutput.sensors[PMLOG_CLK_GFXCLK].value);
                printf("-----------------------------------------\n");
                printf("ADLSensorType: PMLOG_INFO_ACTIVITY_GFX-GPU activity percentage value\n");
                printf("PMLOG_INFO_ACTIVITY_GFX.supported:%d\n", odlpDataOutput.sensors[PMLOG_INFO_ACTIVITY_GFX].supported);
                printf("PMLOG_INFO_ACTIVITY_GFX.value:%d\n", odlpDataOutput.sensors[PMLOG_INFO_ACTIVITY_GFX].value);
                printf("-----------------------------------------\n");
            }
            else
                printf("OD8PLUS Failed to get GPU clocks\n");
        }
        else
        {
            printf("ADL2_New_QueryPMLogData_Get is failed\n");
            return false;
        }
    }
    return true;
}

bool AdlManager::readAdl2Overdrive8GPUVoltageParameters()
{
	int ret = -1;
    //ADL2_Overdrive_Caps(m_context, m_adapterIndex, &iSupported, &iEnabled, &iVersion);
    if (m_overdriveVersion == 8)
    {
        //OD8 initial Status
//        ADLOD8InitSetting m_od8InitSetting;
//        if (ADL_OK != GetOD8InitSetting(m_adapterIndex, m_od8InitSetting))
//        {
//            printf("Get Init Setting failed.\n");
//            return false;
//        }

//        //OD8 Current Status
//        ADLOD8CurrentSetting m_od8CurrentSetting;
//        if (ADL_OK != GetOD8CurrentSetting(m_adapterIndex, m_od8CurrentSetting))
//        {
//            printf("Get Current Setting failed.\n");
//            return false;
//        }

        //make ADL call for VEGA12
        ADLPMLogDataOutput odlpDataOutput;
        memset(&odlpDataOutput, 0, sizeof(ADLPMLogDataOutput));
        ret = ADL2_New_QueryPMLogData_Get(m_context, m_adapterIndex, &odlpDataOutput);
        if (0 == ret)
        {
            if (((m_od8InitSetting.overdrive8Capabilities & ADL_OD8_GFXCLK_LIMITS) == ADL_OD8_GFXCLK_LIMITS) &&
                ((m_od8InitSetting.overdrive8Capabilities & ADL_OD8_GFXCLK_CURVE) == ADL_OD8_GFXCLK_CURVE))
            {
                //GPU Voltage
                //OverdriveRangeDataStruct oneRangeData;
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_GFXCLK_VOLTAGE1, ADL_OD8_GFXCLK_CURVE);
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_GFXCLK_VOLTAGE2, ADL_OD8_GFXCLK_CURVE);
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_GFXCLK_VOLTAGE3, ADL_OD8_GFXCLK_CURVE);
            }
            else if (((m_od8InitSetting.overdrive8Capabilities & ADL_OD8_GFXCLK_LIMITS) == ADL_OD8_GFXCLK_LIMITS) &&
                     ((m_od8InitSetting.overdrive8Capabilities & ADL_OD8_GFXCLK_CURVE) != ADL_OD8_GFXCLK_CURVE)) {
                //GPU Voltage
                //OverdriveRangeDataStruct oneRangeData;
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_OD_VOLTAGE, ADL_OD8_ODVOLTAGE_LIMIT);
            }
            else
                printf("OD8 Failed to get GPU voltages\n");
        }
        else
        {
            printf("ADL2_New_QueryPMLogData_Get is failed\n");
            return false;
        }
    }
    return true;
}

bool AdlManager::readAdl2Overdrive8MemoryClocksParameters()
{
	int ret = -1;
    //ADL2_Overdrive_Caps(m_context, m_adapterIndex, &iSupported, &iEnabled, &iVersion);
    if (m_overdriveVersion == 8)
    {
//        //OD8 initial Status
//        ADLOD8InitSetting m_od8InitSetting;
//        if (ADL_OK != GetOD8InitSetting(m_adapterIndex, m_od8InitSetting))
//        {
//            printf("Get Init Setting failed.\n");
//            return false;
//        }

//        //OD8 Current Status
//        ADLOD8CurrentSetting m_od8CurrentSetting;
//        if (ADL_OK != GetOD8CurrentSetting(m_adapterIndex, m_od8CurrentSetting))
//        {
//            printf("Get Current Setting failed.\n");
//            return false;
//        }

        //make ADL call for VEGA12
        ADLPMLogDataOutput odlpDataOutput;
        memset(&odlpDataOutput, 0, sizeof(ADLPMLogDataOutput));
        ret = ADL2_New_QueryPMLogData_Get(m_context, m_adapterIndex, &odlpDataOutput);
        if (0 == ret)
        {
            if ((m_od8InitSetting.overdrive8Capabilities & ADL_OD8_UCLK_MAX) == ADL_OD8_UCLK_MAX)
            {
                //Memory Clocks
                //OverdriveRangeDataStruct oneRangeData;
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_UCLK_FMAX, ADL_OD8_UCLK_MAX);

                if (m_od8InitSetting.overdrive8Capabilities & m_od8InitSetting.od8SettingTable[OD8_UCLK_FMIN].featureID)
                    //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_UCLK_FMIN, ADL_OD8_UCLK_MAX);

                printf("ADLSensorType: PMLOG_CLK_MEMCLK\n");
                printf("PMLOG_CLK_MEMCLK.supported:%d\n", odlpDataOutput.sensors[PMLOG_CLK_MEMCLK].supported);
                printf("PMLOG_CLK_MEMCLK.value:%d\n", odlpDataOutput.sensors[PMLOG_CLK_MEMCLK].value);
                printf("-----------------------------------------\n");

            }
            else
                printf("OD8 Failed to get Memory Clocks\n");
        }
        else
        {
            printf("ADL2_New_QueryPMLogData_Get is failed\n");
            return false;
        }
    }
    return true;
}


bool AdlManager::readAdl2Overdrive8TemperatureSettingParameters()
{
	int ret = -1;
    //ADL2_Overdrive_Caps(m_context, m_adapterIndex, &iSupported, &iEnabled, &iVersion);
    if (m_overdriveVersion == 8)
    {
//        //OD8 initial Status
//        ADLOD8InitSetting m_od8InitSetting;
//        if (ADL_OK != GetOD8InitSetting(m_adapterIndex, m_od8InitSetting))
//        {
//            printf("Get Init Setting failed.\n");
//            return false;
//        }

//        //OD8 Current Status
//        ADLOD8CurrentSetting m_od8CurrentSetting;
//        if (ADL_OK != GetOD8CurrentSetting(m_adapterIndex, m_od8CurrentSetting))
//        {
//            printf("Get Current Setting failed.\n");
//            return false;
//        }

        //make ADL call for VEGA12
        ADLPMLogDataOutput odlpDataOutput;
        memset(&odlpDataOutput, 0, sizeof(ADLPMLogDataOutput));
        ret = ADL2_New_QueryPMLogData_Get(m_context, m_adapterIndex, &odlpDataOutput);
        if (0 == ret)
        {
            if (((m_od8InitSetting.overdrive8Capabilities &  ADL_OD8_TEMPERATURE_SYSTEM) == ADL_OD8_TEMPERATURE_SYSTEM) ||
                ((m_od8InitSetting.overdrive8Capabilities &  ADL_OD8_TEMPERATURE_FAN) == ADL_OD8_TEMPERATURE_FAN) ||
                ((m_od8InitSetting.overdrive8Capabilities &  ADL_OD8_POWER_LIMIT) == ADL_OD8_POWER_LIMIT))
            {
                //OverdriveRangeDataStruct oneRangeData;
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_OPERATING_TEMP_MAX, ADL_OD8_TEMPERATURE_SYSTEM);
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_FAN_TARGET_TEMP, ADL_OD8_TEMPERATURE_FAN);
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_POWER_PERCENTAGE, ADL_OD8_POWER_LIMIT);

                printf("ADLSensorType: PMLOG_TEMPERATURE_EDGE - Current Temp\n");
                printf("PMLOG_TEMPERATURE_EDGE.supported:%d\n", odlpDataOutput.sensors[PMLOG_TEMPERATURE_EDGE].supported);
                printf("PMLOG_TEMPERATURE_EDGE.value:%d\n", odlpDataOutput.sensors[PMLOG_TEMPERATURE_EDGE].value);

                printf("ADLSensorType: PMLOG_TEMPERATURE_HOTSPOT - Junction Temp\n");
                printf("PMLOG_TEMPERATURE_HOTSPOT.supported:%d\n", odlpDataOutput.sensors[PMLOG_TEMPERATURE_HOTSPOT].supported);
                printf("PMLOG_TEMPERATURE_HOTSPOT.value:%d\n", odlpDataOutput.sensors[PMLOG_TEMPERATURE_HOTSPOT].value);

                printf("ADLSensorType: PMLOG_TEMPERATURE_MEM - Memory Temp\n");
                printf("PMLOG_TEMPERATURE_MEM.supported:%d\n", odlpDataOutput.sensors[PMLOG_TEMPERATURE_MEM].supported);
                printf("PMLOG_TEMPERATURE_MEM.value:%d\n", odlpDataOutput.sensors[PMLOG_TEMPERATURE_MEM].value);
                printf("-----------------------------------------\n");

            }
            else
                printf("OD8 Failed to get Temperature Settings\n");
        }
        else
        {
            printf("ADL2_New_QueryPMLogData_Get is failed\n");
            return false;
        }
    }
    return true;
}

bool AdlManager::readAdl2Overdrive8FanSettingParameters()
{
	int ret = -1;
    //ret = ADL2_Overdrive_Caps(m_context, m_adapterIndex, &iSupported, &iEnabled, &iVersion);
    if (m_overdriveVersion == 8)
    {
        //OD8 initial Status
//        ADLOD8InitSetting m_od8InitSetting;
//        if (ADL_OK != GetOD8InitSetting(m_adapterIndex, m_od8InitSetting))
//        {
//            printf("Get Init Setting failed.\n");
//            return false;
//        }

//        //OD8 Current Status
//        ADLOD8CurrentSetting m_od8CurrentSetting;
//        if (ADL_OK != GetOD8CurrentSetting(m_adapterIndex, m_od8CurrentSetting))
//        {
//            printf("Get Current Setting failed.\n");
//            return false;
//        }

        //make ADL call for VEGA12
        ADLPMLogDataOutput odlpDataOutput;
        memset(&odlpDataOutput, 0, sizeof(ADLPMLogDataOutput));
        ret = ADL2_New_QueryPMLogData_Get(m_context, m_adapterIndex, &odlpDataOutput);
        if (0 == ret)
        {
            if ((m_od8InitSetting.overdrive8Capabilities & ADL_OD8_ACOUSTIC_LIMIT_SCLK) == ADL_OD8_ACOUSTIC_LIMIT_SCLK ||
                (m_od8InitSetting.overdrive8Capabilities & ADL_OD8_FAN_SPEED_MIN) == ADL_OD8_FAN_SPEED_MIN)
            {
                //OverdriveRangeDataStruct oneRangeData;
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_FAN_MIN_SPEED, ADL_OD8_FAN_SPEED_MIN);
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_FAN_ACOUSTIC_LIMIT, ADL_OD8_ACOUSTIC_LIMIT_SCLK);

                printf("ADLSensorType: PMLOG_FAN_RPM\n");
                printf("PMLOG_FAN_RPM.supported:%d\n", odlpDataOutput.sensors[PMLOG_FAN_RPM].supported);
                printf("PMLOG_FAN_RPM.value:%d\n", odlpDataOutput.sensors[PMLOG_FAN_RPM].value);
                printf("-----------------------------------------\n");
            }
            // Fan Curve
            else if ((m_od8InitSetting.overdrive8Capabilities & ADL_OD8_FAN_CURVE) == ADL_OD8_FAN_CURVE)
            {
                printf("ADLSensorType: PMLOG_FAN_RPM\n");
                printf("OD8_FAN_RPM:%d\n", odlpDataOutput.sensors[PMLOG_FAN_RPM].value);
                printf("-----------------------------------------\n");
            }
            else
                printf("OD8 Failed to get Fan Settings\n");
        }
        else
        {
            printf("ADL2_New_QueryPMLogData_Get is failed\n");
            return false;
        }
    }
    return true;
}

bool AdlManager::readAdl2Overdrive8MemoryTimingSettingParameters()
{
	int ret = -1;
    //ADL2_Overdrive_Caps(m_context, m_adapterIndex, &iSupported, &iEnabled, &iVersion);
    if (m_overdriveVersion == 8)
    {
        //OD8 initial Status
//        ADLOD8InitSetting m_od8InitSetting;
//        if (ADL_OK != GetOD8InitSetting(m_adapterIndex, m_od8InitSetting))
//        {
//            printf("Get Init Setting failed.\n");
//            return false;
//        }

//        //OD8 Current Status
//        ADLOD8CurrentSetting m_od8CurrentSetting;
//        if (ADL_OK != GetOD8CurrentSetting(m_adapterIndex, m_od8CurrentSetting))
//        {
//            printf("Get Current Setting failed.\n");
//            return false;
//        }

        ADLPMLogDataOutput odlpDataOutput;
        memset(&odlpDataOutput, 0, sizeof(ADLPMLogDataOutput));
        ret = ADL2_New_QueryPMLogData_Get(m_context, m_adapterIndex, &odlpDataOutput);
        if (0 == ret)
        {
            if ((m_od8InitSetting.overdrive8Capabilities & ADL_OD8_MEMORY_TIMING_TUNE) == ADL_OD8_MEMORY_TIMING_TUNE)
            {
                //OverdriveRangeDataStruct oneRangeData;
                //GetOD8RangePrint(m_od8InitSetting, m_od8CurrentSetting, oneRangeData, OD8_AC_TIMING, ADL_OD8_MEMORY_TIMING_TUNE);
            }
            else
                printf("OD8 Failed to get Memory Timing Settings\n");
        }
        else
        {
            printf("ADL2_New_QueryPMLogData_Get is failed\n");
            return false;
        }
    }
    return true;
}