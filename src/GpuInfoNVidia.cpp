#include "GpuInfoNVidia.h"

#include <windows.h>

#include "external/nvapi/nvapi.h"

#include "external/nvml/nvml.h"

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


#define NVML_DLL "nvml.dll"

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
typedef NvAPI_Status *(*NvAPI_QueryInterface_t)(unsigned int offset);
typedef NvAPI_Status (*NvAPI_Initialize_t)();
typedef NvAPI_Status (*NvAPI_EnumPhysicalGPUs_t)(int **handles, int *count);
typedef NvAPI_Status (*NvAPI_GPU_GetUsages_t)(int *handle, unsigned int *usages);
typedef NvAPI_Status (*NvAPI_GPU_GetThermalSettings_t)(int *handle, int sensorIndex, NV_GPU_THERMAL_SETTINGS *temp);
typedef NvAPI_Status (*NvAPI_GPU_GetMemoryInfoEx_t)(int* handle, NV_GPU_MEMORY_INFO_EX *memory_info);
typedef NvAPI_Status (*NvAPI_GPU_GetMemoryInfo_t)(int* handle, NV_DISPLAY_DRIVER_MEMORY_INFO* pMemoryInfo);
typedef NvAPI_Status (*NvAPI_GPU_GetTachReading_t)(int* handle, unsigned long* speed);
typedef NvAPI_Status (*NvAPI_GPU_GetCoolerSettings_t)(int* handle, int coolerIndex, NV_GPU_COOLER_SETTINGS* pCoolerInfo);
typedef NvAPI_Status (*NvAPI_GPU_GetFullName_t)(int *handle , char* name);
typedef NvAPI_Status (*NvAPI_GPU_GetAllClockFrequencies_t)(int *handle, NV_GPU_CLOCK_FREQUENCIES *clock_freq);
typedef NvAPI_Status (*NvAPI_GPU_GetDynamicPstatesInfoEx_t)(int *handle, NV_GPU_DYNAMIC_PSTATES_INFO_EX *pstates_info);
typedef NvAPI_Status (*NvAPI_GPU_GetRamMaker_t)(int *handle, NV_RAM_MAKER* maker);
typedef NvAPI_Status (*NvAPI_GPU_GetRamType_t)(int *handle, NV_RAM_TYPE* type);
typedef NvAPI_Status (*NvAPI_GetErrorMessage_t)(NvAPI_Status, NvAPI_ShortString);
typedef NvAPI_Status (*NvAPI_SYS_GetDisplayDriverInfo_t)(NV_DISPLAY_DRIVER_INFO* pDriverInfo);

// nvapi.dll internal function pointers
NvAPI_QueryInterface_t              _NvAPI_QueryInterface     = NULL;
NvAPI_Initialize_t                  _NvAPI_Initialize         = NULL;
NvAPI_EnumPhysicalGPUs_t            _NvAPI_EnumPhysicalGPUs   = NULL;
NvAPI_GPU_GetUsages_t               _NvAPI_GPU_GetUsages      = NULL;
NvAPI_GPU_GetThermalSettings_t	    _NvAPI_GPU_GetThermalSettings = NULL;
NvAPI_GPU_GetMemoryInfoEx_t         _NvAPI_GPU_GetMemoryInfoEx = NULL;
NvAPI_GPU_GetMemoryInfo_t           _NvAPI_GPU_GetMemoryInfo = NULL;
NvAPI_GPU_GetTachReading_t          _NvAPI_GPU_GetTachReading = NULL;
NvAPI_GPU_GetCoolerSettings_t       _NvAPI_GPU_GetCoolerSettings = NULL;
NvAPI_GPU_GetFullName_t             _NvAPI_GPU_GetFullName = NULL;
NvAPI_GPU_GetAllClockFrequencies_t  _NvAPI_GPU_GetAllClockFrequencies = NULL;
NvAPI_GPU_GetDynamicPstatesInfoEx_t _NvAPI_GPU_GetDynamicPstatesInfoEx = NULL;
NvAPI_GPU_GetRamMaker_t             _NvAPI_GPU_GetRamMaker = NULL;
NvAPI_GPU_GetRamType_t              _NvAPI_GPU_GetRamType = NULL;
NvAPI_GetErrorMessage_t             _NvAPI_GetErrorMessage = NULL;
NvAPI_SYS_GetDisplayDriverInfo_t    _NvAPI_SYS_GetDisplayDriverInfo = NULL;

GpuInfoNVidia::GpuInfoNVidia()
{
    qDebug() << __FUNCTION__;

    m_gpuChipDesigner = "NVIDIA";
}

GpuInfoNVidia::~GpuInfoNVidia()
{
    qDebug() << __FUNCTION__;
}

