#include "NvapiHandler.h"

#include <windows.h>

#include "external/nvapi/nvapi.h"

#include <QDebug>

//#ifndef NV_H
//#define NV_H
//
//def MAKE_NVAPI_VERSION(struct, version):
//	return sizeof(struct) | (version << 16)

#if defined(_M_X64) || defined(__amd64__)
#define NVAPI_DLL "nvapi64.dll"
#else
#define NVAPI_DLL "nvapi.dll"
#endif

// magic numbers, do not change them
//#define NVAPI_MAX_PHYSICAL_GPUS   64
#define NVAPI_MAX_USAGES_PER_GPU  34
#define NVAPI_MAX_COOLERS_PER_GPU 20 //3

typedef enum _NV_RAM_MAKER
{
    NV_RAM_MAKER_NONE,
    NV_RAM_MAKER_SAMSUNG,
    NV_RAM_MAKER_QIMONDA,
    NV_RAM_MAKER_ELPIDA,
    NV_RAM_MAKER_ETRON,
    NV_RAM_MAKER_NANYA,
    NV_RAM_MAKER_HYNIX,
    NV_RAM_MAKER_MOSEL,
    NV_RAM_MAKER_WINBOND,
    NV_RAM_MAKER_ELITE,
    NV_RAM_MAKER_MICRON,
    NV_RAM_MAKER_UNKNOWN_0,
    NV_RAM_MAKER_UNKNOWN_1,
    NV_RAM_MAKER_UNKNOWN_2,
    NV_RAM_MAKER_UNKNOWN_3,
    NV_RAM_MAKER_UNKNOWN_4,
    NV_RAM_MAKER_UNKNOWN_5,
    NV_RAM_MAKER_UNKNOWN_6,
    NV_RAM_MAKER_UNKNOWN_7,
    NV_RAM_MAKER_UNKNOWN_8
} NV_RAM_MAKER;

std::vector<std::string> _NV_RAM_MAKER_NAME
{
    "NONE",
    "SAMSUNG",
    "QIMONDA",
    "ELPIDA",
    "ETRON",
    "NANYA",
    "HYNIX",
    "MOSEL",
    "WINBOND",
    "ELITE",
    "MICRON",
    "UNKNOWN_0",
    "UNKNOWN_1",
    "UNKNOWN_2",
    "UNKNOWN_3",
    "UNKNOWN_4",
    "UNKNOWN_5",
    "UNKNOWN_6",
    "UNKNOWN_7",
    "UNKNOWN_8"
};

typedef enum _NV_RAM_TYPE
{
    NV_RAM_TYPE_NONE,
    NV_RAM_TYPE_SDRAM,
    NV_RAM_TYPE_DDR1,
    NV_RAM_TYPE_DDR2,
    NV_RAM_TYPE_GDDR2,
    NV_RAM_TYPE_GDDR3,
    NV_RAM_TYPE_GDDR4,
    NV_RAM_TYPE_DDR3,
    NV_RAM_TYPE_GDDR5,
    NV_RAM_TYPE_LPDDR2,
    NV_RAM_TYPE_GDDR5X,
    NV_RAM_TYPE_GDDR6,
    NV_RAM_TYPE_GDDR6X,
    NV_RAM_TYPE_UNKNOWN_0,
    NV_RAM_TYPE_UNKNOWN_1,
    NV_RAM_TYPE_UNKNOWN_2
} NV_RAM_TYPE;

std::vector<std::string> _NV_RAM_TYPE_NAME
{
    "NONE",
    "SDRAM",
    "DDR1",
    "DDR2",
    "GDDR2",
    "GDDR3",
    "GDDR4",
    "DDR3",
    "GDDR5",
    "LPDDR2",
    "GDDR5X",
    "GDDR6",
    "GDDR6X",
    "UNKNOWN_0",
    "UNKNOWN_1",
    "UNKNOWN_2"
};

//
//#define NV_GPU_GETCOOLER_SETTINGS_VER       NV_GPU_GETCOOLER_SETTINGS_VER3

// rev
typedef enum _NV_COOLER_TYPE
{
    NVAPI_COOLER_TYPE_NONE = 0,
    NVAPI_COOLER_TYPE_FAN,
    NVAPI_COOLER_TYPE_WATER,
    NVAPI_COOLER_TYPE_LIQUID_NO2,
} NV_COOLER_TYPE;

