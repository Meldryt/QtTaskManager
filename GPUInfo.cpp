#include "GPUInfo.h"

#include <windows.h>

#ifndef NV_H
#define NV_H

#define NVAPI_MAX_THERMAL_SENSORS_PER_GPU 3
typedef enum
{
    NVAPI_THERMAL_TARGET_NONE          = 0,
    NVAPI_THERMAL_TARGET_GPU           = 1,     //!< GPU core temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_MEMORY        = 2,     //!< GPU memory temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_POWER_SUPPLY  = 4,     //!< GPU power supply temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_BOARD         = 8,     //!< GPU board ambient temperature requires NvPhysicalGpuHandle
    NVAPI_THERMAL_TARGET_VCD_BOARD     = 9,     //!< Visual Computing Device Board temperature requires NvVisualComputingDeviceHandle
    NVAPI_THERMAL_TARGET_VCD_INLET     = 10,    //!< Visual Computing Device Inlet temperature requires NvVisualComputingDeviceHandle
    NVAPI_THERMAL_TARGET_VCD_OUTLET    = 11,    //!< Visual Computing Device Outlet temperature requires NvVisualComputingDeviceHandle

    NVAPI_THERMAL_TARGET_ALL           = 15,
    NVAPI_THERMAL_TARGET_UNKNOWN       = -1,
} NV_THERMAL_TARGET;

typedef struct
{
    int   version;                //!< structure version
    int   count;                  //!< number of associated thermal sensors
    struct
    {
        int       controller;        //!< internal, ADM1032, MAX6649...
        int                       defaultMinTemp;    //!< The min default temperature value of the thermal sensor in degree Celsius
        int                       defaultMaxTemp;    //!< The max default temperature value of the thermal sensor in degree Celsius
        int                       currentTemp;       //!< The current temperature value of the thermal sensor in degree Celsius
        NV_THERMAL_TARGET           target;            //!< Thermal sensor targeted @ GPU, memory, chipset, powersupply, Visual Computing Device, etc.
    } sensor[NVAPI_MAX_THERMAL_SENSORS_PER_GPU];

} NV_GPU_THERMAL_SETTINGS;

//#include <nvapi.h>

#endif // NV_H

#if defined(_M_X64) || defined(__amd64__)
#define NVAPI_DLL "nvapi64.dll"
#else
#define NVAPI_DLL "nvapi.dll"
#endif


// magic numbers, do not change them
#define NVAPI_MAX_PHYSICAL_GPUS   64
#define NVAPI_MAX_USAGES_PER_GPU  34

// function pointer types
typedef int *(*NvAPI_QueryInterface_t)(unsigned int offset);
typedef int (*NvAPI_Initialize_t)();
typedef int (*NvAPI_EnumPhysicalGPUs_t)(int **handles, int *count);
typedef int (*NvAPI_GPU_GetUsages_t)(int *handle, unsigned int *usages);
typedef int (*NvAPI_GPU_GetThermalSettings_t)(int *handle, int sensorIndex, NV_GPU_THERMAL_SETTINGS *temp);

typedef int(*NvAPI_GPU_GetFullName_t)(int *handle , char* name);

// nvapi.dll internal function pointers
NvAPI_QueryInterface_t      NvAPI_QueryInterface     = NULL;
NvAPI_Initialize_t          NvAPI_Initialize         = NULL;
NvAPI_EnumPhysicalGPUs_t    NvAPI_EnumPhysicalGPUs   = NULL;
NvAPI_GPU_GetUsages_t       NvAPI_GPU_GetUsages      = NULL;
NvAPI_GPU_GetThermalSettings_t	NvAPI_GPU_GetThermalSettings = NULL;
NvAPI_GPU_GetFullName_t     NvAPI_GPU_GetFullName = NULL;


GPUInfo::GPUInfo()
{

}

void GPUInfo::init()
{
    detectGPU();
    if(gpuDetected)
    {
        fetchStaticInfo();
    }
}

void GPUInfo::update()
{
    if(gpuDetected)
    {
        fetchDynamicInfo();
    }
}

void GPUInfo::fetchStaticInfo()
{
    if(gpuManufacturer == GPUManufacturer::NVIDIA)
    {
        fetchStaticInfo_NVidia();
    }
    else
    {
        fetchStaticInfo_AMD();
    }
}