bool GpuInfoNVidia::init()
{
    if (initNvApi() && initNvml())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * Name: NvGpuDetected
 * Desc: Returns true if an NVIDIA Gpu has been detected on this system.
 * NOTE: This function depends on whether a valid NVIDIA driver is installed
 *       on the target machine.  Since the Surface Hub does not include an
 *		 nvapi[64].dll in it's strippified driver, we need to load it directly
 *		 in order for the required APIs to work.
 */
bool GpuInfoNVidia::initNvApi()
{
    qDebug() << __FUNCTION__;

    HMODULE hmod = LoadLibraryA( NVAPI_DLL );

    if( hmod == NULL )
    {
        //qDebug() << "Couldn't find " << NVAPI_DLL;
        return false;
    }
    else
    {
        //qDebug() << "GPU Detected";
    }

    // nvapi_QueryInterface is a function used to retrieve other internal functions in nvapi.dll
    _NvAPI_QueryInterface = (NvAPI_QueryInterface_t) GetProcAddress( hmod, "nvapi_QueryInterface" );

    // some useful internal functions that aren't exported by nvapi.dll
    _NvAPI_Initialize                = (NvAPI_Initialize_t) (*_NvAPI_QueryInterface)(0x0150E828);
    _NvAPI_GetErrorMessage           = (NvAPI_GetErrorMessage_t)(*_NvAPI_QueryInterface)(0x6C2D048C);
    _NvAPI_EnumPhysicalGPUs          = (NvAPI_EnumPhysicalGPUs_t) (*_NvAPI_QueryInterface)(0xE5AC921F);
    _NvAPI_GPU_GetUsages             = (NvAPI_GPU_GetUsages_t) (*_NvAPI_QueryInterface)(0x189A1FDF);
    _NvAPI_GPU_GetThermalSettings    = (NvAPI_GPU_GetThermalSettings_t) (*_NvAPI_QueryInterface)(0xE3640A56);
    _NvAPI_GPU_GetMemoryInfoEx       = (NvAPI_GPU_GetMemoryInfoEx_t) (*_NvAPI_QueryInterface)(0xc0599498);
    _NvAPI_GPU_GetMemoryInfo         = (NvAPI_GPU_GetMemoryInfo_t) (*_NvAPI_QueryInterface)(0x774AA982);
    //_NvAPI_GPU_GetRamType            = (NvAPI_GPU_GetRamType_t) (*_NvAPI_QueryInterface)(0x57F7CAACUL);
    _NvAPI_GPU_GetTachReading        = (NvAPI_GPU_GetTachReading_t) (*_NvAPI_QueryInterface)(0x5F608315);
    _NvAPI_GPU_GetAllClockFrequencies = (NvAPI_GPU_GetAllClockFrequencies_t) (*_NvAPI_QueryInterface)(0xDCB616C3);
    _NvAPI_GPU_GetDynamicPstatesInfoEx = (NvAPI_GPU_GetDynamicPstatesInfoEx_t) (*_NvAPI_QueryInterface)(0x60DED2ED);
    _NvAPI_GPU_GetCoolerSettings     = (NvAPI_GPU_GetCoolerSettings_t)(*_NvAPI_QueryInterface)(0xDA141340);
    _NvAPI_GPU_GetFullName           = (NvAPI_GPU_GetFullName_t)(*_NvAPI_QueryInterface)(0xCEEE8e9F);

    _NvAPI_GPU_GetRamMaker           = (NvAPI_GPU_GetRamMaker_t)(*_NvAPI_QueryInterface)(0x42AEA16A);
    _NvAPI_GPU_GetRamType            = (NvAPI_GPU_GetRamType_t)(*_NvAPI_QueryInterface)(0x57F7CAAC);
    _NvAPI_SYS_GetDisplayDriverInfo  = (NvAPI_SYS_GetDisplayDriverInfo_t)(*_NvAPI_QueryInterface)(0x721faceb);

    if(_NvAPI_Initialize == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_Initialize in nvapi.dll";
        return false;
    }
    if (_NvAPI_GetErrorMessage == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GetErrorMessage in nvapi.dll";
        return false;
    }
    if (_NvAPI_EnumPhysicalGPUs == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_EnumPhysicalGPUs in nvapi.dll";
        return false;
    }
    if (_NvAPI_GPU_GetUsages == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GPU_GetUsages in nvapi.dll";
    }
    if (_NvAPI_GPU_GetThermalSettings == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GPU_GetThermalSettings in nvapi.dll";
    }
    if (_NvAPI_GPU_GetMemoryInfoEx == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GPU_GetMemoryInfoEx in nvapi.dll";
    }
    if (_NvAPI_GPU_GetMemoryInfo == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GPU_GetMemoryInfo in nvapi.dll";
    }
    if (_NvAPI_GPU_GetTachReading == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GPU_GetTachReading in nvapi.dll";
    }
    if (_NvAPI_GPU_GetAllClockFrequencies == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GPU_GetAllClockFrequencies in nvapi.dll";
    }
    if (_NvAPI_GPU_GetDynamicPstatesInfoEx == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GPU_GetDynamicPstatesInfoEx in nvapi.dll";
    }
    if (_NvAPI_GPU_GetCoolerSettings == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GPU_GetCoolerSettings in nvapi.dll";
    }
    if (_NvAPI_GPU_GetFullName == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GPU_GetFullName in nvapi.dll";
    }
    if (_NvAPI_GPU_GetRamMaker == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GPU_GetRamMaker in nvapi.dll";
    }
    if (_NvAPI_GPU_GetRamType == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_GPU_GetRamType in nvapi.dll";
    }
    if (_NvAPI_SYS_GetDisplayDriverInfo == NULL)
    {
        qWarning() << "Couldn't get function _NvAPI_SYS_GetDisplayDriverInfo in nvapi.dll";
    }
    // initialize NvAPI library, call it once before calling any other NvAPI functions
    if( (*_NvAPI_Initialize)() != 0 )
    {
        qWarning() << "Could not initialize nvapi!";
    }

    return true;
}

#define dlsym (void *) GetProcAddress
#define dlclose FreeLibrary

HMODULE hDLL;

extern bool opt_nonvml;

static char* (*_nvmlErrorString)(nvmlReturn_t);
static nvmlReturn_t(*_nvmlInit)();
static nvmlReturn_t(*_nvmlDeviceGetCount)(uint*);
static nvmlReturn_t(*_nvmlDeviceGetHandleByIndex)(uint, nvmlDevice_t*);
static nvmlReturn_t(*_nvmlDeviceGetName)(nvmlDevice_t, char*, uint);
static nvmlReturn_t(*_nvmlDeviceGetPciInfo)(nvmlDevice_t, nvmlPciInfo_t*);
static nvmlReturn_t(*_nvmlDeviceGetTemperature)(nvmlDevice_t, nvmlTemperatureSensors_t, uint*);
static nvmlReturn_t(*_nvmlDeviceGetFanSpeed)(nvmlDevice_t, uint*);
static nvmlReturn_t(*_nvmlDeviceGetFanSpeed_v2)(nvmlDevice_t, unsigned int, unsigned int*);
static nvmlReturn_t(*_nvmlDeviceGetNumFans)(nvmlDevice_t, unsigned int*);
static nvmlReturn_t(*_nvmlShutdown)();
static nvmlReturn_t(*_nvmlDeviceGetPowerUsage)(nvmlDevice_t, unsigned int*);
static nvmlReturn_t(*_nvmlDeviceGetPowerState)(nvmlDevice_t, nvmlPstates_t*);
static nvmlReturn_t(*_nvmlDeviceGetPowerSource)(nvmlDevice_t, nvmlPowerSource_t*);
static nvmlReturn_t(*_nvmlDeviceGetPerformanceState)(nvmlDevice_t, nvmlPstates_t*);
static nvmlReturn_t(*_nvmlDeviceGetPowerManagementMode)(nvmlDevice_t, nvmlEnableState_t*);
static nvmlReturn_t(*_nvmlDeviceGetPowerManagementLimitConstraints)(nvmlDevice_t, unsigned int*, unsigned int*);
static nvmlReturn_t(*_nvmlDeviceGetTotalEnergyConsumption)(nvmlDevice_t, unsigned long long*);

nvmlDevice_t nvmlGpuHandle;

bool GpuInfoNVidia::initNvml()
{
    qDebug() << __FUNCTION__;

    /* Not in system path, but could be local */
    hDLL = LoadLibraryA("nvml.dll");
    if (!hDLL) {
        /* %ProgramW6432% is unsupported by OS prior to year 2009 */
        char path[512];
        ExpandEnvironmentStringsA("%ProgramFiles%\\NVIDIA Corporation\\NVSMI\\nvml.dll", path, sizeof(path));
        hDLL = LoadLibraryA(path);
    }

    if (!hDLL) {
        return false;
    }

    _nvmlInit = (nvmlReturn_t(*)()) dlsym(hDLL, "nvmlInit_v2");
    if (!_nvmlInit) {
        /* Try an older interface */
        _nvmlInit = (nvmlReturn_t(*)()) dlsym(hDLL, "nvmlInit");
        if (!_nvmlInit) {
            return false;
        }
        else {
            _nvmlDeviceGetCount = (nvmlReturn_t(*)(uint*)) \
            dlsym(hDLL, "nvmlDeviceGetCount");
            _nvmlDeviceGetHandleByIndex = (nvmlReturn_t(*)(uint, nvmlDevice_t*)) \
            dlsym(hDLL, "nvmlDeviceGetHandleByIndex");
            _nvmlDeviceGetPciInfo = (nvmlReturn_t(*)(nvmlDevice_t, nvmlPciInfo_t*)) \
            dlsym(hDLL, "nvmlDeviceGetPciInfo");
        }
    }
    else {
        _nvmlDeviceGetCount = (nvmlReturn_t(*)(uint*)) \
        dlsym(hDLL, "nvmlDeviceGetCount_v2");
        _nvmlDeviceGetHandleByIndex = (nvmlReturn_t(*)(uint, nvmlDevice_t*)) \
        dlsym(hDLL, "nvmlDeviceGetHandleByIndex_v2");
        _nvmlDeviceGetPciInfo = (nvmlReturn_t(*)(nvmlDevice_t, nvmlPciInfo_t*)) \
        dlsym(hDLL, "nvmlDeviceGetPciInfo_v2");
    }

    _nvmlErrorString = (char* (*)(nvmlReturn_t)) \
    dlsym(hDLL, "nvmlErrorString");
    _nvmlDeviceGetName = (nvmlReturn_t(*)(nvmlDevice_t, char*, uint)) \
    dlsym(hDLL, "nvmlDeviceGetName");
    _nvmlDeviceGetTemperature = (nvmlReturn_t(*)(nvmlDevice_t, nvmlTemperatureSensors_t, uint*)) \
    dlsym(hDLL, "nvmlDeviceGetTemperature");
    _nvmlDeviceGetFanSpeed = (nvmlReturn_t(*)(nvmlDevice_t, uint*)) \
    dlsym(hDLL, "nvmlDeviceGetFanSpeed");
    _nvmlDeviceGetFanSpeed_v2 = (nvmlReturn_t(*)(nvmlDevice_t, uint, uint*)) \
    dlsym(hDLL, "nvmlDeviceGetFanSpeed_v2");
    _nvmlDeviceGetNumFans = (nvmlReturn_t(*)(nvmlDevice_t, uint*)) \
    dlsym(hDLL, "nvmlDeviceGetNumFans");
    _nvmlShutdown = (nvmlReturn_t(*)()) \
    dlsym(hDLL, "nvmlShutdown");
    _nvmlDeviceGetPowerUsage = (nvmlReturn_t(*)(nvmlDevice_t, unsigned int*)) \
    dlsym(hDLL, "nvmlDeviceGetPowerUsage");
    _nvmlDeviceGetPowerState = (nvmlReturn_t(*)(nvmlDevice_t, nvmlPstates_t*)) \
    dlsym(hDLL, "nvmlDeviceGetPowerState");
    _nvmlDeviceGetPowerSource = (nvmlReturn_t(*)(nvmlDevice_t, nvmlPowerSource_t*)) \
    dlsym(hDLL, "nvmlDeviceGetPowerSource");
    _nvmlDeviceGetPerformanceState = (nvmlReturn_t(*)(nvmlDevice_t, nvmlPstates_t*)) \
    dlsym(hDLL, "nvmlDeviceGetPerformanceState");
    _nvmlDeviceGetPowerManagementMode = (nvmlReturn_t(*)(nvmlDevice_t, nvmlEnableState_t*)) \
    dlsym(hDLL, "nvmlDeviceGetPowerManagementMode");
    _nvmlDeviceGetPowerManagementLimitConstraints = (nvmlReturn_t(*)(nvmlDevice_t, unsigned int*, unsigned int*)) \
    dlsym(hDLL, "nvmlDeviceGetPowerManagementLimitConstraints");
    _nvmlDeviceGetTotalEnergyConsumption = (nvmlReturn_t(*)(nvmlDevice_t, unsigned long long*)) \
    dlsym(hDLL, "nvmlDeviceGetTotalEnergyConsumption");

    nvmlReturn_t status = _nvmlInit();
    if (status != NVML_SUCCESS) {
        qWarning() << "_nvmlInit failed! status: " << _nvmlErrorString(status);
        return false;
    }

    if (_nvmlDeviceGetHandleByIndex)
    {
        status = _nvmlDeviceGetHandleByIndex(0, &nvmlGpuHandle);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetHandleByIndex failed! status: " << _nvmlErrorString(status);
            return false;
        }
    }

    if (_nvmlDeviceGetCount)
    {
        uint deviceCount;
        status = _nvmlDeviceGetCount(&deviceCount);
        if (status != NVML_SUCCESS) {
            qWarning() << "nvmlDeviceGetCount failed! status: " << _nvmlErrorString(status);
            return false;
        }
    }

    if (_nvmlDeviceGetPowerManagementMode)
    {
        nvmlEnableState_t enableState;
        status = _nvmlDeviceGetPowerManagementMode(nvmlGpuHandle, &enableState);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetPowerManagementMode failed! status: " << _nvmlErrorString(status);
        }
        qDebug() << "_nvmlDeviceGetPowerManagementMode: " << enableState;
    }

    if (_nvmlDeviceGetPciInfo)
    {
        nvmlPciInfo_t pciInfo;
        // Get PCI information of the device.
        status = _nvmlDeviceGetPciInfo(nvmlGpuHandle, &pciInfo);
        if (status != NVML_SUCCESS)
        {
            qWarning() << "_nvmlDeviceGetPciInfo failed! status: " << _nvmlErrorString(status);
        }
    }
    
    //HMODULE hmod = LoadLibraryA(NVML_DLL);

    //if (hmod == NULL)
    //{
    //    //qDebug() << "Couldn't find " << NVAPI_DLL;
    //    return false;
    //}
    //else
    //{
    //    //qDebug() << "GPU Detected";
    //}

    //nvmlReturn_t result;
    //nvmlDevice_t device;
    //nvmlMemory_t memory;
    //unsigned int device_count, i;

    //result = nvmlInit();
    //if (result != NVML_SUCCESS) {
    //    printf("Failed to initialize NVML: %s\n", nvmlErrorString(result));
    //    return 1;
    //}

    //result = nvmlDeviceGetCount(&device_count);
    //if (result != NVML_SUCCESS) {
    //    printf("Failed to get device count: %s\n", nvmlErrorString(result));
    //    nvmlShutdown();
    //    return 1;
    //}

    //for (i = 0; i < device_count; i++) {
    //    result = nvmlDeviceGetHandleByIndex(i, &device);
    //    if (result != NVML_SUCCESS) {
    //        printf("Failed to get device handle for device %d: %s\n", i, nvmlErrorString(result));
    //        continue;
    //    }

    //    result = nvmlDeviceGetMemoryInfo(device, &memory);
    //    if (result != NVML_SUCCESS) {
    //        printf("Failed to get memory info for device %d: %s\n", i, nvmlErrorString(result));
    //        continue;
    //    }

    //    printf("Device %d:\n", i);
    //    printf("  Total memory: %lu\n", memory.total);
    //    printf("  Free memory: %lu\n", memory.free);
    //    printf("  Used memory: %lu\n", memory.used);
    //}

    //nvmlShutdown();

    return true;
}