// rev
typedef enum _NV_COOLER_CONTROLLER
{
    NVAPI_COOLER_CONTROLLER_NONE = 0,
    NVAPI_COOLER_CONTROLLER_ADI,
    NVAPI_COOLER_CONTROLLER_INTERNAL,
} NV_COOLER_CONTROLLER;

// rev
typedef enum _NV_COOLER_POLICY
{
    NVAPI_COOLER_POLICY_NONE = 0,
    NVAPI_COOLER_POLICY_MANUAL,                     // Manual adjustment of cooler level. Gets applied right away independent of temperature or performance level.
    NVAPI_COOLER_POLICY_PERF,                       // GPU performance controls the cooler level.
    NVAPI_COOLER_POLICY_TEMPERATURE_DISCRETE = 4,   // Discrete thermal levels control the cooler level.
    NVAPI_COOLER_POLICY_TEMPERATURE_CONTINUOUS = 8, // Cooler level adjusted at continuous thermal levels.
    NVAPI_COOLER_POLICY_HYBRID,                     // Hybrid of performance and temperature levels.
} NV_COOLER_POLICY;

// rev
typedef enum _NV_COOLER_TARGET
{
    NVAPI_COOLER_TARGET_NONE = 0,
    NVAPI_COOLER_TARGET_GPU,
    NVAPI_COOLER_TARGET_MEMORY,
    NVAPI_COOLER_TARGET_POWER_SUPPLY = 4,
    NVAPI_COOLER_TARGET_ALL = 7                    // This cooler cools all of the components related to its target gpu.
} NV_COOLER_TARGET;

// rev
typedef enum _NV_COOLER_CONTROL
{
    NVAPI_COOLER_CONTROL_NONE = 0,
    NVAPI_COOLER_CONTROL_TOGGLE,                   // ON/OFF
    NVAPI_COOLER_CONTROL_VARIABLE,                 // Suppports variable control.
} NV_COOLER_CONTROL;

// rev
typedef enum _NV_COOLER_ACTIVITY_LEVEL
{
    NVAPI_INACTIVE = 0,                             // inactive or unsupported
    NVAPI_ACTIVE = 1,                               // active and spinning in case of fan
} NV_COOLER_ACTIVITY_LEVEL;

// rev
typedef struct _NV_GPU_COOLER_SETTINGS
{
    NvU32 version;                           // structure version
    NvU32 count;                             // number of associated coolers with the selected GPU
    struct
    {
        NV_COOLER_TYPE type;                 // type of cooler - FAN, WATER, LIQUID_NO2...
        NV_COOLER_CONTROLLER controller;     // internal, ADI...
        NvU32 defaultMinLevel;               // the min default value % of the cooler
        NvU32 defaultMaxLevel;               // the max default value % of the cooler
        NvU32 currentMinLevel;               // the current allowed min value % of the cooler
        NvU32 currentMaxLevel;               // the current allowed max value % of the cooler
        NvU32 currentLevel;                  // the current value % of the cooler
        NV_COOLER_POLICY defaultPolicy;      // cooler control policy - auto-perf, auto-thermal, manual, hybrid...
        NV_COOLER_POLICY currentPolicy;      // cooler control policy - auto-perf, auto-thermal, manual, hybrid...
        NV_COOLER_TARGET target;             // cooling target - GPU, memory, chipset, powersupply, canoas...
        NV_COOLER_CONTROL controlType;       // toggle or variable
        NV_COOLER_ACTIVITY_LEVEL active;     // is the cooler active - fan spinning...
    } cooler[NVAPI_MAX_COOLERS_PER_GPU];
} NV_GPU_COOLER_SETTINGS, * PNV_GPU_COOLER_SETTINGS;

#define NV_GPU_COOLER_SETTINGS_VER  MAKE_NVAPI_VERSION(NV_GPU_COOLER_SETTINGS, 1)

