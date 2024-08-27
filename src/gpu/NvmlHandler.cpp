#include "NvmlHandler.h"

#include <windows.h>

#include "external/nvml/nvml.h"

#include <QDebug>

#define NVML_DLL "nvml.dll"

#define dlsym (void *) GetProcAddress
#define dlclose FreeLibrary

HMODULE hDLL;

static char* (*_nvmlErrorString)(nvmlReturn_t);
static nvmlReturn_t(*_nvmlInit)();
static nvmlReturn_t(*_nvmlDeviceGetCount)(uint*);
static nvmlReturn_t(*_nvmlDeviceGetHandleByIndex)(uint, nvmlDevice_t*);
static nvmlReturn_t(*_nvmlDeviceGetName)(nvmlDevice_t, char*, uint);
static nvmlReturn_t(*_nvmlDeviceGetMemoryInfo)(nvmlDevice_t, nvmlMemory_t*);
static nvmlReturn_t(*_nvmlDeviceGetMemoryInfo_v2)(nvmlDevice_t, nvmlMemory_v2_t*);
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

NvmlHandler::NvmlHandler()
{
    
}

NvmlHandler::~NvmlHandler()
{

    if (_nvmlShutdown)
    {
        nvmlReturn_t status = _nvmlShutdown();
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlShutdown failed! status: " << _nvmlErrorString(status);
        }
    }
}

bool NvmlHandler::init()
{
    qDebug() << __FUNCTION__;

    /* Not in system path, but could be local */
    hDLL = LoadLibraryA("nvml.dll");
    if (!hDLL) 
    {
        /* %ProgramW6432% is unsupported by OS prior to year 2009 */
        char path[512];
        ExpandEnvironmentStringsA("%ProgramFiles%\\NVIDIA Corporation\\NVSMI\\nvml.dll", path, sizeof(path));
        hDLL = LoadLibraryA(path);
    }

    if (!hDLL) 
    {
        return false;
    }

    _nvmlInit = (nvmlReturn_t(*)()) dlsym(hDLL, "nvmlInit_v2");
    if (!_nvmlInit) 
    {
        /* Try an older interface */
        _nvmlInit = (nvmlReturn_t(*)()) dlsym(hDLL, "nvmlInit");
        if (!_nvmlInit) 
        {
            return false;
        }
        else 
        {
            _nvmlDeviceGetCount = (nvmlReturn_t(*)(uint*)) \
            dlsym(hDLL, "nvmlDeviceGetCount");
            _nvmlDeviceGetHandleByIndex = (nvmlReturn_t(*)(uint, nvmlDevice_t*)) \
            dlsym(hDLL, "nvmlDeviceGetHandleByIndex");
            _nvmlDeviceGetPciInfo = (nvmlReturn_t(*)(nvmlDevice_t, nvmlPciInfo_t*)) \
            dlsym(hDLL, "nvmlDeviceGetPciInfo");
            _nvmlDeviceGetFanSpeed = (nvmlReturn_t(*)(nvmlDevice_t, uint*)) \
            dlsym(hDLL, "nvmlDeviceGetFanSpeed");
            _nvmlDeviceGetMemoryInfo = (nvmlReturn_t(*)(nvmlDevice_t, nvmlMemory_t*)) \
            dlsym(hDLL, "_nvmlDeviceGetMemoryInfo");
        }
    }
    else 
    {
        _nvmlDeviceGetCount = (nvmlReturn_t(*)(uint*)) \
        dlsym(hDLL, "nvmlDeviceGetCount_v2");
        _nvmlDeviceGetHandleByIndex = (nvmlReturn_t(*)(uint, nvmlDevice_t*)) \
        dlsym(hDLL, "nvmlDeviceGetHandleByIndex_v2");
        _nvmlDeviceGetPciInfo = (nvmlReturn_t(*)(nvmlDevice_t, nvmlPciInfo_t*)) \
        dlsym(hDLL, "nvmlDeviceGetPciInfo_v2");
        _nvmlDeviceGetFanSpeed_v2 = (nvmlReturn_t(*)(nvmlDevice_t, uint, uint*)) \
        dlsym(hDLL, "nvmlDeviceGetFanSpeed_v2");
        _nvmlDeviceGetMemoryInfo_v2 = (nvmlReturn_t(*)(nvmlDevice_t, nvmlMemory_v2_t*)) \
        dlsym(hDLL, "nvmlDeviceGetMemoryInfo_v2");
    }

    _nvmlErrorString = (char* (*)(nvmlReturn_t)) \
    dlsym(hDLL, "nvmlErrorString");
    _nvmlDeviceGetName = (nvmlReturn_t(*)(nvmlDevice_t, char*, uint)) \
    dlsym(hDLL, "nvmlDeviceGetName");
    _nvmlDeviceGetTemperature = (nvmlReturn_t(*)(nvmlDevice_t, nvmlTemperatureSensors_t, uint*)) \
    dlsym(hDLL, "nvmlDeviceGetTemperature");
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

    //nvmlShutdown();

    return true;
}