void GpuInfoNVidia::nvmlGpuTempAndFanspeed() 
{
    qDebug() << __FUNCTION__;
    nvmlReturn_t status;
    uint nTemp, nSpeed;

    if (_nvmlDeviceGetTemperature)
    {
        status = _nvmlDeviceGetTemperature(nvmlGpuHandle, NVML_TEMPERATURE_GPU, &nTemp);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetTemperature failed! status: " << _nvmlErrorString(status);
            return;
        }
    }

    if (_nvmlDeviceGetFanSpeed)
    {
        status = _nvmlDeviceGetFanSpeed(nvmlGpuHandle, &nSpeed);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetFanSpeed failed! status: " << _nvmlErrorString(status);
            //return;
        }
    }

    if (_nvmlDeviceGetFanSpeed_v2)
    {
        status = _nvmlDeviceGetFanSpeed_v2(nvmlGpuHandle, 0, &nSpeed);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetFanSpeed_v2 failed! status: " << _nvmlErrorString(status);
            //return;
        }
    }

    if (_nvmlDeviceGetNumFans)
    {
        unsigned int numFans;
        status = _nvmlDeviceGetNumFans(nvmlGpuHandle, &numFans);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetNumFans failed! status: " << _nvmlErrorString(status);
            //return;
        }
    }
}