// function pointer types
typedef NvAPI_Status* (*NvAPI_QueryInterface_t)(unsigned int offset);
typedef NvAPI_Status(*NvAPI_Initialize_t)();
typedef NvAPI_Status(*NvAPI_EnumPhysicalGPUs_t)(int** handles, int* count);
typedef NvAPI_Status(*NvAPI_GPU_GetUsages_t)(int* handle, unsigned int* usages);
typedef NvAPI_Status(*NvAPI_GPU_GetThermalSettings_t)(int* handle, int sensorIndex, NV_GPU_THERMAL_SETTINGS* temp);
typedef NvAPI_Status(*NvAPI_GPU_GetMemoryInfoEx_t)(int* handle, NV_GPU_MEMORY_INFO_EX* memory_info);
typedef NvAPI_Status(*NvAPI_GPU_GetMemoryInfo_t)(int* handle, NV_DISPLAY_DRIVER_MEMORY_INFO* pMemoryInfo);
typedef NvAPI_Status(*NvAPI_GPU_GetTachReading_t)(int* handle, unsigned long* speed);
typedef NvAPI_Status(*NvAPI_GPU_GetCoolerSettings_t)(int* handle, int coolerIndex, NV_GPU_COOLER_SETTINGS* pCoolerInfo);
typedef NvAPI_Status(*NvAPI_GPU_GetFullName_t)(int* handle, char* name);
typedef NvAPI_Status(*NvAPI_GPU_GetAllClockFrequencies_t)(int* handle, NV_GPU_CLOCK_FREQUENCIES* clock_freq);
typedef NvAPI_Status(*NvAPI_GPU_GetDynamicPstatesInfoEx_t)(int* handle, NV_GPU_DYNAMIC_PSTATES_INFO_EX* pstates_info);
typedef NvAPI_Status(*NvAPI_GPU_GetRamMaker_t)(int* handle, NV_RAM_MAKER* maker);
typedef NvAPI_Status(*NvAPI_GPU_GetRamType_t)(int* handle, NV_RAM_TYPE* type);
typedef NvAPI_Status(*NvAPI_GetErrorMessage_t)(NvAPI_Status, NvAPI_ShortString);
typedef NvAPI_Status(*NvAPI_SYS_GetDisplayDriverInfo_t)(NV_DISPLAY_DRIVER_INFO* pDriverInfo);

// nvapi.dll internal function pointers
NvAPI_QueryInterface_t              _NvAPI_QueryInterface = nullptr;
NvAPI_Initialize_t                  _NvAPI_Initialize = nullptr;
NvAPI_EnumPhysicalGPUs_t            _NvAPI_EnumPhysicalGPUs = nullptr;
NvAPI_GPU_GetUsages_t               _NvAPI_GPU_GetUsages = nullptr;
NvAPI_GPU_GetThermalSettings_t	    _NvAPI_GPU_GetThermalSettings = nullptr;
NvAPI_GPU_GetMemoryInfoEx_t         _NvAPI_GPU_GetMemoryInfoEx = nullptr;
NvAPI_GPU_GetMemoryInfo_t           _NvAPI_GPU_GetMemoryInfo = nullptr;
NvAPI_GPU_GetTachReading_t          _NvAPI_GPU_GetTachReading = nullptr;
NvAPI_GPU_GetCoolerSettings_t       _NvAPI_GPU_GetCoolerSettings = nullptr;
NvAPI_GPU_GetFullName_t             _NvAPI_GPU_GetFullName = nullptr;
NvAPI_GPU_GetAllClockFrequencies_t  _NvAPI_GPU_GetAllClockFrequencies = nullptr;
NvAPI_GPU_GetDynamicPstatesInfoEx_t _NvAPI_GPU_GetDynamicPstatesInfoEx = nullptr;
NvAPI_GPU_GetRamMaker_t             _NvAPI_GPU_GetRamMaker = nullptr;
NvAPI_GPU_GetRamType_t              _NvAPI_GPU_GetRamType = nullptr;
NvAPI_GetErrorMessage_t             _NvAPI_GetErrorMessage = nullptr;
NvAPI_SYS_GetDisplayDriverInfo_t    _NvAPI_SYS_GetDisplayDriverInfo = nullptr;

NvapiHandler::NvapiHandler()
{
    qDebug() << __FUNCTION__;
}

NvapiHandler::~NvapiHandler()
{
    qDebug() << __FUNCTION__;
}

