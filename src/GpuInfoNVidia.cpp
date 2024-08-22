#include "GpuInfoNVidia.h"

#include <windows.h>

#include "external/nvapi/nvapi.h"


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
    NV_RAM_MAKER_MICRON
} NV_RAM_MAKER;

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
} NV_RAM_TYPE;

// function pointer types
typedef int *(*NvAPI_QueryInterface_t)(unsigned int offset);
typedef int (*NvAPI_Initialize_t)();
typedef int (*NvAPI_EnumPhysicalGPUs_t)(int **handles, int *count);
typedef int (*NvAPI_GPU_GetUsages_t)(int *handle, unsigned int *usages);
typedef int (*NvAPI_GPU_GetThermalSettings_t)(int *handle, int sensorIndex, NV_GPU_THERMAL_SETTINGS *temp);
typedef int (*NvAPI_GPU_GetMemoryInfoEx_t)(int* handle, NV_GPU_MEMORY_INFO_EX *memory_info);
typedef int (*NvAPI_GPU_GetMemoryInfo_t)(int* handle, NV_DISPLAY_DRIVER_MEMORY_INFO* pMemoryInfo);
typedef int (*NvAPI_GPU_GetTachReading_t)(int* handle, long* speed);
//typedef int (*NvAPI_GPU_GetCoolerSettings_t)(int* handle, int coolerIndex, NV_GPU_COOLER_SETTINGS* pCoolerInfo);
typedef int (*NvAPI_GPU_GetFullName_t)(int *handle , char* name);
typedef int (*NvAPI_GPU_GetAllClockFrequencies_t)(int *handle, NV_GPU_CLOCK_FREQUENCIES *clock_freq);
typedef int (*NvAPI_GPU_GetDynamicPstatesInfoEx_t)(int *handle, NV_GPU_DYNAMIC_PSTATES_INFO_EX *pstates_info);
typedef int (*NvAPI_GPU_GetRamMaker_t)(int *handle, NV_RAM_MAKER* maker);
typedef int (*NvAPI_GPU_GetRamType_t)(int *handle, NV_RAM_TYPE* type);

// nvapi.dll internal function pointers
NvAPI_QueryInterface_t          _NvAPI_QueryInterface     = NULL;
NvAPI_Initialize_t              _NvAPI_Initialize         = NULL;
NvAPI_EnumPhysicalGPUs_t        _NvAPI_EnumPhysicalGPUs   = NULL;
NvAPI_GPU_GetUsages_t           _NvAPI_GPU_GetUsages      = NULL;
NvAPI_GPU_GetThermalSettings_t	_NvAPI_GPU_GetThermalSettings = NULL;
NvAPI_GPU_GetMemoryInfoEx_t     _NvAPI_GPU_GetMemoryInfoEx = NULL;
NvAPI_GPU_GetMemoryInfo_t       _NvAPI_GPU_GetMemoryInfo = NULL;
//NvAPI_GPU_GetRamType_t        _NvAPI_GPU_GetRamType = NULL;
NvAPI_GPU_GetTachReading_t      _NvAPI_GPU_GetTachReading = NULL;
//NvAPI_GPU_GetCoolerSettings_t  _NvAPI_GPU_GetCoolerSettings = NULL;
NvAPI_GPU_GetFullName_t         _NvAPI_GPU_GetFullName = NULL;
NvAPI_GPU_GetAllClockFrequencies_t _NvAPI_GPU_GetAllClockFrequencies = NULL;
NvAPI_GPU_GetDynamicPstatesInfoEx_t _NvAPI_GPU_GetDynamicPstatesInfoEx = NULL;
NvAPI_GPU_GetRamMaker_t         _NvAPI_GPU_GetRamMaker = NULL;
NvAPI_GPU_GetRamType_t          _NvAPI_GPU_GetRamType = NULL;
// static size_t convertToByteUnit(const size_t val, const ByteUnit unit) {
//     return (val >> (size_t) unit);
// }

// static uint MAKE_NVAPI_VERSION<T>(int version)
// {
//     return (UInt32)((Marshal.SizeOf(typeof(T))) | (int)(version << 16));
// }

typedef NV_GPU_MEMORY_INFO_EX_V1 NV_GPU_MEMORY_INFO_EX;

#define NV_GPU_MEMORY_INFO_EX_VER_1  MAKE_NVAPI_VERSION(NV_GPU_MEMORY_INFO_EX_V1,1)

GpuInfoNVidia::GpuInfoNVidia()
{
    m_gpuChipDesigner = "NVIDIA";
}

/*
 * Name: NvGpuDetected
 * Desc: Returns true if an NVIDIA Gpu has been detected on this system.
 * NOTE: This function depends on whether a valid NVIDIA driver is installed
 *       on the target machine.  Since the Surface Hub does not include an
 *		 nvapi[64].dll in it's strippified driver, we need to load it directly
 *		 in order for the required APIs to work.
 */