/*
 * Name: NvGetGpuInfoNVidia
 * Desc: Returns the NVIDIA Gpu's current load percentage.
 */
void GpuInfoNVidia::fetchStaticInfo()
{
    qDebug() << __FUNCTION__;

    NvAPI_Status status;

    status = (*_NvAPI_Initialize)();
    if (status != NVAPI_OK) {
        NvAPI_ShortString string;
        (*_NvAPI_GetErrorMessage)(status, string);
        qWarning() << "_NvAPI_Initialize failed! status: " << string;
        return;
    }

    int          gpuCount = 0;
    int         *gpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { NULL };
    char gpuName[256] = { 0 };

    status = (*_NvAPI_EnumPhysicalGPUs)(gpuHandles, &gpuCount);
    if (status != NVAPI_OK) {
        NvAPI_ShortString string;
        (*_NvAPI_GetErrorMessage)(status, string);
        qWarning() << "_NvAPI_EnumPhysicalGPUs failed! status: " << string;
        return;
    }

    if (_NvAPI_GPU_GetFullName != NULL)
    {
        status = (*_NvAPI_GPU_GetFullName)(gpuHandles[0], gpuName);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetFullName failed! status: " << string;
            return;
        }

        m_gpuModel = gpuName;
    }

    if (_NvAPI_GPU_GetMemoryInfo != NULL)
    {
        NV_DISPLAY_DRIVER_MEMORY_INFO memory_info;
        memory_info.version = NV_DISPLAY_DRIVER_MEMORY_INFO_VER;
        status = (*_NvAPI_GPU_GetMemoryInfo)(gpuHandles[0], &memory_info);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetMemoryInfo failed! status: " << string;
            return;
        }

        m_gpuMemorySize = memory_info.dedicatedVideoMemory / 1024 / 1024;
    }
    else if (_NvAPI_GPU_GetMemoryInfoEx != NULL)
    {
        NV_GPU_MEMORY_INFO_EX memory_info;
        memory_info.version = NV_GPU_MEMORY_INFO_EX_VER;
        status = (*_NvAPI_GPU_GetMemoryInfoEx)(gpuHandles[0], &memory_info);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetMemoryInfoEx failed! status: " << string;
            return;
        }

        m_gpuMemorySize = memory_info.dedicatedVideoMemory / 1024 / 1024;
    }

    if (_NvAPI_GPU_GetRamType != NULL)
    {
        NV_RAM_TYPE ramType;
        status = (*_NvAPI_GPU_GetRamType)(gpuHandles[0], &ramType);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetRamType failed! status: " << string;
            return;
        }
        m_gpuMemoryType = _NV_RAM_TYPE_NAME[ramType];
    }

    if (_NvAPI_GPU_GetRamMaker != NULL)
    {
        NV_RAM_MAKER ramMaker;
        status = (*_NvAPI_GPU_GetRamMaker)(gpuHandles[0], &ramMaker);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetRamMaker failed! status: " << string;
            return;
        }
        m_gpuMemoryVendor = _NV_RAM_MAKER_NAME[ramMaker];
    }

    if (_NvAPI_SYS_GetDisplayDriverInfo)
    {
        NV_DISPLAY_DRIVER_INFO driverInfo = { NV_DISPLAY_DRIVER_INFO_VER };
        status = (*_NvAPI_SYS_GetDisplayDriverInfo)(&driverInfo);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_SYS_GetDisplayDriverInfo failed! status: " << string;
            return;
        }

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

    //@todo: add missing info
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_ChipDesigner] = QString::fromStdString(m_gpuChipDesigner);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_CardManufacturer] = QString::fromStdString(m_gpuCardManufacturer);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Model] = QString::fromStdString(m_gpuModel);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_MemorySize] = m_gpuMemorySize;
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_MemoryType] = QString::fromStdString(m_gpuMemoryType);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_MemoryVendor] = QString::fromStdString(m_gpuMemoryVendor);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_DriverInfo] = QString::fromStdString(m_gpuDriverInfo);
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_DriverVersion] = QString::fromStdString(m_gpuDriverVersion);
}