/*
 * Name: NvGpuDetected
 * Desc: Returns true if an NVIDIA Gpu has been detected on this system.
 * NOTE: This function depends on whether a valid NVIDIA driver is installed
 *       on the target machine.  Since the Surface Hub does not include an
 *		 nvapi[64].dll in it's strippified driver, we need to load it directly
 *		 in order for the required APIs to work.
 */
bool NvapiHandler::init()
{
    HMODULE hmod = LoadLibraryA(NVAPI_DLL);

    if (hmod == NULL)
    {
        qWarning() << "Couldn't find " << NVAPI_DLL;
        return false;
    }

    // nvapi_QueryInterface is a function used to retrieve other internal functions in nvapi.dll
    _NvAPI_QueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(hmod, "nvapi_QueryInterface");

    // some useful internal functions that aren't exported by nvapi.dll
    _NvAPI_Initialize = (NvAPI_Initialize_t)(*_NvAPI_QueryInterface)(0x0150E828);
    _NvAPI_GetErrorMessage = (NvAPI_GetErrorMessage_t)(*_NvAPI_QueryInterface)(0x6C2D048C);
    _NvAPI_EnumPhysicalGPUs = (NvAPI_EnumPhysicalGPUs_t)(*_NvAPI_QueryInterface)(0xE5AC921F);
    _NvAPI_GPU_GetUsages = (NvAPI_GPU_GetUsages_t)(*_NvAPI_QueryInterface)(0x189A1FDF);
    _NvAPI_GPU_GetThermalSettings = (NvAPI_GPU_GetThermalSettings_t)(*_NvAPI_QueryInterface)(0xE3640A56);
    _NvAPI_GPU_GetMemoryInfoEx = (NvAPI_GPU_GetMemoryInfoEx_t)(*_NvAPI_QueryInterface)(0xc0599498);
    _NvAPI_GPU_GetMemoryInfo = (NvAPI_GPU_GetMemoryInfo_t)(*_NvAPI_QueryInterface)(0x774AA982);
    //_NvAPI_GPU_GetRamType            = (NvAPI_GPU_GetRamType_t) (*_NvAPI_QueryInterface)(0x57F7CAACUL);
    _NvAPI_GPU_GetTachReading = (NvAPI_GPU_GetTachReading_t)(*_NvAPI_QueryInterface)(0x5F608315);
    _NvAPI_GPU_GetAllClockFrequencies = (NvAPI_GPU_GetAllClockFrequencies_t)(*_NvAPI_QueryInterface)(0xDCB616C3);
    _NvAPI_GPU_GetDynamicPstatesInfoEx = (NvAPI_GPU_GetDynamicPstatesInfoEx_t)(*_NvAPI_QueryInterface)(0x60DED2ED);
    _NvAPI_GPU_GetCoolerSettings = (NvAPI_GPU_GetCoolerSettings_t)(*_NvAPI_QueryInterface)(0xDA141340);
    _NvAPI_GPU_GetFullName = (NvAPI_GPU_GetFullName_t)(*_NvAPI_QueryInterface)(0xCEEE8e9F);

    _NvAPI_GPU_GetRamMaker = (NvAPI_GPU_GetRamMaker_t)(*_NvAPI_QueryInterface)(0x42AEA16A);
    _NvAPI_GPU_GetRamType = (NvAPI_GPU_GetRamType_t)(*_NvAPI_QueryInterface)(0x57F7CAAC);
    _NvAPI_SYS_GetDisplayDriverInfo = (NvAPI_SYS_GetDisplayDriverInfo_t)(*_NvAPI_QueryInterface)(0x721faceb);

    if (_NvAPI_Initialize)
    {
        NvAPI_Status status = (*_NvAPI_Initialize)();
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_Initialize"] = string;
            qWarning() << __FUNCTION__ << " : " << "_NvAPI_Initialize failed! status: " << string;
        }
        else
        {
            m_initialized = true;
        }
    }
    else
    {
        qWarning() << __FUNCTION__ << " : " << "_NvAPI_Initialize is null!";
    }

    m_functionsSupportStatus["NvAPI_Initialize"] = m_initialized;

    return m_initialized;
}

/*
 * Name: NvGetGpuInfoNVidia
 * Desc: Returns the NVIDIA Gpu's current load percentage.
 */
