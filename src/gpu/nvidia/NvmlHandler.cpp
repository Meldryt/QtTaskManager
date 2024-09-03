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
    qDebug() << __FUNCTION__;
}

NvmlHandler::~NvmlHandler()
{
    qDebug() << __FUNCTION__;

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

    m_functionsSupportStatus["nvmlInit"] = false;
    m_functionsSupportStatus["nvmlDeviceGetHandleByIndex"] = false;

    nvmlReturn_t status;
    if(_nvmlInit)
    {
        status = _nvmlInit();
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlInit"] = _nvmlErrorString(status);
            qWarning() << "_nvmlInit failed! status: " << _nvmlErrorString(status);
            return false;
        }
        else
        {
            m_functionsSupportStatus["nvmlInit"] = true;
        }
    }

    if (_nvmlDeviceGetHandleByIndex)
    {
        status = _nvmlDeviceGetHandleByIndex(0, &nvmlGpuHandle);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetHandleByIndex"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetHandleByIndex failed! status: " << _nvmlErrorString(status);
            return false;
        }
        else
        {
            m_functionsSupportStatus["nvmlDeviceGetHandleByIndex"] = true;
            m_initialized = true;
        }
    }

    if(!m_initialized)
    {
        _nvmlShutdown();
    }

    return true;
}

void NvmlHandler::readStaticInfo()
{
    nvmlReturn_t status;

    m_functionsSupportStatus["nvmlDeviceGetCount"] = false;
    m_functionsSupportStatus["nvmlDeviceGetPowerManagementMode"] = false;
    m_functionsSupportStatus["nvmlDeviceGetPciInfo"] = false;

    if (!m_initialized)
    {
        return;
    }

    if (_nvmlDeviceGetCount)
    {
        uint deviceCount;
        status = _nvmlDeviceGetCount(&deviceCount);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetCount"] = _nvmlErrorString(status);
            qWarning() << "nvmlDeviceGetCount failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetCount"] = true;
        }
    }

    if (_nvmlDeviceGetPowerManagementMode)
    {
        nvmlEnableState_t enableState;
        status = _nvmlDeviceGetPowerManagementMode(nvmlGpuHandle, &enableState);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetPowerManagementMode"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetPowerManagementMode failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetPowerManagementMode"] = true;
        }
    }

    if (_nvmlDeviceGetPciInfo)
    {
        nvmlPciInfo_t pciInfo;
        // Get PCI information of the device.
        status = _nvmlDeviceGetPciInfo(nvmlGpuHandle, &pciInfo);
        if (status != NVML_SUCCESS)
        {
            m_functionsStatusMessage["nvmlDeviceGetPciInfo"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetPciInfo failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetPciInfo"] = true;
        }
    }

    checkSupportedDynamicFunctions();
}