bool GpuInfoNVidia::detectGpu()
{
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
    _NvAPI_EnumPhysicalGPUs          = (NvAPI_EnumPhysicalGPUs_t) (*_NvAPI_QueryInterface)(0xE5AC921F);
    _NvAPI_GPU_GetUsages             = (NvAPI_GPU_GetUsages_t) (*_NvAPI_QueryInterface)(0x189A1FDF);
    _NvAPI_GPU_GetThermalSettings    = (NvAPI_GPU_GetThermalSettings_t) (*_NvAPI_QueryInterface)(0xE3640A56);
    _NvAPI_GPU_GetMemoryInfoEx       = (NvAPI_GPU_GetMemoryInfoEx_t) (*_NvAPI_QueryInterface)(0xc0599498);
    _NvAPI_GPU_GetMemoryInfo         = (NvAPI_GPU_GetMemoryInfo_t) (*_NvAPI_QueryInterface)(0x774AA982);
    //_NvAPI_GPU_GetRamType            = (NvAPI_GPU_GetRamType_t) (*_NvAPI_QueryInterface)(0x57F7CAACUL);
    _NvAPI_GPU_GetTachReading        = (NvAPI_GPU_GetTachReading_t) (*_NvAPI_QueryInterface)(0x5F608315UL);
    _NvAPI_GPU_GetAllClockFrequencies = (NvAPI_GPU_GetAllClockFrequencies_t) (*_NvAPI_QueryInterface)(0xDCB616C3);
    _NvAPI_GPU_GetDynamicPstatesInfoEx = (NvAPI_GPU_GetDynamicPstatesInfoEx_t) (*_NvAPI_QueryInterface)(0x60DED2ED);
    //_NvAPI_GPU_GetCoolerSettings     = (NvAPI_GPU_GetCoolerSettings_t)(*NvAPI_QueryInterface)(0xDA141340UL);
    _NvAPI_GPU_GetFullName           = (NvAPI_GPU_GetFullName_t)(*_NvAPI_QueryInterface)(0xCEEE8e9FUL);

    _NvAPI_GPU_GetRamMaker           = (NvAPI_GPU_GetRamMaker_t)(*_NvAPI_QueryInterface)(0x42AEA16A);
    _NvAPI_GPU_GetRamType            = (NvAPI_GPU_GetRamType_t)(*_NvAPI_QueryInterface)(0x57F7CAAC);

    if(_NvAPI_Initialize == NULL || _NvAPI_EnumPhysicalGPUs == NULL ||
        _NvAPI_EnumPhysicalGPUs == NULL || _NvAPI_GPU_GetUsages == NULL )
    {
        qWarning() << "Couldn't get functions in nvapi.dll";
        return false;
    }

    // initialize NvAPI library, call it once before calling any other NvAPI functions
    if( (*_NvAPI_Initialize)() != 0 )
    {
        qWarning() << "Could not initialize nvapi!";
    }

    return true;
}

/*
 * Name: NvGetGpuInfoNVidia
 * Desc: Returns the NVIDIA Gpu's current load percentage.
 */
void GpuInfoNVidia::fetchStaticInfo()
{
    int status;

    status = (*_NvAPI_Initialize)();
    if (status != NVAPI_OK) {
        qWarning() << "_NvAPI_Initialize failed! status: " << status;
        return;
    }

    int          gpuCount = 0;
    int         *gpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { NULL };
    char gpuName[256] = { 0 };

    status = (*_NvAPI_EnumPhysicalGPUs)( gpuHandles, &gpuCount );
    if (status != NVAPI_OK) {
        qWarning() << "_NvAPI_EnumPhysicalGPUs failed! status: " << status;
        return;
    }


    status = (*_NvAPI_GPU_GetFullName)( gpuHandles[0], gpuName );
    if (status != NVAPI_OK) {
        qWarning() << "_NvAPI_GPU_GetFullName failed! status: " << status;
        return;
    }

    m_gpuModel = gpuName;

    //NV_RAM_TYPE RamType;
    //(*NvAPI_GPU_GetRamType)(AddElem->pAdapterIndex, &RamType);

    //NV_GPU_MEMORY_INFO_EX memoryInfo = { sizeof(NV_GPU_MEMORY_INFO_EX) | (1 << 16) };
    // NV_DISPLAY_DRIVER_MEMORY_INFO memoryInfo = { NV_DISPLAY_DRIVER_MEMORY_INFO_VER };
    // (*_NvAPI_GPU_GetMemoryInfo)(gpuHandles[0], &memoryInfo);

    NV_GPU_MEMORY_INFO_EX memory_info;
    memory_info.version = NV_GPU_MEMORY_INFO_EX_VER;
    status = (*_NvAPI_GPU_GetMemoryInfoEx)(gpuHandles[0], &memory_info);
    if (status != NVAPI_OK) {
        qWarning() << "_NvAPI_Initialize failed! status: " << status;
        return;
    }

    m_gpuMemorySize = memory_info.curAvailableDedicatedVideoMemory;
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
}

/*
 * Name: NvGetGpuInfoNVidia
 * Desc: Returns the NVIDIA Gpu's current load percentage.
 */
