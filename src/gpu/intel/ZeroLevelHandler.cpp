#include "ZeroLevelHandler.h"

#include <QDebug>

#include <stdlib.h>
#include <memory>

#ifdef USE_LEVEL_ZERO
#include "zello_init.h"

void print_loader_versions() {

    zel_component_version_t* versions;
    size_t size = 0;
    zelLoaderGetVersions(&size, nullptr);
    std::cout << "zelLoaderGetVersions number of components found: " << size << std::endl;
    versions = new zel_component_version_t[size];
    zelLoaderGetVersions(&size, versions);

    for (size_t i = 0; i < size; i++) {
        std::cout << "Version " << i << std::endl;
        std::cout << "Name: " << versions[i].component_name << std::endl;
        std::cout << "Major: " << versions[i].component_lib_version.major << std::endl;
        std::cout << "Minor: " << versions[i].component_lib_version.minor << std::endl;
        std::cout << "Patch: " << versions[i].component_lib_version.patch << std::endl;
    }

    delete[] versions;
}

#if defined(_WIN32)
#define putenv_safe _putenv
#else
#define putenv_safe putenv
#endif

#endif
/***************************************************************
 * @brief Main Function
 *
 * place_holder_for_Detailed_desc
 * @param
 * @return
 ***************************************************************/
int zeroLevelTest()
{
#ifdef USE_LEVEL_ZERO
    bool tracing_runtime_enabled = false;

    ze_result_t status;
    const ze_device_type_t type = ZE_DEVICE_TYPE_GPU;

    ze_driver_handle_t pDriver = nullptr;
    ze_device_handle_t pDevice = nullptr;
    if (init_ze())
    {

        print_loader_versions();

        if (tracing_runtime_enabled) {
            std::cout << "Enabling Tracing Layer after init" << std::endl;
            status = zelEnableTracingLayer();
            if (status != ZE_RESULT_SUCCESS) {
                std::cout << "zelEnableTracingLayer Failed with return code: " << to_string(status) << std::endl;
                exit(1);
            }
        }

        uint32_t driverCount = 0;
        status = zeDriverGet(&driverCount, nullptr);
        if (status != ZE_RESULT_SUCCESS) {
            std::cout << "zeDriverGet Failed with return code: " << to_string(status) << std::endl;
            exit(1);
        }

        std::vector<ze_driver_handle_t> drivers(driverCount);
        status = zeDriverGet(&driverCount, drivers.data());
        if (status != ZE_RESULT_SUCCESS) {
            std::cout << "zeDriverGet Failed with return code: " << to_string(status) << std::endl;
            exit(1);
        }

        for (uint32_t driver = 0; driver < driverCount; ++driver)
        {
            pDriver = drivers[driver];
            pDevice = findDevice(pDriver, type);
            if (pDevice)
            {
                break;
            }
        }
    }

    if (!pDevice)
    {
        std::cout << "Did NOT find matching " << to_string(type) << " device!" << "\n";
        return -1;
    }


    // Create the context
    ze_context_handle_t context;
    ze_context_desc_t context_desc = {};
    context_desc.stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC;
    status = zeContextCreate(pDriver, &context_desc, &context);
    if (status != ZE_RESULT_SUCCESS) {
        std::cout << "zeContextCreate Failed with return code: " << to_string(status) << std::endl;
        exit(1);
    }

    // Create an immediate command list for direct submission
    ze_command_queue_desc_t altdesc = {};
    altdesc.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC;
    ze_command_list_handle_t command_list = {};
    status = zeCommandListCreateImmediate(context, pDevice, &altdesc, &command_list);
    if (status != ZE_RESULT_SUCCESS) {
        std::cout << "zeCommandListCreateImmediate Failed with return code: " << to_string(status) << std::endl;
        exit(1);
    }

    // Create an event to be signaled by the device
    ze_event_pool_desc_t ep_desc = {};
    ep_desc.stype = ZE_STRUCTURE_TYPE_EVENT_POOL_DESC;
    ep_desc.count = 1;
    ep_desc.flags = ZE_EVENT_POOL_FLAG_HOST_VISIBLE;
    ze_event_desc_t ev_desc = {};
    ev_desc.stype = ZE_STRUCTURE_TYPE_EVENT_DESC;
    ev_desc.signal = ZE_EVENT_SCOPE_FLAG_HOST;
    ev_desc.wait = ZE_EVENT_SCOPE_FLAG_HOST;
    ze_event_handle_t event;
    ze_event_pool_handle_t event_pool;

    status = zeEventPoolCreate(context, &ep_desc, 1, &pDevice, &event_pool);
    if (status != ZE_RESULT_SUCCESS) {
        std::cout << "zeEventPoolCreate Failed with return code: " << to_string(status) << std::endl;
        exit(1);
    }

    status = zeEventCreate(event_pool, &ev_desc, &event);
    if (status != ZE_RESULT_SUCCESS) {
        std::cout << "zeEventCreate Failed with return code: " << to_string(status) << std::endl;
        exit(1);
    }

    // signal the event from the device and wait for completion
    zeCommandListAppendSignalEvent(command_list, event);
    zeEventHostSynchronize(event, UINT64_MAX);
    std::cout << "Congratulations, the device completed execution!\n";

    zeContextDestroy(context);
    zeCommandListDestroy(command_list);
    zeEventDestroy(event);
    zeEventPoolDestroy(event_pool);

    if (tracing_runtime_enabled) {
        std::cout << "Disable Tracing Layer after init" << std::endl;
        status = zelDisableTracingLayer();
        if (status != ZE_RESULT_SUCCESS) {
            std::cout << "zelDisableTracingLayer Failed with return code: " << to_string(status) << std::endl;
            exit(1);
        }
    }
#endif
    return 0;
}

ZeroLevelHandler::ZeroLevelHandler()
{
    qDebug() << __FUNCTION__;
}

ZeroLevelHandler::~ZeroLevelHandler()
{
    qDebug() << __FUNCTION__;
}

bool ZeroLevelHandler::init()
{

    return m_initialized;
}

void ZeroLevelHandler::readStaticInfo()
{
    zeroLevelTest();
    checkSupportedDynamicFunctions();
}

void ZeroLevelHandler::checkSupportedDynamicFunctions()
{
    m_functionsSupportStatus["NvAPI_GPU_GetDynamicPstatesInfoEx"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetAllClockFrequencies"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetThermalSettings"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetTachReading"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetTachReading"] = false;
}


void ZeroLevelHandler::readDynamicInfo()
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

void ZeroLevelHandler::readGpuUsage()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetDynamicPstatesInfoEx"])
    {

    }
}

void ZeroLevelHandler::readGpuFrequencies()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetAllClockFrequencies"])
    {

    }
}

void ZeroLevelHandler::readGpuMemory()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetMemoryInfoEx"])
    {

    }
}

void ZeroLevelHandler::readGpuTemperature()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetThermalSettings"])
    {

    }
}

void ZeroLevelHandler::readGpuFanSpeed()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetTachReading"])
    {

    }
}


void ZeroLevelHandler::readGpuCooler()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetCoolerSettings"])
    {

    }
}