/*
 * Name: NvGetGpuInfoNVidia
 * Desc: Returns the NVIDIA Gpu's current load percentage.
 */
void GpuInfoNVidia::fetchDynamicInfo()
{
    qDebug() << __FUNCTION__;

    NvAPI_Status status;

    status = (*_NvAPI_Initialize)();
    if (status != NVAPI_OK) {
        NvAPI_ShortString string;
        (*_NvAPI_GetErrorMessage)(status, string);
        qWarning() << "_NvAPI_Initialize failed! status: " << string;
        return;
    }

    int          gpuCount = 0;
    int         *gpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { NULL };

    status = (*_NvAPI_EnumPhysicalGPUs)( gpuHandles, &gpuCount );
    if (status != NVAPI_OK) {
        NvAPI_ShortString string;
        (*_NvAPI_GetErrorMessage)(status, string);
        qWarning() << "_NvAPI_EnumPhysicalGPUs failed! status: " << string;
        return;
    }

    readGpuUsage(gpuHandles[0]);
    readGpuFrequencies(gpuHandles[0]);
    readGpuMemory(gpuHandles[0]);
    readGpuTemperature(gpuHandles[0]); 
    readGpuFanSpeed(gpuHandles[0]);

    nvmlGpuTempAndFanspeed();
    readGpuPowerUsage();

    if (_NvAPI_GPU_GetCoolerSettings != NULL)
    {
        NV_GPU_COOLER_SETTINGS cooler_settings;
        cooler_settings.version = NV_GPU_COOLER_SETTINGS_VER;
        status = (*_NvAPI_GPU_GetCoolerSettings)(gpuHandles[0], NVAPI_COOLER_TARGET_GPU, &cooler_settings);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetCoolerSettings failed! status: " << string;
            return;
        }
        status = (*_NvAPI_GPU_GetCoolerSettings)(gpuHandles[0], NVAPI_COOLER_TARGET_ALL, &cooler_settings);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetCoolerSettings failed! status: " << string;
            return;
        }
    }

    //stats->used_memory = ConvertToByteUnit(memory_info.availableDedicatedVideoMemory - memory_info.curAvailableDedicatedVideoMemory, ECS_BYTE_TO_MB);
    //stats->utilization = pstate_ex.utilization[0].bIsPresent ? pstate_ex.utilization[0].percentage : 0;
    //stats->temperature_core = thermal_settings.count > 0 ? thermal_settings.sensor[0].currentTemp : 0;
    //// These are in kHz and we want MHz
    //stats->clock_core = (frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].bIsPresent ? 
    //    frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency : 0) / ECS_KB_10;
    //stats->clock_memory = (frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].bIsPresent ?
    //    frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency : 0) / ECS_KB_10;

    // At the moment there is no power API
    //stats->power_draw = 0;

    //@todo: add missing info
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Usage] = m_gpuUsage;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_ClockSpeed] = m_gpuClockSpeed;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamUsage] = m_gpuVramUsage;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamClockSpeed] = m_gpuVramClockSpeed;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamUsed] = m_gpuVramUsed;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Power] = m_gpuPower;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_TotalBoardPower] = m_gpuTotalBoardPower;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Voltage] = m_gpuVoltage;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Temperature] = m_gpuTemperature;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_HotSpotTemperature] = m_gpuHotspotTemperature;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_FanSpeed] = m_gpuFanSpeed;
    m_dynamicInfo[Globals::SysInfoAttr::Key_Gpu_FanSpeedUsage] = m_gpuFanSpeedUsage;
}