void NvapiHandler::readStaticInfo()
{
    NvAPI_Status status;
    NvAPI_ShortString string;

    m_functionsSupportStatus["NvAPI_GetErrorMessage"] = false;
    m_functionsSupportStatus["NvAPI_EnumPhysicalGPUs"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetFullName"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetMemoryInfo"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetMemoryInfoEx"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetRamType"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetRamMaker"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetCoolerSettings"] = false;

    if (!m_initialized)
    {
        return;
    }

    if (_NvAPI_GetErrorMessage)
    {
        status = (*_NvAPI_GetErrorMessage)(NVAPI_OK, string);
        if (status == NVAPI_OK) {
            m_functionsSupportStatus["NvAPI_GetErrorMessage"] = true;
        }
    }

    if (_NvAPI_EnumPhysicalGPUs)
    {
        int gpuCount = 0;
        int* gpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { NULL };

        status = (*_NvAPI_EnumPhysicalGPUs)(gpuHandles, &gpuCount);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_EnumPhysicalGPUs"] = string;

            qWarning() << "_NvAPI_EnumPhysicalGPUs failed! status: " << string;
            return;
        }
        else
        {
            m_functionsSupportStatus["NvAPI_EnumPhysicalGPUs"] = true;
            m_gpuHandle = gpuHandles[0];
        }
    }

    if (_NvAPI_GPU_GetFullName)
    {
        char gpuName[256] = { 0 };
        status = (*_NvAPI_GPU_GetFullName)(m_gpuHandle, gpuName);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_GPU_GetFullName"] = string;

            qWarning() << "_NvAPI_GPU_GetFullName failed! status: " << string;
        }
        else
        {
            m_functionsSupportStatus["NvAPI_GPU_GetFullName"] = true;
            m_gpuModel = gpuName;
        }
    }

    if (_NvAPI_GPU_GetMemoryInfo)
    {
        NV_DISPLAY_DRIVER_MEMORY_INFO memory_info;
        memory_info.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER;
        status = (*_NvAPI_GPU_GetMemoryInfo)(m_gpuHandle, &memory_info);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_GPU_GetMemoryInfo"] = string;
            qWarning() << "_NvAPI_GPU_GetMemoryInfo failed! status: " << string;
        }
        else
        {
            m_functionsSupportStatus["NvAPI_GPU_GetMemoryInfo"] = true;
            m_gpuMemorySize = memory_info.dedicatedVideoMemory / 1024 / 1024;
        }  
    }
    
    if (_NvAPI_GPU_GetMemoryInfoEx)
    {
        NV_GPU_MEMORY_INFO_EX memory_info;
        memory_info.version = NV_GPU_MEMORY_INFO_EX_VER;
        status = (*_NvAPI_GPU_GetMemoryInfoEx)(m_gpuHandle, &memory_info);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_GPU_GetMemoryInfoEx"] = string;

            qWarning() << "_NvAPI_GPU_GetMemoryInfoEx failed! status: " << string;
        }
        else
        {
            m_functionsSupportStatus["NvAPI_GPU_GetMemoryInfoEx"] = true;
            m_gpuMemorySize = memory_info.dedicatedVideoMemory / 1024 / 1024;
        }
    }

    if (_NvAPI_GPU_GetRamType)
    {
        NV_RAM_TYPE ramType;
        status = (*_NvAPI_GPU_GetRamType)(m_gpuHandle, &ramType);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_GPU_GetRamType"] = string;

            qWarning() << "_NvAPI_GPU_GetRamType failed! status: " << string;
        }
        else
        {
            m_functionsSupportStatus["NvAPI_GPU_GetRamType"] = true;
            m_gpuMemoryType = _NV_RAM_TYPE_NAME[ramType];
        }  
    }

    if (_NvAPI_GPU_GetRamMaker)
    {
        NV_RAM_MAKER ramMaker;
        status = (*_NvAPI_GPU_GetRamMaker)(m_gpuHandle, &ramMaker);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_GPU_GetRamMaker"] = string;

            qWarning() << "_NvAPI_GPU_GetRamMaker failed! status: " << string;
        }
        else
        {
            m_functionsSupportStatus["NvAPI_GPU_GetRamMaker"] = true;
            m_gpuMemoryVendor = _NV_RAM_MAKER_NAME[ramMaker];
        }    
    }

    if (_NvAPI_SYS_GetDisplayDriverInfo)
    {
        NV_DISPLAY_DRIVER_INFO driverInfo = { NV_DISPLAY_DRIVER_INFO_VER };
        status = (*_NvAPI_SYS_GetDisplayDriverInfo)(&driverInfo);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_SYS_GetDisplayDriverInfo"] = string;

            qWarning() << "_NvAPI_SYS_GetDisplayDriverInfo failed! status: " << string;
        }
        else
        {
            m_functionsSupportStatus["NvAPI_SYS_GetDisplayDriverInfo"] = true;

            if (driverInfo.bIsNVIDIAGameReadyPackage)
            {
                m_gpuDriverInfo = "NVIDIA GameReady";
            }
            else if (driverInfo.bIsNVIDIAStudioPackage)
            {
                m_gpuDriverInfo = "NVIDIA Studio";
            }
            else if (driverInfo.bIsDCHDriver)
            {
                m_gpuDriverInfo = "Standard Display Driver";
            }

            m_gpuDriverVersion = std::to_string(driverInfo.driverVersion);
            m_gpuDriverVersion = m_gpuDriverVersion.substr(0, 3) + "." + m_gpuDriverVersion.substr(3);
        }
    }

    checkSupportedDynamicFunctions();

    //NvU32 DriverVersion;
    //NvAPI_ShortString BuildBranchString;
    //if (NvAPI_SYS_GetDriverAndBranchVersion(&DriverVersion, BuildBranchString) == NVAPI_OK)
    //{
    //    printf("DriverVersion: %u\n", DriverVersion);
    //    printf("BuildBranchString: %s\n", BuildBranchString);
    //}

    //IDXGIAdapter* adapter = ...
    //    LARGE_INTEGER i;
    //if (SUCCEEDED(adapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &i)))
    //{
    //    printf("User mode driver version: %u.%u.%u.%u\n",
    //        i.QuadPart >> 48,
    //        (i.QuadPart >> 32) & 0xFFFF,
    //        (i.QuadPart >> 16) & 0xFFFF,
    //        i.QuadPart & 0xFFFF);
    //}

    //NvAPI_GPU_GetRamMaker
    //NvAPI_GPU_GetRamType
}

