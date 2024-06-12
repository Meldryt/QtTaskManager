#pragma once

#include <windows.h>
#include <tchar.h>
#include "../external/adl/include/adl_sdk.h"
#include <map>
#include <vector>
#include <string>

class AdlManager
{
public:
    AdlManager();
    bool init();

    bool fetchInfo();

    int GetAdapterActiveStatus(int adapterId, int& active);
    int PrintAdapterInfo(int adapterId);
    void DestroyADL();
    int GpuBDF(int busNo_, int devNo_, int funcNo_);
    std::string GetGPUVRAMNameFromID(int iVramVendorRevId);
    void PrepareAPI();
    void Get_All_DisplayInfo(int adapterIndex);

    int printApplicationProfilesX3(ADL_CONTEXT_HANDLE context, int iListType);
    int test();

private:
    bool loadLibrary();
    bool setupMainControl();
    bool setupAdapter();
    bool setupDisplay();
    bool setupGraphics();
    bool setupOverdrive();
    bool setupDesktop();

    void readAdapterInfo();
    void readMemoryInfo();
    void readGraphicsDriverInfo();
    void readBiosInfo();
    void readOverdriveInfo();
    void readOverdriveVersion();

    bool readAdlOverdrive5Info();
    bool readAdlOverdrive5Temperature();
    bool readAdlOverdrive5FanSpeed();
    bool readAdlOverdrive5PowerControl();
    bool readAdlOverdrive5ODParameters();
    bool readAdlOverdrive5PerformanceLevel();
    bool readAdlOverdrive5CurrentActivity();

    bool readAdlOverdrive6Info();
    bool readAdlOverdrive6Temperature();
    bool readAdlOverdrive6FanSpeed();
    bool readAdlOverdrive6CurrentStatus();
    bool readAdlOverdrive6PowerControl();

    bool readAdl2Overdrive5Info();
    bool readAdl2Overdrive5Temperature();
    bool readAdl2Overdrive5FanSpeed();
    bool readAdl2Overdrive5PowerControl();
    bool readAdl2Overdrive5ODParameters();
    bool readAdl2Overdrive5PerformanceLevel();
    bool readAdl2Overdrive5CurrentActivity();
    bool readAdl2Overdrive6Info();
    bool readAdl2Overdrive8Info();

    // ADL module handle
#if defined (LINUX)
    void* m_hDLL{ nullptr };		// Handle to .so library
#else
    HINSTANCE m_hDLL = NULL;		// Handle to DLL
#endif