void GPUInfo::detectGPU()
{
    detectGPU_NVidia();
    if(gpuDetected)
    {
        gpuManufacturer = GPUManufacturer::NVIDIA;
        return;
    }

    detectGPU_AMD();
    if(gpuDetected)
    {
        gpuManufacturer = GPUManufacturer::AMD;
        return;
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
void GPUInfo::detectGPU_NVidia()
{
    HMODULE hmod = LoadLibraryA( NVAPI_DLL );

    if( hmod == NULL )
    {
        //qDebug() << "Couldn't find " << NVAPI_DLL;
        gpuDetected = false;
        return;
    }
    else
    {
        //qDebug() << "GPU Detected";
    }

    // nvapi_QueryInterface is a function used to retrieve other internal functions in nvapi.dll
    NvAPI_QueryInterface = (NvAPI_QueryInterface_t) GetProcAddress( hmod, "nvapi_QueryInterface" );

    // some useful internal functions that aren't exported by nvapi.dll
    NvAPI_Initialize = (NvAPI_Initialize_t) (*NvAPI_QueryInterface)(0x0150E828);
    NvAPI_EnumPhysicalGPUs = (NvAPI_EnumPhysicalGPUs_t) (*NvAPI_QueryInterface)(0xE5AC921F);
    NvAPI_GPU_GetUsages = (NvAPI_GPU_GetUsages_t) (*NvAPI_QueryInterface)(0x189A1FDF);
    NvAPI_GPU_GetThermalSettings = (NvAPI_GPU_GetThermalSettings_t) (*NvAPI_QueryInterface)(0xE3640A56);

    NvAPI_GPU_GetFullName=(NvAPI_GPU_GetFullName_t)(*NvAPI_QueryInterface)(0xCEEE8e9FUL);

    if( NvAPI_Initialize == NULL || NvAPI_EnumPhysicalGPUs == NULL ||
        NvAPI_EnumPhysicalGPUs == NULL || NvAPI_GPU_GetUsages == NULL )
    {
        //qDebug() << "Couldn't get functions in nvapi.dll";
        gpuDetected = false;
        return;
    }

    // initialize NvAPI library, call it once before calling any other NvAPI functions
    if( (*NvAPI_Initialize)() != 0 )
    {
        //qDebug() << "Could not initialize nvapi!";
    }

    gpuDetected = true;
}

void GPUInfo::detectGPU_AMD()
{

}

/*
 * Name: NvGetGpuInfo
 * Desc: Returns the NVIDIA Gpu's current load percentage.
 */
void GPUInfo::fetchStaticInfo_NVidia()
{
    (*NvAPI_Initialize)();
    int          gpuCount = 0;
    int         *gpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { NULL };
    char gpuName[256] = { 0 };

    (*NvAPI_EnumPhysicalGPUs)( gpuHandles, &gpuCount );

    (*NvAPI_GPU_GetFullName)( gpuHandles[0], gpuName );
    staticInfo.gpuBrand = gpuName;
}

void GPUInfo::fetchStaticInfo_AMD()
{

}

void GPUInfo::fetchDynamicInfo()
{
    if(gpuManufacturer == GPUManufacturer::NVIDIA)
    {
        fetchDynamicInfo_NVidia();
    }
    else
    {
        fetchDynamicInfo_AMD();
    }
}

/*
 * Name: NvGetGpuInfo
 * Desc: Returns the NVIDIA Gpu's current load percentage.
 */
void GPUInfo::fetchDynamicInfo_NVidia()
{
    (*NvAPI_Initialize)();
    NV_GPU_THERMAL_SETTINGS 		 nvgts;
    int          gpuCount = 0;
    int         *gpuHandles[NVAPI_MAX_PHYSICAL_GPUS] = { NULL };
    unsigned int gpuUsages[NVAPI_MAX_USAGES_PER_GPU] = { 0 };

    // gpuUsages[0] must be this value, otherwise NvAPI_GPU_GetUsages won't work
    gpuUsages[0] = (NVAPI_MAX_USAGES_PER_GPU * 4) | 0x10000;

    (*NvAPI_EnumPhysicalGPUs)( gpuHandles, &gpuCount );
    nvgts.version = sizeof(NV_GPU_THERMAL_SETTINGS) | (1<<16);
    nvgts.count = 0;
    nvgts.sensor[0].controller = -1;
    nvgts.sensor[0].target = NVAPI_THERMAL_TARGET_GPU;

    (*NvAPI_GPU_GetThermalSettings)(gpuHandles[0], 0 ,&nvgts);
    dynamicInfo.gpuTemperature = nvgts.sensor[0].currentTemp;

    (*NvAPI_GPU_GetUsages)( gpuHandles[0], gpuUsages );
    dynamicInfo.gpuTotalLoad = gpuUsages[3];
}

void GPUInfo::fetchDynamicInfo_AMD()
{

}

const GPUInfo::GPU_StaticInfo &GPUInfo::getStaticInfo() const
{
    return staticInfo;
}

const GPUInfo::GPU_DynamicInfo &GPUInfo::getDynamicInfo() const
{
    return dynamicInfo;
}
