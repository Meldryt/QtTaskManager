
#include "AdlManager.h"
#include <string>
#include <iostream>

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
	setupOverdrive();
	setupDesktop();

	// Initialize legacy ADL transaction.Note that applications still can mix ADL and ADL2 API providing that only single 
	// transaction that uses legacy ADL APIs exists at any given time in the process. Numer of ADL2 transactions is not limited.  
	// The second parameter is 1, which means:
	// retrieve adapter information only for adapters that are physically present and enabled in the system
	if (ADL_OK != ADL2_Main_Control_Create(ADL_Main_Memory_Alloc, 1, &m_context))
	{
		printf("ADL Initialization Error!\n");
		return false;
	}

	readAdapterInfo();
	readMemoryInfo();
	readGraphicsDriverInfo();
	readBiosInfo();
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
		printf("ADL library not found!\n");
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

	if (nullptr == ADL2_Main_Control_Create ||
		nullptr == ADL2_Main_Control_Destroy)
	{
		printf("ADL's API is missing!\n");
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

	if (nullptr == ADL2_Adapter_NumberOfAdapters_Get ||
		nullptr == ADL2_Adapter_Active_Get ||
		nullptr == ADL2_Adapter_Primary_Get ||
		nullptr == ADL2_Adapter_AdapterInfo_Get ||
		nullptr == ADL2_Adapter_MemoryInfo3_Get ||
		nullptr == ADL2_Adapter_VideoBiosInfo_Get ||
		nullptr == ADL2_Adapter_PMLog_Support_Get ||
		nullptr == ADL2_Adapter_PMLog_Start ||
		nullptr == ADL2_Adapter_PMLog_Stop)
	{
		printf("ADL's API is missing!\n");
		return ADL_ERR;
	}

	return ADL_OK;
}

bool AdlManager::setupDisplay()
{
	ADL2_Display_Modes_Get = (ADL2_DISPLAY_MODES_GET)GetProcAddress(m_hDLL, "ADL2_Display_Modes_Get");
	ADL2_Display_DisplayInfo_Get = (ADL2_DISPLAY_DISPLAYINFO_GET)GetProcAddress(m_hDLL, "ADL2_Display_DisplayInfo_Get");

	ADL2_Display_Property_Get = (ADL2_DISPLAY_PROPERTY_GET)GetProcAddress(m_hDLL, "ADL2_Display_Property_Get");
	ADL2_Display_DCE_Get = (ADL2_DISPLAY_DCE_GET)GetProcAddress(m_hDLL, "ADL2_Display_DCE_Get");

	if (nullptr == ADL2_Display_Modes_Get ||
		nullptr == ADL2_Display_DisplayInfo_Get ||
		nullptr == ADL2_Display_Property_Get ||
		nullptr == ADL2_Display_DCE_Get)
	{
		printf("ADL's API is missing!\n");
		return ADL_ERR;
	}

	return ADL_OK;
}

bool AdlManager::setupGraphics()
{
	ADL2_Graphics_VersionsX2_Get = (ADL2_GRAPHICS_VERSIONSX2_GET)GetProcAddress(m_hDLL, "ADL2_Graphics_VersionsX2_Get");

	if (nullptr == ADL2_Graphics_VersionsX2_Get)
	{
		printf("ADL's API is missing!\n");
		return false;
	}

	return true;
}