    ADL_CONTEXT_HANDLE m_context = NULL;
    //std::map<int, int> m_AdapterIndexMap;
    std::map<int, ADLDisplayID> m_DisplayID;
    
    
    //Declaring pointers to ADL Methods. 
    //ADL2_MAIN_CONTROL_CREATE                 ADL2_Main_Control_Create = nullptr;
    //ADL2_MAIN_CONTROLX2_CREATE               ADL2_Main_ControlX2_Create;
    //ADL2_MAIN_CONTROL_DESTROY                ADL2_Main_Control_Destroy;
    //ADL2_ADAPTER_NUMBEROFADAPTERS_GET        ADL2_Adapter_NumberOfAdapters_Get;
    //ADL2_ADAPTER_PRIMARY_GET                 ADL2_Adapter_Primary_Get;
    //ADL2_ADAPTER_ADAPTERINFO_GET             ADL2_Adapter_AdapterInfo_Get;
    //ADL2_ADAPTER_ACTIVE_GET                  ADL2_Adapter_Active_Get;
    //ADL2_ADAPTER_REGVALUEINT_SET             ADL2_Adapter_RegValueInt_Set;
    //ADL2_ADAPTER_REGVALUEINT_GET             ADL2_Adapter_RegValueInt_Get;
    //ADL2_ADAPTER_REGVALUESTRING_SET			ADL2_Adapter_RegValueString_Set;
    //ADL2_ADAPTER_REGVALUESTRING_GET			ADL2_Adapter_RegValueString_Get;
    //
    //ADL2_DFP_GPUSCALINGENABLE_SET            ADL2_DFP_GPUScalingEnable_Set;
    //ADL2_DFP_GPUSCALINGENABLE_GET            ADL2_DFP_GPUScalingEnable_Get;
    //ADL2_DISPLAY_PRESERVEDASPECTRATIO_SET    ADL2_Display_PreservedAspectRatio_Set;
    //ADL2_DISPLAY_PRESERVEDASPECTRATIO_GET	ADL2_Display_PreservedAspectRatio_Get;
    //ADL2_DISPLAY_IMAGEEXPANSION_GET			ADL2_Display_ImageExpansion_Get;
    //ADL2_DISPLAY_IMAGEEXPANSION_SET          ADL2_Display_ImageExpansion_Set;
    //ADL2_DISPLAY_DISPLAYINFO_GET             ADL2_Display_DisplayInfo_Get;
    //ADL2_ADAPTER_ADAPTERINFOX4_GET           ADL2_Adapter_AdapterInfoX4_Get;
    //
    //ADL2_DISPLAY_FREESYNCSTATE_GET           ADL2_Display_FreeSyncState_Get;
    //ADL2_DISPLAY_FREESYNCSTATE_SET           ADL2_Display_FreeSyncState_Set;
    //
    //ADL2_DISPLAY_PROPERTY_GET                ADL2_Display_Property_Get;
    //ADL2_DISPLAY_PROPERTY_SET                ADL2_Display_Property_Set;
    //
    //ADL2_DISPLAY_COLORDEPTH_GET              ADL2_Display_ColorDepth_Get;
    //ADL2_DISPLAY_COLORDEPTH_SET              ADL2_Display_ColorDepth_Set;
    //ADL2_DISPLAY_SUPPORTEDCOLORDEPTH_GET     ADL2_Display_SupportedColorDepth_Get;
    //ADL2_DISPLAY_PIXELFORMAT_GET             ADL2_Display_PixelFormat_Get;
    //ADL2_DISPLAY_PIXELFORMAT_SET             ADL2_Display_PixelFormat_Set;
    //ADL2_DISPLAY_SUPPORTEDPIXELFORMAT_GET    ADL2_Display_SupportedPixelFormat_Get;
    //ADL2_DISPLAY_PIXELFORMATDEFAULT_GET      ADL2_Display_PixelFormatDefault_Get;
    //ADL2_DISPLAY_COLORTEMPERATURESOURCE_GET  ADL2_Display_ColorTemperatureSource_Get;
    //ADL2_DISPLAY_COLOR_GET                   ADL2_Display_Color_Get;
    //ADL2_DISPLAY_COLOR_SET                   ADL2_Display_Color_Set;
    //ADL2_DISPLAY_COLORTEMPERATURESOURCE_SET  ADL2_Display_ColorTemperatureSource_Set;
    //ADL2_CHILL_SETTINGS_SET					ADL2_Chill_Settings_Set;
    //ADL2_CHILL_SETTINGS_GET					ADL2_Chill_Settings_Get;
    //
    //ADL2_TURBOSYNCSUPPORT_GET				ADL2_TurboSyncSupport_Get;
    //ADL2_DISPLAY_MODES_GET				ADL2_Display_Modes_Get;
    //ADL2_GRAPHICS_VERSIONSX2_GET        ADL2_Graphics_VersionsX2_Get;

    // Definitions of the used function pointers. Add more if you use other ADL APIs. Note that that sample will use mixture of legacy ADL and ADL2 APIs.
    
    //Declaring ADL Methods.

    //Main
    typedef int(*ADL2_MAIN_CONTROL_CREATE)				  (ADL_MAIN_MALLOC_CALLBACK, int, ADL_CONTEXT_HANDLE*);
    typedef int(*ADL2_MAIN_CONTROLX2_CREATE)			  (ADL_MAIN_MALLOC_CALLBACK, int, ADL_CONTEXT_HANDLE*, ADLThreadingModel);
    typedef int(*ADL2_MAIN_CONTROL_DESTROY)				  (ADL_CONTEXT_HANDLE);