bool GpuInfoNVidia::readGpuUsage(int* gpuHandle)
{
    if (_NvAPI_GPU_GetDynamicPstatesInfoEx != NULL)
    {
        NvAPI_Status status;
        unsigned int gpuUsages[NVAPI_MAX_USAGES_PER_GPU] = { 0 };

        //@note: we can use both nvapi functions to get gpu usage
        // gpuUsages[0] must be this value, otherwise NvAPI_GPU_GetUsages won't work
        //gpuUsages[0] = (NVAPI_MAX_USAGES_PER_GPU * 4) | 0x10000;

        //status = (*_NvAPI_GPU_GetUsages)(gpuHandle, gpuUsages);
        //if (status != NVAPI_OK) {
        //    qWarning() << "_NvAPI_GPU_GetUsages failed! status: " << status;
        //    return false;
        //}

        //m_gpuUsage = gpuUsages[3];

        NV_GPU_DYNAMIC_PSTATES_INFO_EX pstates_info;
        pstates_info.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;
        status = (*_NvAPI_GPU_GetDynamicPstatesInfoEx)(gpuHandle, &pstates_info);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetDynamicPstatesInfoEx failed! status: " << string;
            return false;
        }
        m_gpuUsage = pstates_info.utilization[0].percentage;
        m_gpuVramUsage = pstates_info.utilization[1].percentage;
    }

    return true;
}