void NvmlHandler::readStaticInfo()
{
    nvmlReturn_t status;

    if (_nvmlDeviceGetCount)
    {
        uint deviceCount;
        status = _nvmlDeviceGetCount(&deviceCount);
        if (status != NVML_SUCCESS) {
            qWarning() << "nvmlDeviceGetCount failed! status: " << _nvmlErrorString(status);
        }
    }

    if (_nvmlDeviceGetPowerManagementMode)
    {
        nvmlEnableState_t enableState;
        status = _nvmlDeviceGetPowerManagementMode(nvmlGpuHandle, &enableState);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetPowerManagementMode failed! status: " << _nvmlErrorString(status);
        }
        m_powerManagementModeEnabled = enableState;
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
}

void NvmlHandler::readDynamicInfo()
{
    readGpuMemory();
    readGpuTemperature();
    readGpuFanSpeed();
    readGpuPowerUsage();
}

void NvmlHandler::readGpuMemory()
{
    nvmlReturn_t status;

    if (_nvmlDeviceGetMemoryInfo)
    {
        nvmlMemory_t memory;
        status = _nvmlDeviceGetMemoryInfo(nvmlGpuHandle, &memory);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetMemoryInfo failed! status: " << _nvmlErrorString(status);
        }
    }

    if (_nvmlDeviceGetMemoryInfo_v2)
    {
        nvmlMemory_v2_t memory;
        status = _nvmlDeviceGetMemoryInfo_v2(nvmlGpuHandle, &memory);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetMemoryInfo_v2 failed! status: " << _nvmlErrorString(status);
        }
    }
}

void NvmlHandler::readGpuTemperature()
{
    if (_nvmlDeviceGetTemperature)
    {
        nvmlReturn_t status;
        uint temperature;

        status = _nvmlDeviceGetTemperature(nvmlGpuHandle, NVML_TEMPERATURE_GPU, &temperature);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetTemperature failed! status: " << _nvmlErrorString(status);
        }
        else {
            m_gpuTemperature = temperature;
        }
    }
}

void NvmlHandler::readGpuFanSpeed()
{
    nvmlReturn_t status;
    uint fanSpeed;

    if (_nvmlDeviceGetFanSpeed)
    {
        status = _nvmlDeviceGetFanSpeed(nvmlGpuHandle, &fanSpeed);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetFanSpeed failed! status: " << _nvmlErrorString(status);
        }
        else {
            m_gpuFanSpeed = fanSpeed;
        }
    }

    if (_nvmlDeviceGetFanSpeed_v2)
    {
        status = _nvmlDeviceGetFanSpeed_v2(nvmlGpuHandle, 0, &fanSpeed);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetFanSpeed_v2 failed! status: " << _nvmlErrorString(status);
        }
        else {
            m_gpuFanSpeed = fanSpeed;
        }
    }

    if (_nvmlDeviceGetNumFans)
    {
        unsigned int numFans;
        status = _nvmlDeviceGetNumFans(nvmlGpuHandle, &numFans);
        if (status != NVML_SUCCESS) {
            qWarning() << "_nvmlDeviceGetNumFans failed! status: " << _nvmlErrorString(status);
        }
    }
}

void NvmlHandler::readGpuPowerUsage()
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
}