    //Adapter
    typedef int(*ADL2_ADAPTER_NUMBEROFADAPTERS_GET)		  (ADL_CONTEXT_HANDLE, int*);
    typedef int(*ADL2_ADAPTER_PRIMARY_GET)				  (ADL_CONTEXT_HANDLE, int* lpPrimaryAdapterIndex);
    typedef int(*ADL2_ADAPTER_ACTIVE_GET) 				  (ADL_CONTEXT_HANDLE, int, int*);
    typedef int(*ADL2_ADAPTER_ADAPTERINFO_GET)			  (ADL_CONTEXT_HANDLE, LPAdapterInfo lpInfo, int iInputSize);
    typedef int(*ADL2_ADAPTER_ADAPTERINFOX4_GET)		  (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* numAdapters, AdapterInfoX2** lppAdapterInfo);
    typedef int(*ADL2_ADAPTER_PMLOG_SUPPORT_GET)          (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLPMLogSupportInfo* pPMLogSupportInfo);
    typedef int(*ADL2_ADAPTER_PMLOG_START)                (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLPMLogStartInput* pPMLogStartInput, ADLPMLogStartOutput* pPMLogStartOutput, ADL_D3DKMT_HANDLE pDevice);
    typedef int(*ADL2_ADAPTER_PMLOG_STOP)                 (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADL_D3DKMT_HANDLE pDevice);
    typedef int(*ADL2_ADAPTER_MEMORYINFO3_GET)            (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLMemoryInfo3*);
    typedef int(*ADL2_ADAPTER_REGVALUESTRING_GET)         (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDriverPathOption, const char* szSubKey, const char* szKeyName, int iSize, char* lpKeyValue);
    typedef int(*ADL2_ADAPTER_REGVALUEINT_GET)            (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDriverPathOption, const char* szSubKey, const char* szKeyName, int* lpKeyValue);
    typedef int(*ADL2_ADAPTER_VIDEOBIOSINFO_GET)          (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLBiosInfo* lpBiosInfo);
    typedef int(*ADL2_ADAPTER_ASPECTS_GET)                (ADL_CONTEXT_HANDLE context, int iAdapterIndex, char* lpAspects, int iSize);

    //Display
    typedef int(*ADL2_DISPLAY_MODES_GET)                  (ADL_CONTEXT_HANDLE, int iAdapterIndex, int iDisplayIndex, int* lpNumModes, ADLMode** lppModes);
    typedef int(*ADL2_DISPLAY_PRESERVEDASPECTRATIO_GET)   (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, int* lpSupport, int* lpCurrent, int* lpDefault);
    typedef int(*ADL2_DISPLAY_IMAGEEXPANSION_GET)         (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, int* lpSupport, int* lpCurrent, int* lpDefault);
    typedef int(*ADL2_DISPLAY_DISPLAYINFO_GET)			  (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* lpNumDisplays, ADLDisplayInfo** lppInfo, int iForceDetect);

    //Display, Color Depth
    typedef int(*ADL2_DISPLAY_COLORDEPTH_GET)             (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, int* lpColorDepth);
    typedef int(*ADL2_DISPLAY_SUPPORTEDCOLORDEPTH_GET)    (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, int* lpColorDepth);
      
    //Display, Pixel Format
    typedef int(*ADL2_DISPLAY_PIXELFORMAT_GET)            (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, int* lpPixelFormat);
    typedef int(*ADL2_DISPLAY_PIXELFORMATDEFAULT_GET)     (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, int* lpDefPixelFormat);
    typedef int(*ADL2_DISPLAY_SUPPORTEDPIXELFORMAT_GET)   (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, int* lpPixelFormat);
    
    //Display, FreeSync ADL2 calls
    typedef int(*ADL2_DISPLAY_FREESYNCSTATE_GET)          (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, int* lpCurrent, int* lpDefault, int* lpMinRefreshRateInMicroHz, int* lpMaxRefreshRateInMicroHz);
    
    //Display, Virtual Resolution ADL2 calls
    typedef int(*ADL2_DISPLAY_PROPERTY_GET)               (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, ADLDisplayProperty* lpDisplayProperty);
    
    //Display, Color Temperature
    typedef int(*ADL2_DISPLAY_COLORTEMPERATURESOURCE_GET) (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, int* lpSource);
    typedef int(*ADL2_DISPLAY_COLOR_GET)                  (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, int iColorType, int* iCurrent, int* iDefault, int* iMin, int* iMax, int* iStep);

    //Display, Spec
    typedef int(*ADL2_DISPLAY_DCE_GET)                    (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, ADLDceSettings* lpDceSettings);

    //DFP
    typedef int(*ADL2_DFP_GPUSCALINGENABLE_GET)			  (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDisplayIndex, int* lpSupport, int* lpCurrent, int* lpDefault);
    
    //Chill
    typedef int(*ADL2_CHILL_SETTINGS_GET)                 (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* lpEnabled);
    