bool GpuInfoNVidia::readGpuFrequencies(int* gpuHandle)
{
    if (_NvAPI_GPU_GetAllClockFrequencies != NULL)
    {
        NvAPI_Status status;

        NV_GPU_CLOCK_FREQUENCIES frequencies;
        frequencies.version = NV_GPU_CLOCK_FREQUENCIES_VER;
        frequencies.ClockType = NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ;
        status = (*_NvAPI_GPU_GetAllClockFrequencies)(gpuHandle, &frequencies);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetAllClockFrequencies failed! status: " << string;
            return false;
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

    return true;
}

bool GpuInfoNVidia::readGpuMemory(int* gpuHandle)
{
    if (_NvAPI_GPU_GetMemoryInfoEx != NULL)
    {
        NvAPI_Status status;

        NV_GPU_MEMORY_INFO_EX memory_info;
        memory_info.version = NV_GPU_MEMORY_INFO_EX_VER;
        status = (*_NvAPI_GPU_GetMemoryInfoEx)(gpuHandle, &memory_info);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetMemoryInfoEx failed! status: " << string;
            return false;
        }

        const uint32_t gpuVramUsed = (memory_info.availableDedicatedVideoMemory - memory_info.curAvailableDedicatedVideoMemory);
        m_gpuVramUsed = (gpuVramUsed) / 1024 / 1024;
        m_gpuVramUsage = (static_cast<double>(gpuVramUsed) / memory_info.availableDedicatedVideoMemory) * 100.0;
    }

    return true;
}