void NvapiHandler::checkSupportedDynamicFunctions()
{
    NvAPI_Status status;
    NvAPI_ShortString string;

    m_functionsSupportStatus["NvAPI_GPU_GetDynamicPstatesInfoEx"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetAllClockFrequencies"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetThermalSettings"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetTachReading"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetTachReading"] = false;

    if (_NvAPI_GPU_GetDynamicPstatesInfoEx)
    {
        NV_GPU_DYNAMIC_PSTATES_INFO_EX pstates_info;
        pstates_info.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;
        status = (*_NvAPI_GPU_GetDynamicPstatesInfoEx)(m_gpuHandle, &pstates_info);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_GPU_GetDynamicPstatesInfoEx"] = string;
        }
        else {
            m_functionsSupportStatus["NvAPI_GPU_GetDynamicPstatesInfoEx"] = true;
        }
    }

    if (_NvAPI_GPU_GetAllClockFrequencies)
    {
        NV_GPU_CLOCK_FREQUENCIES frequencies;
        frequencies.version = NV_GPU_CLOCK_FREQUENCIES_VER;
        frequencies.ClockType = NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ;
        status = (*_NvAPI_GPU_GetAllClockFrequencies)(m_gpuHandle, &frequencies);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_GPU_GetAllClockFrequencies"] = string;
        }
        else {
            m_functionsSupportStatus["NvAPI_GPU_GetAllClockFrequencies"] = true;
        }
    }

    if (_NvAPI_GPU_GetThermalSettings)
    {
        NV_GPU_THERMAL_SETTINGS thermalSettings;
        thermalSettings.version = NV_GPU_THERMAL_SETTINGS_VER;
        thermalSettings.count = 0;
        thermalSettings.sensor[0].controller = NV_THERMAL_CONTROLLER::NVAPI_THERMAL_CONTROLLER_UNKNOWN;
        thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_GPU;

        status = (*_NvAPI_GPU_GetThermalSettings)(m_gpuHandle, 0, &thermalSettings);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_GPU_GetThermalSettings"] = string;
        }
        else
        {
            m_functionsSupportStatus["NvAPI_GPU_GetThermalSettings"] = true;
        }

    }

    if (_NvAPI_GPU_GetTachReading)
    {
        unsigned long rpmSpeed = 0;
        //@note: fails on some gpus because of no support.
        status = (*_NvAPI_GPU_GetTachReading)(m_gpuHandle, &rpmSpeed);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_GPU_GetTachReading"] = string;
        }
        else {
            m_functionsSupportStatus["NvAPI_GPU_GetTachReading"] = true;
        }
    }

    if (_NvAPI_GPU_GetCoolerSettings)
    {
        NvAPI_Status status;

        NV_GPU_COOLER_SETTINGS cooler_settings;
        cooler_settings.version = NV_GPU_COOLER_SETTINGS_VER;
        status = (*_NvAPI_GPU_GetCoolerSettings)(m_gpuHandle, NVAPI_COOLER_TARGET_GPU, &cooler_settings);
        if (status != NVAPI_OK) {
            (*_NvAPI_GetErrorMessage)(status, string);
            m_functionsStatusMessage["NvAPI_GPU_GetCoolerSettings"] = string;
            return;
        }
        else {
            m_functionsSupportStatus["NvAPI_GPU_GetCoolerSettings"] = true;
        }
    }
}