    //Turbo Sync
    typedef int(*ADL2_TURBOSYNCSUPPORT_GET)               (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* iTurboSyncSupported);

    //Graphics
    typedef int(*ADL2_GRAPHICS_VERSIONSX2_GET)            (ADL_CONTEXT_HANDLE context, ADLVersionsInfoX2* lpVersionsInfo);
    typedef int(*ADL2_GRAPHICS_VERSIONX2_GET)             (ADL_CONTEXT_HANDLE context, ADLVersionsInfoX2* lpVersionsInfo);
    typedef int(*ADL2_GRAPHICS_VERSION_GET)               (ADL_CONTEXT_HANDLE context, ADLVersionsInfo* lpVersionsInfo);  

    //Desktop
    typedef int(*ADL2_DESKTOP_DEVICE_CREATE)              (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADL_D3DKMT_HANDLE* pDevice);
    typedef int(*ADL2_DESKTOP_DEVICE_DESTROY)             (ADL_CONTEXT_HANDLE context, ADL_D3DKMT_HANDLE hDevice);

    //Others
    typedef int(*ADL2_POWERXPRESS_ANCILLARYDEVICES_GET)   (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* lpNumberOfAncillaryDevices, ADLBdf** lppAncillaryDevices);
    typedef int(*ADL2_APPLICATIONPROFILES_HITLISTSX3_GET) (ADL_CONTEXT_HANDLE context, int iListType, int* lpNumApps, ADLApplicationDataX3** lppAppList);
    

    //Overdrive
    typedef int(*ADL_OVERDRIVE_CAPS)                      (int iAdapterIndex, int* iSupported, int* iEnabled, int* iVersion);
    typedef int(*ADL2_OVERDRIVE_CAPS)                     (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* iSupported, int* iEnabled, int* iVersion);

    //OVERDRIVE5
    typedef int (*ADL_OVERDRIVE5_THERMALDEVICES_ENUM)     (int iAdapterIndex, int iThermalControllerIndex, ADLThermalControllerInfo* lpThermalControllerInfo);
    typedef int (*ADL_OVERDRIVE5_ODPARAMETERS_GET)        (int iAdapterIndex, ADLODParameters* lpOdParameters);
    typedef int (*ADL_OVERDRIVE5_TEMPERATURE_GET)         (int iAdapterIndex, int iThermalControllerIndex, ADLTemperature* lpTemperature);
    typedef int (*ADL_OVERDRIVE5_FANSPEED_GET)            (int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue* lpFanSpeedValue);
    typedef int (*ADL_OVERDRIVE5_FANSPEEDINFO_GET)        (int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedInfo* lpFanSpeedInfo);
    typedef int (*ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET) (int iAdapterIndex, int iDefault, ADLODPerformanceLevels* lpOdPerformanceLevels);
    typedef int (*ADL_OVERDRIVE5_ODPARAMETERS_GET)        (int iAdapterIndex, ADLODParameters* lpOdParameters);
    typedef int (*ADL_OVERDRIVE5_CURRENTACTIVITY_GET)     (int iAdapterIndex, ADLPMActivity* lpActivity);
    typedef int (*ADL_OVERDRIVE5_POWERCONTROL_CAPS)       (int iAdapterIndex, int* lpSupported);
    typedef int (*ADL_OVERDRIVE5_POWERCONTROLINFO_GET)    (int iAdapterIndex, ADLPowerControlInfo* lpPowerControlInfo);
    typedef int (*ADL_OVERDRIVE5_POWERCONTROL_GET)        (int iAdapterIndex, int* lpCurrentValue, int* lpDefaultValue);