bool AdlManager::setupOverdrive()
{
	ADL_Overdrive_Caps = (ADL_OVERDRIVE_CAPS)GetProcAddress(m_hDLL, "ADL_Overdrive_Caps");
	ADL2_Overdrive_Caps = (ADL2_OVERDRIVE_CAPS)GetProcAddress(m_hDLL, "ADL2_Overdrive_Caps");

	if (nullptr == ADL_Overdrive_Caps ||
		nullptr == ADL2_Overdrive_Caps)
	{
		printf("ADL's API is missing!\n");
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

			if (nullptr == ADL_Overdrive5_ThermalDevices_Enum ||
				nullptr == ADL_Overdrive5_Temperature_Get ||
				nullptr == ADL_Overdrive5_FanSpeedInfo_Get ||
				nullptr == ADL_Overdrive5_ODPerformanceLevels_Get ||
				nullptr == ADL_Overdrive5_ODParameters_Get ||
				nullptr == ADL_Overdrive5_CurrentActivity_Get ||
				nullptr == ADL_Overdrive5_PowerControl_Caps ||
				nullptr == ADL_Overdrive5_PowerControlInfo_Get ||
				nullptr == ADL_Overdrive5_PowerControl_Get)
			{
				printf("ADL's API is missing!\n");
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

			if (nullptr == ADL_Overdrive6_FanSpeed_Get ||
				nullptr == ADL_Overdrive6_ThermalController_Caps ||
				nullptr == ADL_Overdrive6_Temperature_Get ||
				nullptr == ADL_Overdrive6_Capabilities_Get ||
				nullptr == ADL_Overdrive6_StateInfo_Get ||
				nullptr == ADL_Overdrive6_CurrentStatus_Get ||
				nullptr == ADL_Overdrive6_PowerControl_Caps ||
				nullptr == ADL_Overdrive6_PowerControlInfo_Get ||
				nullptr == ADL_Overdrive6_PowerControl_Get)
			{
				printf("ADL's API is missing!\n");
				return false;
			}
		}
	}
	else if (m_adlOverdriveVersion == 2)
	{
		ADL2_WS_Overdrive_Caps = (ADL2_WS_OVERDRIVE_CAPS)GetProcAddress(m_hDLL, "ADL2_WS_Overdrive_Caps");

		if (nullptr == ADL2_WS_Overdrive_Caps)
		{
			printf("ADL's API is missing!\n");
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

			if (nullptr == ADL2_Overdrive5_ThermalDevices_Enum ||
				nullptr == ADL2_Overdrive5_Temperature_Get ||
				nullptr == ADL2_Overdrive5_FanSpeedInfo_Get ||
				nullptr == ADL2_Overdrive5_ODPerformanceLevels_Get ||
				nullptr == ADL2_Overdrive5_ODParameters_Get ||
				nullptr == ADL2_Overdrive5_CurrentActivity_Get ||
				nullptr == ADL2_Overdrive5_PowerControl_Caps ||
				nullptr == ADL2_Overdrive5_PowerControlInfo_Get ||
				nullptr == ADL2_Overdrive5_PowerControl_Get)
			{
				printf("ADL's API is missing!\n");
				return false;
			}
		}
		else if (m_overdriveVersion == 6)
		{
			ADL2_Overdrive8_Init_SettingX2_Get = (ADL2_OVERDRIVE8_INIT_SETTINGX2_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive8_Init_SettingX2_Get");
			ADL2_Overdrive8_Current_SettingX2_Get = (ADL2_OVERDRIVE8_CURRENT_SETTINGX2_GET)GetProcAddress(m_hDLL, "ADL2_Overdrive8_Current_SettingX2_Get");

			if (nullptr == ADL2_Overdrive8_Init_SettingX2_Get ||
				nullptr == ADL2_Overdrive8_Current_SettingX2_Get)
			{
				printf("ADL's API is missing!\n");
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

	if (nullptr == ADL2_Desktop_Device_Create ||
		nullptr == ADL2_Desktop_Device_Destroy)
	{
		printf("ADL's API is missing!\n");
		return false;
	}

	return true;
}

bool AdlManager::fetchInfo()
{
	PrepareAPI();

	//ADLVersionsInfoX2 versionsInfo;
	//ADL2_Graphics_VersionsX2_Get(m_context, &versionsInfo);

	//int  iNumberAdapters;

	//// Obtain the number of adapters for the system
	//if (ADL_OK != ADL2_Adapter_NumberOfAdapters_Get(m_context, &iNumberAdapters))
	//{
	//	printf("Cannot get the number of adapters!\n");
	//	return false;
	//}

	//for (int adapterId = 0; adapterId < iNumberAdapters; adapterId++)
	//{
	//	if (ADL_OK != PrintAdapterInfo(adapterId))
	//		break;
	//}

	////Finishing legacy ADL transaction
	//if (ADL_OK != ADL2_Main_Control_Destroy(m_context))
	//{
	//	printf("Failed to destroy ADL context");
	//}

	return true;
}


//Retrieves active status of given adapter. Implements the retrieval as isolated ADL2 transaction.
//Note that the function can be nested inside another ADL2 transaction without interfering with it.
//In real application it would be much more efficient to share the same context with the parent transaction by passing the context handle in the function argument list.   
int AdlManager::GetAdapterActiveStatus(int adapterId, int& active)
{
	active = 0;

	if (ADL_OK != ADL2_Adapter_Active_Get(m_context, adapterId, &active))
	{
		printf("Failed to get adapter status");
	}
	return ADL_OK;
}

//Demonstrates execution of multiple nested ADL2 transactions that are executed on the same thread. 
//Uncoordinated ADL2 transactions can be also executed on separate thread. 
int AdlManager::PrintAdapterInfo(int adapterId)
{
	int active = 0;

	//Invoking additional nested ADL2 based transaction on the same thread to demonstrate that multiple ADL2 transactions can be executed at the same time inside 
	//the process without interfering. Not the most efficient way to work with ADL. In real application it would be much more efficient to re-use  context of parent
	//transaction by passing it to GetAdapterActiveStatus.  
	if (ADL_OK == GetAdapterActiveStatus(adapterId, active))
	{
		printf("*************************************************\n");

		printf("Adapter %d is %s\n", adapterId, (active) ? "active" : "not active");
		if (active)
		{
			int numModes;
			ADLMode* adlMode;

			if (ADL_OK == ADL2_Display_Modes_Get(m_context, adapterId, -1, &numModes, &adlMode))
			{
				if (numModes == 1)
				{
					printf("Adapter %d resolution is %d by %d\n", adapterId, adlMode->iXRes, adlMode->iYRes);
					ADL_Main_Memory_Free(adlMode);
				}
			}
		}
	}

	return ADL_OK;
}

//Destroy ADL. ALD calls can't be called after the method is invoked;
void AdlManager::DestroyADL()
{
	if (NULL != ADL2_Main_Control_Destroy)
	{
		ADL2_Main_Control_Destroy(m_context);
	}

	if (m_hDLL)
	{
		FreeLibrary(m_hDLL);
	}
}

int AdlManager::GpuBDF(int busNo_, int devNo_, int funcNo_)
{
	return ((busNo_ & 0xFF) << 8) | ((devNo_ & 0x1F) << 3) | (funcNo_ & 0x07);
}

std::string AdlManager::GetGPUVRAMNameFromID(int iVramVendorRevId)
{
	switch (iVramVendorRevId)
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
	int iNumberAdapters = 0;

	if (ADL_OK == ADL2_Adapter_NumberOfAdapters_Get(m_context, &iNumberAdapters))
	{
		int primary = -1;
		ADL2_Adapter_Primary_Get(m_context, &primary);

		AdapterInfo* infos = new AdapterInfo[iNumberAdapters];
		if (ADL_OK == ADL2_Adapter_AdapterInfo_Get(m_context, infos, sizeof(AdapterInfo) * iNumberAdapters))
		{
			for (uint32_t adapterIdx = 0; adapterIdx < iNumberAdapters; ++adapterIdx)
			{
				int active = 0;
				if (ADL_OK == ADL2_Adapter_Active_Get(m_context, adapterIdx, &active) && active/* && primary == adapterIdx*/)
				{
					m_adapterInfo = infos[adapterIdx];
					m_adapterIndex = m_adapterInfo.iAdapterIndex;
					m_model = m_adapterInfo.strAdapterName;
					break;
				}
			}

			delete[] infos;
		}
	}
}

void AdlManager::readMemoryInfo()
{
	ADLMemoryInfo3 adlMemInfo3;
	if (ADL_OK == ADL2_Adapter_MemoryInfo3_Get(m_context, m_adapterInfo.iAdapterIndex, &adlMemInfo3))
	{
		m_memorySize = adlMemInfo3.iMemorySize / 1024 / 1024 / 1000;
		m_memoryType = adlMemInfo3.strMemoryType;
		m_memoryBandwidth = adlMemInfo3.iMemoryBandwidth / 1024;

		if (adlMemInfo3.iVramVendorRevId == ADLvRamVendor_Unsupported == adlMemInfo3.iVramVendorRevId)
		{
			printf("\tGPU Vedio RAM vendor ID Unsupport, only support AMD dGPU now.\n");
		}
		else
		{
			m_memoryVendor = GetGPUVRAMNameFromID(adlMemInfo3.iVramVendorRevId);
		}
	}
}

void AdlManager::readGraphicsDriverInfo()
{
	if (ADL2_Graphics_VersionsX2_Get) 
	{
		ADLVersionsInfoX2 versionsInfo;
		int ADLResult = ADL2_Graphics_VersionsX2_Get(m_context, &versionsInfo);
		if (ADL_OK == ADLResult || ADL_OK_WARNING == ADLResult) 
		{
			m_driverInfo = versionsInfo.strDriverVer;

			std::string catalystVersion = std::string(versionsInfo.strCatalystVersion);
			if (catalystVersion.empty())
			{
				m_driverVersion = versionsInfo.strCrimsonVersion;
			}
			else
			{
				m_driverVersion = catalystVersion;
			}	
		}
	}
	else if (ADL2_Graphics_Versions_Get) 
	{
		ADLVersionsInfo versionsInfo;
		int ADLResult = ADL2_Graphics_Versions_Get(m_context, &versionsInfo);
		if (ADL_OK == ADLResult || ADL_OK_WARNING == ADLResult) 
		{
			m_driverInfo = versionsInfo.strDriverVer;
			m_driverVersion = versionsInfo.strCatalystVersion;
		}
	}
}

void AdlManager::readBiosInfo()
{
	ADLBiosInfo biosInfo;
	if (ADL_OK == ADL2_Adapter_VideoBiosInfo_Get(m_context, m_adapterIndex, &biosInfo)) {
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

#define ADL_WARNING_NO_DATA      -100

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
	else
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

void AdlManager::readOverdriveVersion()
{
	int iOverdriveSupported = 0;
	int iOverdriveEnabled = 0;
	int	iOverdriveVersion = 0;
	int result;

	result = ADL_Overdrive_Caps(m_adapterIndex, &iOverdriveSupported, &iOverdriveEnabled, &iOverdriveVersion);
	if (result != ADL_OK)
	{
		result = ADL2_Overdrive_Caps(m_context, m_adapterIndex, &iOverdriveSupported, &iOverdriveEnabled, &iOverdriveVersion);
		if(result != ADL_OK)
		{
		}
		else
		{
			m_adlOverdriveVersion = 2;
			printf("Found ADL2_Overdrive_Caps \n");
		}
	}
	else
	{
		m_adlOverdriveVersion = 1;
		printf("Found ADL_Overdrive_Caps \n");
	}

	if (!iOverdriveSupported)
	{
		printf("Overdrive is not supported \n");
	}

	m_overdriveSupported = (iOverdriveSupported == 1);
	m_overdriveEnabled = (iOverdriveEnabled == 1);
	m_overdriveVersion = iOverdriveVersion;
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

bool AdlManager::readAdlOverdrive5Temperature()
{
	ADLThermalControllerInfo termalControllerInfo = { 0 };
	termalControllerInfo.iSize = sizeof(ADLThermalControllerInfo);

	ADLTemperature temperatureInfo;
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
			if (ADL_OK != ADL2_Overdrive5_Temperature_Get(m_context, m_adapterIndex, iThermalControllerIndex, &adlTemperature))
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

bool AdlManager::readAdlOverdrive5FanSpeed()
{
	if (!m_fanSpeedInfo)
	{
		m_fanSpeedInfo = new ADLFanSpeedInfo();
		m_fanSpeedInfo->iSize = sizeof(ADLFanSpeedInfo);

		if (ADL_OK != ADL_Overdrive5_FanSpeedInfo_Get(m_adapterIndex, m_thermalDomainControllerIndex, m_fanSpeedInfo))
		{
			printf("Failed to get fan caps\n");
			return false;
		}

		m_fanSpeedReportingMethod = ((m_fanSpeedInfo->iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_READ) == ADL_DL_FANCTRL_SUPPORTS_RPM_READ) ? ADL_DL_FANCTRL_SPEED_TYPE_RPM : ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
	}

	ADLFanSpeedValue fanSpeedValue = { 0 };
	
	//Set to ADL_DL_FANCTRL_SPEED_TYPE_RPM or to ADL_DL_FANCTRL_SPEED_TYPE_PERCENT to request fan speed to be returned in rounds per minute or in percentage points.
	//Note that the call might fail if requested fan speed reporting method is not supported by the GPU.
	fanSpeedValue.iSpeedType = m_fanSpeedReportingMethod;
	if (ADL_OK != ADL_Overdrive5_FanSpeed_Get(m_adapterIndex, m_thermalDomainControllerIndex, &fanSpeedValue))
	{
		printf("Failed to get fan speed\n");
		return false;
	}

	if (m_fanSpeedReportingMethod == ADL_DL_FANCTRL_SPEED_TYPE_RPM)
	{
		printf("Current fan speed: %d rpm\n", fanSpeedValue.iFanSpeed);
		printf("Minimum fan speed: %d rpm\n", m_fanSpeedInfo->iMinRPM);
		printf("Maximum fan speed: %d rpm\n", m_fanSpeedInfo->iMaxRPM);
	}
	else
	{
		printf("Current fan speed: %d percent\n", fanSpeedValue.iFanSpeed);
		printf("Minimum fan speed: %d percent\n", m_fanSpeedInfo->iMinPercent);
		printf("Maximum fan speed: %d percent\n", m_fanSpeedInfo->iMaxPercent);
	}

	return true;
}

bool AdlManager::readAdlOverdrive5PowerControl()
{
	int powerControlSupported = 0;
	ADLPowerControlInfo powerControlInfo = { 0 };
	int powerControlCurrent = 0;
	int powerControlDefault = 0;

	if (ADL_OK != ADL_Overdrive5_PowerControl_Caps(m_adapterIndex, &powerControlSupported))
	{
		printf("Failed to get Power Controls support\n");
		return false;
	}

	if (powerControlSupported)
	{
		if (ADL_OK != ADL_Overdrive5_PowerControlInfo_Get(m_adapterIndex, &powerControlInfo))
		{
			printf("Failed to get Power Controls Info\n");
			return false;
		}

		if (ADL_OK != ADL_Overdrive5_PowerControl_Get(m_adapterIndex, &powerControlCurrent, &powerControlDefault))
		{
			printf("Failed to get Power Control current value\n");
			return false;
		}

		printf("The Power Control threshold range is %d to %d with step of %d \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue, powerControlInfo.iStepValue);
		printf("Current value of Power Control threshold is %d \n", powerControlCurrent);
	}

	return true;
}

bool AdlManager::readAdlOverdrive5ODParameters()
{
	if (!m_overdriveParameters)
	{
		m_overdriveParameters = new ADLODParameters();
		m_overdriveParameters->iSize = sizeof(ADLODParameters);

		if (ADL_OK != ADL_Overdrive5_ODParameters_Get(m_adapterIndex, m_overdriveParameters))
		{
			printf("Failed to get overdrive parameters\n");
			return 0;
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


		if (ADL_OK != ADL_Overdrive5_ODPerformanceLevels_Get(m_adapterIndex, 1/*Getting default values first*/, performanceLevels))
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

		if (ADL_OK != ADL_Overdrive5_ODPerformanceLevels_Get(m_adapterIndex, 0/*Getting current values first*/, performanceLevels))
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

bool AdlManager::readAdlOverdrive5CurrentActivity()
{
	//Getting real current values for clocks, performance levels, voltage effective in the system.
	ADLPMActivity activity = { 0 };
	activity.iSize = sizeof(ADLPMActivity);
	if (ADL_OK != ADL_Overdrive5_CurrentActivity_Get(m_adapterIndex, &activity))
	{
		printf("Failed to get current GPU activity.  \n");
		return 0;
	}
	printf("Current Engine Clock: %d MHz\n", activity.iEngineClock / 100);
	printf("Current Memory Clock: %d MHz\n", activity.iMemoryClock / 100);
	printf("Current Core Voltage: %d \n", activity.iVddc);
	printf("Current Performance Level: %d \n", activity.iCurrentPerformanceLevel);

	if (m_overdriveParameters->iActivityReportingSupported)
	{
		printf("Current Engine Clock: %d persent\n", activity.iActivityPercent);
	}

	//...setter...
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
	int temperature = 0;

	if (!m_thermalControllerCaps)
	{
		m_thermalControllerCaps = new ADLOD6ThermalControllerCaps();

		if (ADL_OK != ADL_Overdrive6_ThermalController_Caps(m_adapterIndex, m_thermalControllerCaps))
		{
			printf("Failed to get thermal controller capabilities \n");
			return false;
		}
	}

	if (ADL_OD6_TCCAPS_THERMAL_CONTROLLER == (m_thermalControllerCaps->iCapabilities & ADL_OD6_TCCAPS_THERMAL_CONTROLLER)) //Verifies that thermal controller exists on the GPU.
	{
		if (ADL_OK != ADL_Overdrive6_Temperature_Get(m_adapterIndex, &temperature))
		{
			printf("Failed to get GPU temperature\n");
			return false;
		}

		printf("GPU temperature is %d degrees celsius \n", temperature / 1000); //The temperature is returned in mili-degree of Celsius 
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

			if (ADL_OK != ADL_Overdrive6_FanSpeed_Get(m_adapterIndex, &fanSpeedInfo))
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
		}
	}

	return true;
}

bool AdlManager::readAdlOverdrive6CurrentStatus()
{
	ADLOD6Capabilities od6Capabilities = { 0 };
	ADLOD6CurrentStatus currentStatus = { 0 };

	if (ADL_OK != ADL_Overdrive6_Capabilities_Get(m_adapterIndex, &od6Capabilities))
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
	if (ADL_OK != ADL_Overdrive6_StateInfo_Get(m_adapterIndex, ADL_OD6_GETSTATEINFO_DEFAULT_PERFORMANCE, defaultStateInfo))
	{
		printf("Failed to get default performance levels info\n");
		return false;
	}

	ADLOD6StateInfo* customStateInfo = (ADLOD6StateInfo*)malloc(size);
	memset((void*)customStateInfo, 0, size);
	customStateInfo->iNumberOfPerformanceLevels = 2;

	//Getting default effective minimum and maximum values for memory and GPU clocks.
	//The only state supported by Overdrive6 is "Performance". 
	if (ADL_OK != ADL_Overdrive6_StateInfo_Get(m_adapterIndex, ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE, customStateInfo))
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

	if (ADL_OK != ADL_Overdrive6_CurrentStatus_Get(m_adapterIndex, &currentStatus))
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

	if (ADL_OK != ADL_Overdrive6_PowerControl_Caps(m_adapterIndex, &powerControlSupported))
	{
		printf("Failed to get power control capabilities\n");
		return false;
	}

	if (powerControlSupported)
	{
		if (ADL_OK != ADL_Overdrive6_PowerControlInfo_Get(m_adapterIndex, &powerControlInfo))
		{
			printf("Failed to get power control information\n");
			return false;
		}

		if (ADL_OK != ADL_Overdrive6_PowerControl_Get(m_adapterIndex, &powerControlCurrent, &powerControlDefault))
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

	ADLTemperature temperatureInfo;
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
			if (ADL_OK != ADL2_Overdrive5_Temperature_Get(m_context, m_adapterIndex, iThermalControllerIndex, &adlTemperature))
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
	if (!m_fanSpeedInfo)
	{
		m_fanSpeedInfo = new ADLFanSpeedInfo();
		m_fanSpeedInfo->iSize = sizeof(ADLFanSpeedInfo);

		if (ADL_OK != ADL_Overdrive5_FanSpeedInfo_Get(m_adapterIndex, m_thermalDomainControllerIndex, m_fanSpeedInfo))
		{
			printf("Failed to get fan caps\n");
			return false;
		}

		m_fanSpeedReportingMethod = ((m_fanSpeedInfo->iFlags & ADL_DL_FANCTRL_SUPPORTS_RPM_READ) == ADL_DL_FANCTRL_SUPPORTS_RPM_READ) ? ADL_DL_FANCTRL_SPEED_TYPE_RPM : ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;
	}

	ADLFanSpeedValue fanSpeedValue = { 0 };

	//Set to ADL_DL_FANCTRL_SPEED_TYPE_RPM or to ADL_DL_FANCTRL_SPEED_TYPE_PERCENT to request fan speed to be returned in rounds per minute or in percentage points.
	//Note that the call might fail if requested fan speed reporting method is not supported by the GPU.
	fanSpeedValue.iSpeedType = m_fanSpeedReportingMethod;
	if (ADL_OK != ADL_Overdrive5_FanSpeed_Get(m_adapterIndex, m_thermalDomainControllerIndex, &fanSpeedValue))
	{
		printf("Failed to get fan speed\n");
		return false;
	}

	if (m_fanSpeedReportingMethod == ADL_DL_FANCTRL_SPEED_TYPE_RPM)
	{
		printf("Current fan speed: %d rpm\n", fanSpeedValue.iFanSpeed);
		printf("Minimum fan speed: %d rpm\n", m_fanSpeedInfo->iMinRPM);
		printf("Maximum fan speed: %d rpm\n", m_fanSpeedInfo->iMaxRPM);
	}
	else
	{
		printf("Current fan speed: %d percent\n", fanSpeedValue.iFanSpeed);
		printf("Minimum fan speed: %d percent\n", m_fanSpeedInfo->iMinPercent);
		printf("Maximum fan speed: %d percent\n", m_fanSpeedInfo->iMaxPercent);
	}

	return true;
}

bool AdlManager::readAdl2Overdrive5PowerControl()
{
	int powerControlSupported = 0;
	ADLPowerControlInfo powerControlInfo = { 0 };
	int powerControlCurrent = 0;
	int powerControlDefault = 0;

	if (ADL_OK != ADL2_Overdrive5_PowerControl_Caps(m_context, m_adapterIndex, &powerControlSupported))
	{
		printf("Failed to get Power Controls support\n");
		return false;
	}

	if (powerControlSupported)
	{
		if (ADL_OK != ADL2_Overdrive5_PowerControlInfo_Get(m_context, m_adapterIndex, &powerControlInfo))
		{
			printf("Failed to get Power Controls Info\n");
			return false;
		}

		if (ADL_OK != ADL2_Overdrive5_PowerControl_Get(m_context, m_adapterIndex, &powerControlCurrent, &powerControlDefault))
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

		if (ADL_OK != ADL2_Overdrive5_ODParameters_Get(m_context, m_adapterIndex, m_overdriveParameters))
		{
			printf("Failed to get overdrive parameters\n");
			return 0;
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


		if (ADL_OK != ADL2_Overdrive5_ODPerformanceLevels_Get(m_context, m_adapterIndex, 1/*Getting default values first*/, performanceLevels))
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

		if (ADL_OK != ADL2_Overdrive5_ODPerformanceLevels_Get(m_context, m_adapterIndex, 0/*Getting current values first*/, performanceLevels))
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
	//Getting real current values for clocks, performance levels, voltage effective in the system.
	ADLPMActivity activity = { 0 };
	activity.iSize = sizeof(ADLPMActivity);
	if (ADL_OK != ADL2_Overdrive5_CurrentActivity_Get(m_context, m_adapterIndex, &activity))
	{
		printf("Failed to get current GPU activity.  \n");
		return 0;
	}
	printf("Current Engine Clock: %d MHz\n", activity.iEngineClock / 100);
	printf("Current Memory Clock: %d MHz\n", activity.iMemoryClock / 100);
	printf("Current Core Voltage: %d \n", activity.iVddc);
	printf("Current Performance Level: %d \n", activity.iCurrentPerformanceLevel);

	if (m_overdriveParameters->iActivityReportingSupported)
	{
		printf("Current Engine Clock: %d persent\n", activity.iActivityPercent);
	}

	//...setter...
}

bool AdlManager::readAdl2Overdrive6Info()
{
	return true;
}

bool AdlManager::readAdl2Overdrive8Info()
{
	return true;
}

void AdlManager::PrepareAPI()
{

	//	int numModes;
	//	ADLMode* adlMode;
	//	if (ADL_OK == ADL2_Display_Modes_Get(m_context, adapterIdx, -1, &numModes, &adlMode)) {
	//		printf("\tDisplay Modes\n");
	//		if (numModes == 1) {
	//			printf("\t\t%dx%d\n", adlMode->iXRes, adlMode->iYRes);
	//			ADL_Main_Memory_Free(adlMode);
	//		}
	//	}

	//	if (ADL2_Adapter_Aspects_Get) {
	//		int iSize = ADL_MAX_CHAR;
	//		char lpAspects[ADL_MAX_CHAR];
	//		if (ADL_OK == ADL2_Adapter_Aspects_Get(m_context, adapterIdx, lpAspects, iSize)) {
	//			printf("\tAspects: %s\n", lpAspects);
	//		}
	//	}

	//	if (ADL2_PowerXpress_AncillaryDevices_Get) {
	//		int  numberOfAncillaryDevices = 0;
	//		ADLBdf* lpAncillaryDevices = nullptr;
	//		if (ADL_OK == ADL2_PowerXpress_AncillaryDevices_Get(m_context, adapterIdx, &numberOfAncillaryDevices, &lpAncillaryDevices)) {
	//			printf("\tPowerXpress Ancillary Devices\n");
	//			for (int i = 0; i < numberOfAncillaryDevices; ++i) {
	//				ADLBdf bdf = lpAncillaryDevices[i];
	//				printf("\t\tLocation: PCI bus %i, device %i, function %i\n", bdf.iBus, bdf.iDevice, bdf.iFunction);
	//			}
	//		}
	//	}
	//}
}

void AdlManager::Get_All_DisplayInfo(int adapterIndex)
{
	int numDisplays = 0;
	ADLDisplayInfo* allDisplaysBuffer = NULL;

	if (ADL_OK == ADL2_Display_DisplayInfo_Get(m_context, adapterIndex, &numDisplays, &allDisplaysBuffer, 1)) {
		for (int i = 0; i < numDisplays; i++) {
			ADLDisplayInfo* oneDis = &(allDisplaysBuffer[i]);
			if (ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED == (oneDis->iDisplayInfoValue & ADL_DISPLAY_DISPLAYINFO_DISPLAYCONNECTED)) {
				ADLDisplayID display(oneDis->displayID);
				m_DisplayID.insert(std::pair<int, ADLDisplayID>(i, display));
			}

		}

	}

	ADL_Main_Memory_Free((void**)&allDisplaysBuffer);
}

int AdlManager::printApplicationProfilesX3(ADL_CONTEXT_HANDLE context, int iListType) {
	int numApps = 0;
	if (ADL2_ApplicationProfiles_HitListsX3_Get) {
		ADLApplicationDataX3* profiles = NULL;
		if (ADL_OK == ADL2_ApplicationProfiles_HitListsX3_Get(context, iListType, &numApps, &profiles)) {
			wprintf(L"\t[N] File,Path,Time,Version,ProcessId\n");
			for (int i = 0; i < numApps; ++i) {
				ADLApplicationDataX3 profile = profiles[i];
				wprintf(L"\t[%i] %s,%s,%s,%s,%08x\n",
					i,
					profile.strFileName,
					profile.strPathName,
					profile.strTimeStamp,
					profile.strVersion,
					profile.iProcessId
				);
			}
		}
	}
	return numApps;
}


//		{
//			printf("ADL_PX40_MRU\n");
//			printApplicationProfilesX3(context, ADL_PX40_MRU);
//
//			printf("ADL_PX40_MISSED\n");
//			printApplicationProfilesX3(context, ADL_PX40_MISSED);
//
//			printf("ADL_PX40_DISCRETE\n");
//			printApplicationProfilesX3(context, ADL_PX40_DISCRETE);
//
//			printf("ADL_PX40_INTEGRATED\n");
//			printApplicationProfilesX3(context, ADL_PX40_INTEGRATED);
//
//			printf("ADL_MMD_PROFILED\n");
//			printApplicationProfilesX3(context, ADL_MMD_PROFILED);
//
//			printf("ADL_PX40_TOTAL\n");
//			printApplicationProfilesX3(context, ADL_PX40_TOTAL);
//		}
//
//		ADL2_Main_Control_Destroy(context);
//	}
//
//	return 0;
//}