/*
 * Name: NvGetGpuInfoNVidia
 * Desc: Returns the NVIDIA Gpu's current load percentage.
 */
void NvapiHandler::readDynamicInfo()
{
    if (!m_initialized)
    {
        return;
    }

    readGpuUsage();
    readGpuFrequencies();
    readGpuMemory();
    readGpuTemperature();
    readGpuFanSpeed();
    readGpuCooler();
}

void NvapiHandler::readGpuUsage()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetDynamicPstatesInfoEx"])
    {
        NvAPI_Status status;
        unsigned int gpuUsages[NVAPI_MAX_USAGES_PER_GPU] = { 0 };

        //@note: we can use both nvapi functions to get gpu usage
        // gpuUsages[0] must be this value, otherwise NvAPI_GPU_GetUsages won't work
        //gpuUsages[0] = (NVAPI_MAX_USAGES_PER_GPU * 4) | 0x10000;

        //status = (*_NvAPI_GPU_GetUsages)(m_gpuHandle, gpuUsages);
        //if (status != NVAPI_OK) {
        //    qWarning() << "_NvAPI_GPU_GetUsages failed! status: " << status;
        //    return false;
        //}

        //m_gpuUsage = gpuUsages[3];

        NV_GPU_DYNAMIC_PSTATES_INFO_EX pstates_info;
        pstates_info.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;
        status = (*_NvAPI_GPU_GetDynamicPstatesInfoEx)(m_gpuHandle, &pstates_info);
        if (status != NVAPI_OK) {
            return;
        }
        m_gpuUsage = pstates_info.utilization[0].percentage;
        m_gpuVramUsage = pstates_info.utilization[1].percentage;
    }
}

void NvapiHandler::readGpuFrequencies()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetAllClockFrequencies"])
    {
        NvAPI_Status status;

        NV_GPU_CLOCK_FREQUENCIES frequencies;
        frequencies.version = NV_GPU_CLOCK_FREQUENCIES_VER;
        frequencies.ClockType = NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ;
        status = (*_NvAPI_GPU_GetAllClockFrequencies)(m_gpuHandle, &frequencies);
        if (status != NVAPI_OK) {
            return;
        }

        if (frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].bIsPresent)
        {
            m_gpuClockSpeed = frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency / 1000;
        }
        if (frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].bIsPresent)
        {
            m_gpuVramClockSpeed = frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency / 1000;
        }
        //@note: no usage and not present anyway
        //if (frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].bIsPresent)
        //{
            //m_gpuClockSpeed = frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].frequency / 1000;
        //}

        //@note: tested with base and boost clock, gives the same values
        //frequencies.ClockType = NV_GPU_CLOCK_FREQUENCIES_BASE_CLOCK;
        //frequencies.ClockType = NV_GPU_CLOCK_FREQUENCIES_BOOST_CLOCK;
    }
}