void GpuInfoNVidia::fetchDynamicInfo()
{
    int status;

    status = (*_NvAPI_Initialize)();
    if (status != NVAPI_OK) {
        qWarning() << "_NvAPI_Initialize failed! status: " << status;
        return;
    }

    NV_GPU_THERMAL_SETTINGS 		 nvgts;
    int          gpuCount = 0;
    int         *gpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { NULL };
    unsigned int gpuUsages[NVAPI_MAX_USAGES_PER_GPU] = { 0 };

    // gpuUsages[0] must be this value, otherwise NvAPI_GPU_GetUsages won't work
    gpuUsages[0] = (NVAPI_MAX_USAGES_PER_GPU * 4) | 0x10000;

    status = (*_NvAPI_EnumPhysicalGPUs)( gpuHandles, &gpuCount );
    if (status != NVAPI_OK) {
        qWarning() << "_NvAPI_EnumPhysicalGPUs failed! status: " << status;
        return;
    }

    nvgts.version = sizeof(NV_GPU_THERMAL_SETTINGS) | (1<<16);
    nvgts.count = 0;
    nvgts.sensor[0].controller = NV_THERMAL_CONTROLLER::NVAPI_THERMAL_CONTROLLER_UNKNOWN;
    nvgts.sensor[0].target = NVAPI_THERMAL_TARGET_GPU;

    status = (*_NvAPI_GPU_GetThermalSettings)(gpuHandles[0], 0 ,&nvgts);
    if (status != NVAPI_OK) {
        qWarning() << "_NvAPI_GPU_GetThermalSettings failed! status: " << status;
        return;
    }

    m_gpuTemperature = nvgts.sensor[0].currentTemp;

    status = (*_NvAPI_GPU_GetUsages)( gpuHandles[0], gpuUsages );
    if (status != NVAPI_OK) {
        qWarning() << "_NvAPI_GPU_GetUsages failed! status: " << status;
        return;
    }

    m_gpuUsage = gpuUsages[3];

    long RPMSpeed = 0;
    status = (*_NvAPI_GPU_GetTachReading)(gpuHandles[0], &RPMSpeed);
    if (status != NVAPI_OK) {
        qWarning() << "_NvAPI_GPU_GetTachReading failed! status: " << status;
        return;
    }

    NV_GPU_MEMORY_INFO_EX memory_info;
    memory_info.version = NV_GPU_MEMORY_INFO_EX_VER;
    status = (*_NvAPI_GPU_GetMemoryInfoEx)(gpuHandles[0], &memory_info);
    if (status != NVAPI_OK) {
        qWarning() << "_NvAPI_GPU_GetMemoryInfoEx failed! status: " << status;
        return;
    }

    m_gpuVramUsed = memory_info.availableDedicatedVideoMemory - memory_info.curAvailableDedicatedVideoMemory;
    m_gpuVramUsage = (static_cast<double>(m_gpuVramUsed) / m_gpuMemorySize) * 100.0;

    NV_GPU_CLOCK_FREQUENCIES frequencies;
    frequencies.version = NV_GPU_CLOCK_FREQUENCIES_VER;
    frequencies.ClockType = NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ;
    // frequencies.reserved = 0;
    // frequencies.reserved1 = 0;
    status = status = (*_NvAPI_GPU_GetAllClockFrequencies)(gpuHandles[0], &frequencies);
    if (status != NVAPI_OK) {
        qWarning() << "_NvAPI_GPU_GetAllClockFrequencies failed! status: " << status;
        return;
    }
    if(frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].bIsPresent)
    {
        m_gpuClockSpeed = frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency / 1000;
    }
    if(frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].bIsPresent)
    {
        m_gpuVramClockSpeed = frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency / 1000;
    }
    if(frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].bIsPresent)
    {
        //m_gpuClockSpeed = frequencies.domain[NVAPI_GPU_PUBLIC_CLOCK_VIDEO].frequency / 1000;
    }

    NV_GPU_DYNAMIC_PSTATES_INFO_EX pstates_info;
    pstates_info.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;
    status = (*_NvAPI_GPU_GetDynamicPstatesInfoEx)(gpuHandles[0], &pstates_info);
    if (status != NVAPI_OK) {
        qWarning() << "NvAPI_GPU_GetDynamicPstatesInfoEx failed! status: " << status;
        return;
    }
    m_gpuUsage = pstates_info.utilization[0].percentage;
    m_gpuVramUsage = pstates_info.utilization[1].percentage;

    NV_GPU_THERMAL_SETTINGS thermal_settings;
    thermal_settings.version = NV_GPU_THERMAL_SETTINGS_VER; //NV_GPU_THERMAL_SETTINGS_VER_2 ?
    status = (*_NvAPI_GPU_GetThermalSettings)(gpuHandles[0], NVAPI_THERMAL_TARGET_ALL, &thermal_settings);
    if (status != NVAPI_OK) {
        qWarning() << "NvAPI_GPU_GetThermalSettings failed! status: " << status;
        return;
    }
    m_gpuTemperature = thermal_settings.sensor[0].currentTemp;

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