    typedef int (*ADL2_OVERDRIVE5_THERMALDEVICES_ENUM)    (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iThermalControllerIndex, ADLThermalControllerInfo* lpThermalControllerInfo);   
    typedef int (*ADL2_OVERDRIVE5_ODPARAMETERS_GET)       (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODParameters* lpOdParameters);
    typedef int (*ADL2_OVERDRIVE5_TEMPERATURE_GET)        (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iThermalControllerIndex, ADLTemperature* lpTemperature);
    typedef int (*ADL2_OVERDRIVE5_FANSPEED_GET)           (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue* lpFanSpeedValue);
    typedef int (*ADL2_OVERDRIVE5_FANSPEEDINFO_GET)       (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedInfo* lpFanSpeedInfo);
    typedef int (*ADL2_OVERDRIVE5_ODPERFORMANCELEVELS_GET)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iDefault, ADLODPerformanceLevels* lpOdPerformanceLevels);
    typedef int (*ADL2_OVERDRIVE5_ODPARAMETERS_GET)       (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODParameters* lpOdParameters);
    typedef int (*ADL2_OVERDRIVE5_CURRENTACTIVITY_GET)    (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLPMActivity* lpActivity);
    typedef int (*ADL2_OVERDRIVE5_POWERCONTROL_CAPS)      (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* lpSupported);
    typedef int (*ADL2_OVERDRIVE5_POWERCONTROLINFO_GET)   (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLPowerControlInfo* lpPowerControlInfo);
    typedef int (*ADL2_OVERDRIVE5_POWERCONTROL_GET)       (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* lpCurrentValue, int* lpDefaultValue);

    //OVERDRIVE6
    typedef int (*ADL_OVERDRIVE6_FANSPEED_GET)            (int iAdapterIndex, ADLOD6FanSpeedInfo* lpFanSpeedInfo);
    typedef int (*ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS)  (int iAdapterIndex, ADLOD6ThermalControllerCaps* lpThermalControllerCaps);
    typedef int (*ADL_OVERDRIVE6_TEMPERATURE_GET)         (int iAdapterIndex, int* lpTemperature);
    typedef int (*ADL_OVERDRIVE6_CAPABILITIES_GET)        (int iAdapterIndex, ADLOD6Capabilities* lpODCapabilities);
    typedef int (*ADL_OVERDRIVE6_STATEINFO_GET)           (int iAdapterIndex, int iStateType, ADLOD6StateInfo* lpStateInfo);
    typedef int	(*ADL_OVERDRIVE6_CURRENTSTATUS_GET)       (int iAdapterIndex, ADLOD6CurrentStatus* lpCurrentStatus);
    typedef int (*ADL_OVERDRIVE6_POWERCONTROL_CAPS)       (int iAdapterIndex, int* lpSupported);
    typedef int (*ADL_OVERDRIVE6_POWERCONTROLINFO_GET)    (int iAdapterIndex, ADLOD6PowerControlInfo* lpPowerControlInfo);
    typedef int (*ADL_OVERDRIVE6_POWERCONTROL_GET)        (int iAdapterIndex, int* lpCurrentValue, int* lpDefaultValue);

    //OVERDRIVE8
    typedef int(*ADL2_OVERDRIVE8_INIT_SETTING_GET)        (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLOD8InitSetting* setting);
    typedef int(*ADL2_OVERDRIVE8_CURRENT_SETTING_GET)     (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLOD8CurrentSetting* setting);
    typedef int(*ADL2_NEW_QUERYPMLOGDATA_GET)             (ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLPMLogDataOutput* dataOutput);

    typedef int(*ADL2_OVERDRIVE8_INIT_SETTINGX2_GET)      (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* lpOverdrive8Capabilities, int* lpNumberOfFeatures, ADLOD8SingleInitSetting** lppInitSettingList);
    typedef int(*ADL2_OVERDRIVE8_CURRENT_SETTINGX2_GET)   (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* lpNumberOfFeatures, int** lppCurrentSettingList);
    typedef int(*ADL2_WS_OVERDRIVE_CAPS)                  (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* iSupported, int* iEnabled, int* iVersion);

    //Defining pointers to ADL Methods.
    ADL2_MAIN_CONTROL_CREATE			        ADL2_Main_Control_Create = nullptr;
    ADL2_MAIN_CONTROL_DESTROY			        ADL2_Main_Control_Destroy = nullptr;

    ADL2_ADAPTER_NUMBEROFADAPTERS_GET	        ADL2_Adapter_NumberOfAdapters_Get = nullptr;
    ADL2_ADAPTER_PRIMARY_GET                    ADL2_Adapter_Primary_Get = nullptr;
    ADL2_ADAPTER_ADAPTERINFO_GET                ADL2_Adapter_AdapterInfo_Get = nullptr;
    ADL2_ADAPTER_ACTIVE_GET				        ADL2_Adapter_Active_Get = nullptr;
    ADL2_ADAPTER_MEMORYINFO3_GET                ADL2_Adapter_MemoryInfo3_Get = nullptr;
    ADL2_ADAPTER_PMLOG_SUPPORT_GET              ADL2_Adapter_PMLog_Support_Get = nullptr;
    ADL2_ADAPTER_PMLOG_START                    ADL2_Adapter_PMLog_Start = nullptr;
    ADL2_ADAPTER_PMLOG_STOP                     ADL2_Adapter_PMLog_Stop = nullptr;
    ADL2_ADAPTER_VIDEOBIOSINFO_GET              ADL2_Adapter_VideoBiosInfo_Get = nullptr;
    ADL2_ADAPTER_ASPECTS_GET                    ADL2_Adapter_Aspects_Get = nullptr;

    ADL2_GRAPHICS_VERSIONSX2_GET                ADL2_Graphics_VersionsX2_Get = nullptr;
    ADL2_GRAPHICS_VERSION_GET                   ADL2_Graphics_Versions_Get = nullptr;

    ADL2_DISPLAY_MODES_GET				        ADL2_Display_Modes_Get = nullptr;
    ADL2_DISPLAY_DISPLAYINFO_GET                ADL2_Display_DisplayInfo_Get = nullptr;
    ADL2_DISPLAY_PROPERTY_GET                   ADL2_Display_Property_Get = nullptr;
    ADL2_DISPLAY_DCE_GET                        ADL2_Display_DCE_Get = nullptr;

    ADL2_APPLICATIONPROFILES_HITLISTSX3_GET     ADL2_ApplicationProfiles_HitListsX3_Get = nullptr;

    ADL2_POWERXPRESS_ANCILLARYDEVICES_GET       ADL2_PowerXpress_AncillaryDevices_Get = nullptr;

    //ADL_OVERDRIVE
    ADL_OVERDRIVE_CAPS						    ADL_Overdrive_Caps = nullptr;

    //ADL_OVERDRIVE5
    ADL_OVERDRIVE5_THERMALDEVICES_ENUM		    ADL_Overdrive5_ThermalDevices_Enum = nullptr;
    ADL_OVERDRIVE5_ODPARAMETERS_GET			    ADL_Overdrive5_ODParameters_Get = nullptr;
    ADL_OVERDRIVE5_TEMPERATURE_GET			    ADL_Overdrive5_Temperature_Get = nullptr;
    ADL_OVERDRIVE5_FANSPEED_GET				    ADL_Overdrive5_FanSpeed_Get = nullptr;
    ADL_OVERDRIVE5_FANSPEEDINFO_GET			    ADL_Overdrive5_FanSpeedInfo_Get = nullptr;
    ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET	    ADL_Overdrive5_ODPerformanceLevels_Get = nullptr;
    ADL_OVERDRIVE5_CURRENTACTIVITY_GET		    ADL_Overdrive5_CurrentActivity_Get = nullptr;
    ADL_OVERDRIVE5_POWERCONTROL_CAPS		    ADL_Overdrive5_PowerControl_Caps = nullptr;
    ADL_OVERDRIVE5_POWERCONTROLINFO_GET		    ADL_Overdrive5_PowerControlInfo_Get = nullptr;
    ADL_OVERDRIVE5_POWERCONTROL_GET			    ADL_Overdrive5_PowerControl_Get = nullptr;

    //ADL_OVERDRIVE6
    ADL_OVERDRIVE6_FANSPEED_GET                 ADL_Overdrive6_FanSpeed_Get = nullptr;
    ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS       ADL_Overdrive6_ThermalController_Caps = nullptr;
    ADL_OVERDRIVE6_TEMPERATURE_GET              ADL_Overdrive6_Temperature_Get = nullptr;
    ADL_OVERDRIVE6_CAPABILITIES_GET             ADL_Overdrive6_Capabilities_Get = nullptr;
    ADL_OVERDRIVE6_STATEINFO_GET                ADL_Overdrive6_StateInfo_Get = nullptr;
    ADL_OVERDRIVE6_CURRENTSTATUS_GET            ADL_Overdrive6_CurrentStatus_Get = nullptr;
    ADL_OVERDRIVE6_POWERCONTROL_CAPS            ADL_Overdrive6_PowerControl_Caps = nullptr;
    ADL_OVERDRIVE6_POWERCONTROLINFO_GET         ADL_Overdrive6_PowerControlInfo_Get = nullptr;
    ADL_OVERDRIVE6_POWERCONTROL_GET             ADL_Overdrive6_PowerControl_Get = nullptr;

    //ADL2_OVERDRIVE
    ADL2_OVERDRIVE_CAPS                         ADL2_Overdrive_Caps = nullptr;

    //ADL2_OVERDRIVE5
    ADL2_OVERDRIVE5_THERMALDEVICES_ENUM		    ADL2_Overdrive5_ThermalDevices_Enum = nullptr;
    ADL2_OVERDRIVE5_ODPARAMETERS_GET			ADL2_Overdrive5_ODParameters_Get = nullptr;
    ADL2_OVERDRIVE5_TEMPERATURE_GET			    ADL2_Overdrive5_Temperature_Get = nullptr;
    ADL2_OVERDRIVE5_FANSPEED_GET				ADL2_Overdrive5_FanSpeed_Get = nullptr;
    ADL2_OVERDRIVE5_FANSPEEDINFO_GET			ADL2_Overdrive5_FanSpeedInfo_Get = nullptr;
    ADL2_OVERDRIVE5_ODPERFORMANCELEVELS_GET	    ADL2_Overdrive5_ODPerformanceLevels_Get = nullptr;
    ADL2_OVERDRIVE5_CURRENTACTIVITY_GET		    ADL2_Overdrive5_CurrentActivity_Get = nullptr;
    ADL2_OVERDRIVE5_POWERCONTROL_CAPS		    ADL2_Overdrive5_PowerControl_Caps = nullptr;
    ADL2_OVERDRIVE5_POWERCONTROLINFO_GET		ADL2_Overdrive5_PowerControlInfo_Get = nullptr;
    ADL2_OVERDRIVE5_POWERCONTROL_GET			ADL2_Overdrive5_PowerControl_Get = nullptr;

    //ADL2_OVERDRIVE8
    ADL2_OVERDRIVE8_INIT_SETTINGX2_GET          ADL2_Overdrive8_Init_SettingX2_Get = nullptr;
    ADL2_OVERDRIVE8_CURRENT_SETTINGX2_GET       ADL2_Overdrive8_Current_SettingX2_Get = nullptr;

    ADL2_WS_OVERDRIVE_CAPS                      ADL2_WS_Overdrive_Caps = nullptr;

    ADL2_DESKTOP_DEVICE_CREATE                  ADL2_Desktop_Device_Create = nullptr;
    ADL2_DESKTOP_DEVICE_DESTROY                 ADL2_Desktop_Device_Destroy = nullptr;

    const std::map<uint32_t, std::string> VENDOR_IDS = {
        {0x1002, "AMD"},
        {0x10DE, "NVIDIA"},
        {0x1043, "ASUS"},
        {0x196D, "Club 3D"},
        {0x1092, "Diamond Multimedia"},
        {0x18BC, "GeCube"},
        {0x1458, "Gigabyte"},
        {0x17AF, "HIS"},
        {0x16F3, "Jetway"},
        {0x1462, "MSI"},
        {0x1DA2, "Sapphire"},
        {0x148C, "PowerColor"},
        {0x1545, "VisionTek"},
        {0x1682, "XFX"},
        {0x1025, "Acer"},
        {0x106B, "Apple"},
        {0x1028, "Dell"},
        {0x107B, "Gateway"},
        {0x103C, "HP"},
        {0x17AA, "Lenovo"},
        {0x104D, "Sony"},
        {0x1179, "Toshiba"}
    };

    AdapterInfo m_adapterInfo;
    uint32_t m_adapterIndex{ 0 };

    ADLODParameters* m_overdriveParameters{ nullptr };

    int m_maxThermalControllerIndex{ 10 };
    int m_thermalDomainControllerIndex{ 0 };

    ADLFanSpeedInfo* m_fanSpeedInfo{ nullptr };
    int m_fanSpeedReportingMethod{ 0 };

    ADLOD6ThermalControllerCaps* m_thermalControllerCaps{ nullptr };

    std::string m_model{};

    std::string m_memoryVendor{ "" };
    std::string m_memorySize{ "" };
    std::string m_memoryType{ "" };
    uint32_t m_memoryBandwidth{ 0 };

    std::string m_driverInfo{ "" };
    std::string m_driverVersion{ "" };

    bool m_overdriveSupported{ false };
    bool m_overdriveEnabled{ false };
    int m_adlOverdriveVersion{ 1 };
    int m_overdriveVersion{ 0 };
};