void NvmlHandler::checkSupportedDynamicFunctions()
{
    nvmlReturn_t status;

    m_functionsSupportStatus["nvmlDeviceGetMemoryInfo"] = false;
    m_functionsSupportStatus["nvmlDeviceGetMemoryInfo_v2"] = false;
    m_functionsSupportStatus["nvmlDeviceGetTemperature"] = false;
    m_functionsSupportStatus["nvmlDeviceGetFanSpeed"] = false;
    m_functionsSupportStatus["nvmlDeviceGetFanSpeed_v2"] = false;
    m_functionsSupportStatus["nvmlDeviceGetNumFans"] = false;
    m_functionsSupportStatus["nvmlDeviceGetPowerUsage"] = false;
    m_functionsSupportStatus["nvmlDeviceGetTotalEnergyConsumption"] = false;
    m_functionsSupportStatus["nvmlDeviceGetPowerSource"] = false;
    m_functionsSupportStatus["nvmlDeviceGetPerformanceState"] = false;
    m_functionsSupportStatus["nvmlDeviceGetPowerManagementLimitConstraints"] = false;

    if (_nvmlDeviceGetMemoryInfo)
    {
        nvmlMemory_t memory;
        status = _nvmlDeviceGetMemoryInfo(nvmlGpuHandle, &memory);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetMemoryInfo"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetMemoryInfo failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetMemoryInfo"] = true;
        }
    }

    if (_nvmlDeviceGetMemoryInfo_v2)
    {
        nvmlMemory_v2_t memory;
        status = _nvmlDeviceGetMemoryInfo_v2(nvmlGpuHandle, &memory);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetMemoryInfo_v2"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetMemoryInfo_v2 failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetMemoryInfo_v2"] = true;
        }
    }

    if (_nvmlDeviceGetTemperature)
    {
        nvmlReturn_t status;
        uint temperature;

        status = _nvmlDeviceGetTemperature(nvmlGpuHandle, NVML_TEMPERATURE_GPU, &temperature);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetTemperature"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetTemperature failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetTemperature"] = true;
        }
    }

    uint fanSpeed;

    if (_nvmlDeviceGetFanSpeed)
    {
        status = _nvmlDeviceGetFanSpeed(nvmlGpuHandle, &fanSpeed);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetFanSpeed"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetFanSpeed failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetFanSpeed"] = true;
        }
    }

    if (_nvmlDeviceGetFanSpeed_v2)
    {
        status = _nvmlDeviceGetFanSpeed_v2(nvmlGpuHandle, 0, &fanSpeed);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetFanSpeed_v2"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetFanSpeed_v2 failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetFanSpeed_v2"] = true;
        }
    }

    if (_nvmlDeviceGetNumFans)
    {
        unsigned int numFans;
        status = _nvmlDeviceGetNumFans(nvmlGpuHandle, &numFans);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetNumFans"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetNumFans failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetNumFans"] = true;
        }
    }

    if (_nvmlDeviceGetPowerUsage)
    {
        unsigned int powerUsage;
        status = _nvmlDeviceGetPowerUsage(nvmlGpuHandle, &powerUsage);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetPowerUsage"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetPowerUsage failed! status: " << _nvmlErrorString(status); 
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetPowerUsage"] = true;
        }

    }

    if (_nvmlDeviceGetTotalEnergyConsumption)
    {
        unsigned long long totalConsumption;
        status = _nvmlDeviceGetTotalEnergyConsumption(nvmlGpuHandle, &totalConsumption);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetTotalEnergyConsumption"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetTotalEnergyConsumption failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetTotalEnergyConsumption"] = true;
        }
    }

    if (_nvmlDeviceGetPowerSource)
    {
        nvmlPowerSource_t powerSource;

        status = _nvmlDeviceGetPowerSource(nvmlGpuHandle, &powerSource);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetPowerSource"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetPowerSource failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetPowerSource"] = true;
        }
    }

    //nvmlPstates_t pStatesOld;
    //status = _nvmlDeviceGetPowerState(nvmlGpuHandle, &pStatesOld);
    //if (status != NVML_SUCCESS) {
    //    qWarning() << "_nvmlDeviceGetPowerState failed! status: " << _nvmlErrorString(status);
    //}

    if (_nvmlDeviceGetPerformanceState)
    {
        nvmlPstates_t pStates;
        status = _nvmlDeviceGetPerformanceState(nvmlGpuHandle, &pStates);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetPerformanceState"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetPerformanceState failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetPerformanceState"] = true;
        }
    }

    if (_nvmlDeviceGetPowerManagementLimitConstraints)
    {
        unsigned int minLimit, maxLimit;
        status = _nvmlDeviceGetPowerManagementLimitConstraints(nvmlGpuHandle, &minLimit, &maxLimit);
        if (status != NVML_SUCCESS) {
            m_functionsStatusMessage["nvmlDeviceGetPowerManagementLimitConstraints"] = _nvmlErrorString(status);
            qWarning() << "_nvmlDeviceGetPowerManagementLimitConstraints failed! status: " << _nvmlErrorString(status);
        }
        else{
            m_functionsSupportStatus["nvmlDeviceGetPowerManagementLimitConstraints"] = true;
        }
    }
}

void NvmlHandler::readDynamicInfo()
{
    if (!m_initialized)
    {
        return;
    }

    readGpuMemory();
    readGpuTemperature();
    readGpuFanSpeed();
    readGpuPowerUsage();
}