void NvapiHandler::readGpuMemory()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetMemoryInfoEx"])
    {
        NvAPI_Status status;

        NV_GPU_MEMORY_INFO_EX memory_info;
        memory_info.version = NV_GPU_MEMORY_INFO_EX_VER;
        status = (*_NvAPI_GPU_GetMemoryInfoEx)(m_gpuHandle, &memory_info);
        if (status != NVAPI_OK) {
            return;
        }

        const uint32_t gpuVramUsed = (memory_info.availableDedicatedVideoMemory - memory_info.curAvailableDedicatedVideoMemory);
        m_gpuVramUsed = (gpuVramUsed) / 1024 / 1024;
        m_gpuVramUsage = (static_cast<double>(gpuVramUsed) / memory_info.availableDedicatedVideoMemory) * 100.0;
    }
}

void NvapiHandler::readGpuTemperature()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetThermalSettings"])
    {
        NvAPI_Status status;
        NV_GPU_THERMAL_SETTINGS thermalSettings;
        thermalSettings.version = NV_GPU_THERMAL_SETTINGS_VER;
        thermalSettings.count = 0;
        thermalSettings.sensor[0].controller = NV_THERMAL_CONTROLLER::NVAPI_THERMAL_CONTROLLER_UNKNOWN;
        thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_GPU;

        status = (*_NvAPI_GPU_GetThermalSettings)(m_gpuHandle, 0, &thermalSettings);
        if (status != NVAPI_OK) {
            return;
        }

        m_gpuTemperature = thermalSettings.sensor[0].currentTemp;
        m_gpuHotspotTemperature = m_gpuTemperature;

        thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_MEMORY;
        status = (*_NvAPI_GPU_GetThermalSettings)(m_gpuHandle, 0, &thermalSettings);
        if (status != NVAPI_OK) {
            return;
        }
        else if (thermalSettings.sensor[0].currentTemp > m_gpuHotspotTemperature)
        {
            m_gpuHotspotTemperature = thermalSettings.sensor[0].currentTemp;
        }


        thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_POWER_SUPPLY;
        status = (*_NvAPI_GPU_GetThermalSettings)(m_gpuHandle, 0, &thermalSettings);
        if (status != NVAPI_OK) {
            return;
        }
        else if (thermalSettings.sensor[0].currentTemp > m_gpuHotspotTemperature)
        {
            m_gpuHotspotTemperature = thermalSettings.sensor[0].currentTemp;
        }

        thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_BOARD;
        status = (*_NvAPI_GPU_GetThermalSettings)(m_gpuHandle, 0, &thermalSettings);
        if (status != NVAPI_OK) {
            return;
        }
        else if (thermalSettings.sensor[0].currentTemp > m_gpuHotspotTemperature)
        {
            m_gpuHotspotTemperature = thermalSettings.sensor[0].currentTemp;
        }

        thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_ALL;
        status = (*_NvAPI_GPU_GetThermalSettings)(m_gpuHandle, 0, &thermalSettings);
        if (status != NVAPI_OK) {
            return;
        }
        else if (thermalSettings.sensor[0].currentTemp > m_gpuHotspotTemperature)
        {
            m_gpuHotspotTemperature = thermalSettings.sensor[0].currentTemp;
        }
    }
}

void NvapiHandler::readGpuFanSpeed()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetTachReading"])
    {
        NvAPI_Status status;

        unsigned long rpmSpeed = 0;
        //@note: fails on some gpus because of no support.
        status = (*_NvAPI_GPU_GetTachReading)(m_gpuHandle, &rpmSpeed);
        if (status == NVAPI_OK) {
            m_gpuFanSpeed = rpmSpeed;
        }
    }
}


void NvapiHandler::readGpuCooler()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetCoolerSettings"])
    {
        NvAPI_Status status;

        NV_GPU_COOLER_SETTINGS cooler_settings;
        cooler_settings.version = NV_GPU_COOLER_SETTINGS_VER;
        status = (*_NvAPI_GPU_GetCoolerSettings)(m_gpuHandle, NVAPI_COOLER_TARGET_GPU, &cooler_settings);
        if (status != NVAPI_OK) {
            return;
        }
        status = (*_NvAPI_GPU_GetCoolerSettings)(m_gpuHandle, NVAPI_COOLER_TARGET_ALL, &cooler_settings);
        if (status != NVAPI_OK) {
            return;
        }
    }
}