bool GpuInfoNVidia::readGpuTemperature(int* gpuHandle)
{
    if (_NvAPI_GPU_GetThermalSettings != NULL)
    {
        NvAPI_Status status;
        NV_GPU_THERMAL_SETTINGS thermalSettings;
        thermalSettings.version = NV_GPU_THERMAL_SETTINGS_VER;
        thermalSettings.count = 0;
        thermalSettings.sensor[0].controller = NV_THERMAL_CONTROLLER::NVAPI_THERMAL_CONTROLLER_UNKNOWN;
        thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_GPU;

        status = (*_NvAPI_GPU_GetThermalSettings)(gpuHandle, 0, &thermalSettings);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetThermalSettings for TARGET_GPU failed! status: " << string;
            return false;
        }

        m_gpuTemperature = thermalSettings.sensor[0].currentTemp;
        m_gpuHotspotTemperature = m_gpuTemperature;

        thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_MEMORY;
        status = (*_NvAPI_GPU_GetThermalSettings)(gpuHandle, 0, &thermalSettings);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetThermalSettings for TARGET_MEMORY failed! status: " << string;
            return false;
        }
        else if (thermalSettings.sensor[0].currentTemp > m_gpuHotspotTemperature)
        {
            m_gpuHotspotTemperature = thermalSettings.sensor[0].currentTemp;
        }


        thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_POWER_SUPPLY;
        status = (*_NvAPI_GPU_GetThermalSettings)(gpuHandle, 0, &thermalSettings);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetThermalSettings POWER_SUPPLY failed! status: " << string;
            return false;
        }
        else if (thermalSettings.sensor[0].currentTemp > m_gpuHotspotTemperature)
        {
            m_gpuHotspotTemperature = thermalSettings.sensor[0].currentTemp;
        }

        thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_BOARD;
        status = (*_NvAPI_GPU_GetThermalSettings)(gpuHandle, 0, &thermalSettings);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetThermalSettings TARGET_BOARD failed! status: " << string;
            return false;
        }
        else if (thermalSettings.sensor[0].currentTemp > m_gpuHotspotTemperature)
        {
            m_gpuHotspotTemperature = thermalSettings.sensor[0].currentTemp;
        }

        thermalSettings.sensor[0].target = NVAPI_THERMAL_TARGET_ALL;
        status = (*_NvAPI_GPU_GetThermalSettings)(gpuHandle, 0, &thermalSettings);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetThermalSettings TARGET_ALL failed! status: " << string;
            return false;
        }
        else if (thermalSettings.sensor[0].currentTemp > m_gpuHotspotTemperature)
        {
            m_gpuHotspotTemperature = thermalSettings.sensor[0].currentTemp;
        }
    }

    return true;
}

bool GpuInfoNVidia::readGpuFanSpeed(int* gpuHandle)
{
    if (_NvAPI_GPU_GetTachReading != NULL)
    {
        NvAPI_Status status;

        unsigned long rpmSpeed = 0;
        //@note: fails on some gpus because of no support.
        status = (*_NvAPI_GPU_GetTachReading)(gpuHandle, &rpmSpeed);
        if (status != NVAPI_OK) {
            NvAPI_ShortString string;
            (*_NvAPI_GetErrorMessage)(status, string);
            qWarning() << "_NvAPI_GPU_GetTachReading failed! status: " << string;
            //return;
        }
        else
        {
            m_gpuFanSpeed = rpmSpeed;
        }
    }

    return true;
}

bool GpuInfoNVidia::readGpuPowerUsage()
{
    nvmlReturn_t status;

    if (_nvmlDeviceGetPowerUsage != NULL)
    {
        unsigned int powerUsage;
        status = _nvmlDeviceGetPowerUsage(nvmlGpuHandle, &powerUsage);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetPowerUsage failed! status: " << _nvmlErrorString(status); 
        }
        else {
            m_gpuPower = powerUsage * 1000;
        }

    }

    if (_nvmlDeviceGetTotalEnergyConsumption != NULL)
    {
        unsigned long long totalConsumption;
        status = _nvmlDeviceGetTotalEnergyConsumption(nvmlGpuHandle, &totalConsumption);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetTotalEnergyConsumption failed! status: " << _nvmlErrorString(status);
        }
    }

    if (_nvmlDeviceGetPowerSource != NULL)
    {
        nvmlPowerSource_t powerSource;

        status = _nvmlDeviceGetPowerSource(nvmlGpuHandle, &powerSource);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetPowerSource failed! status: " << _nvmlErrorString(status);
        }
    }

    //nvmlPstates_t pStatesOld;
    //status = _nvmlDeviceGetPowerState(nvmlGpuHandle, &pStatesOld);
    //if (status != NVML_SUCCESS) {
    //    qWarning() << "_nvmlDeviceGetPowerState failed! status: " << _nvmlErrorString(status);
    //}

    if (_nvmlDeviceGetPerformanceState != NULL)
    {
        nvmlPstates_t pStates;
        status = _nvmlDeviceGetPerformanceState(nvmlGpuHandle, &pStates);
        if (status != NVML_SUCCESS) {
            m_gpuPowerState = pStates;
            qWarning() << "_nvmlDeviceGetPerformanceState failed! status: " << _nvmlErrorString(status);
        }
        else {
            m_gpuPowerState = pStates;
        }
    }

    if (_nvmlDeviceGetPowerManagementLimitConstraints != NULL)
    {
        unsigned int minLimit, maxLimit;
        status = _nvmlDeviceGetPowerManagementLimitConstraints(nvmlGpuHandle, &minLimit, &maxLimit);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetPowerManagementLimitConstraints failed! status: " << _nvmlErrorString(status);
        }
    }

    return false;
}