void NvmlHandler::readGpuMemory()
{
    nvmlReturn_t status;

    if (m_functionsSupportStatus["nvmlDeviceGetMemoryInfo"])
    {
        nvmlMemory_t memory;
        status = _nvmlDeviceGetMemoryInfo(nvmlGpuHandle, &memory);
        if (status == NVML_SUCCESS) {
        }
    }

    if (m_functionsSupportStatus["nvmlDeviceGetMemoryInfo_v2"])
    {
        nvmlMemory_v2_t memory;
        status = _nvmlDeviceGetMemoryInfo_v2(nvmlGpuHandle, &memory);
        if (status == NVML_SUCCESS) {
        }
    }
}

void NvmlHandler::readGpuTemperature()
{
    if (m_functionsSupportStatus["nvmlDeviceGetTemperature"])
    {
        nvmlReturn_t status;
        uint temperature;

        status = _nvmlDeviceGetTemperature(nvmlGpuHandle, NVML_TEMPERATURE_GPU, &temperature);
        if (status == NVML_SUCCESS) {
            m_gpuTemperature = temperature;
        }
    }
}

void NvmlHandler::readGpuFanSpeed()
{
    nvmlReturn_t status;
    uint fanSpeed;

    if (m_functionsSupportStatus["nvmlDeviceGetFanSpeed"])
    {
        status = _nvmlDeviceGetFanSpeed(nvmlGpuHandle, &fanSpeed);
        if (status == NVML_SUCCESS) {
            m_gpuFanSpeed = fanSpeed;
        }
    }

    if (m_functionsSupportStatus["nvmlDeviceGetFanSpeed_v2"])
    {
        status = _nvmlDeviceGetFanSpeed_v2(nvmlGpuHandle, 0, &fanSpeed);
        if (status == NVML_SUCCESS) {
            m_gpuFanSpeed = fanSpeed;
        }
    }

    if (m_functionsSupportStatus["nvmlDeviceGetNumFans"])
    {
        unsigned int numFans;
        status = _nvmlDeviceGetNumFans(nvmlGpuHandle, &numFans);
        if (status == NVML_SUCCESS) {
        }
    }
}

void NvmlHandler::readGpuPowerUsage()
{
    nvmlReturn_t status;

    if (m_functionsSupportStatus["nvmlDeviceGetPowerUsage"])
    {
        unsigned int powerUsage;
        status = _nvmlDeviceGetPowerUsage(nvmlGpuHandle, &powerUsage);
        if (status == NVML_SUCCESS) {
            m_gpuPower = powerUsage * 1000;
        }

    }

    if (m_functionsSupportStatus["nvmlDeviceGetTotalEnergyConsumption"])
    {
        unsigned long long totalConsumption;
        status = _nvmlDeviceGetTotalEnergyConsumption(nvmlGpuHandle, &totalConsumption);
        if (status == NVML_SUCCESS) {
        }
    }

    if (m_functionsSupportStatus["nvmlDeviceGetPowerSource"])
    {
        nvmlPowerSource_t powerSource;

        status = _nvmlDeviceGetPowerSource(nvmlGpuHandle, &powerSource);
        if (status == NVML_SUCCESS) {
        }
    }

    //nvmlPstates_t pStatesOld;
    //status = _nvmlDeviceGetPowerState(nvmlGpuHandle, &pStatesOld);
    //if (status != NVML_SUCCESS) {
    //    qWarning() << "_nvmlDeviceGetPowerState failed! status: " << _nvmlErrorString(status);
    //}

    if (m_functionsSupportStatus["nvmlDeviceGetPerformanceState"])
    {
        nvmlPstates_t pStates;
        status = _nvmlDeviceGetPerformanceState(nvmlGpuHandle, &pStates);
        if (status == NVML_SUCCESS) {
            m_gpuPowerState = pStates;
        }
    }

    if (m_functionsSupportStatus["nvmlDeviceGetPowerManagementLimitConstraints"])
    {
        unsigned int minLimit, maxLimit;
        status = _nvmlDeviceGetPowerManagementLimitConstraints(nvmlGpuHandle, &minLimit, &maxLimit);
        if (status == NVML_SUCCESS) {
        }
    }
}