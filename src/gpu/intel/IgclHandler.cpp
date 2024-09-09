#include "IgclHandler.h"

#include <crtdbg.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <vector>

#include <iostream>
#include <string>

#include <windows.h>
#include <stdio.h>
#include "igcl_api.h"
#include "GenericIGCLApp.h"

#include <QDebug>

/***************************************************************
 * @brief Main Function
 *
 * place_holder_for_Detailed_desc
 * @param
 * @return
 ***************************************************************/
int ctlTest()
{
    ctl_result_t Result = CTL_RESULT_SUCCESS;
    ctl_device_adapter_handle_t* hDevices = nullptr;
    ctl_device_adapter_properties_t StDeviceAdapterProperties = { 0 };
    // Get a handle to the DLL module.
    uint32_t Adapter_count = 0;
    uint32_t Display_count = 0;
    uint32_t Index = 0;
    uint32_t Display_index = 0;

    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    ctl_init_args_t CtlInitArgs;
    ctl_api_handle_t hAPIHandle;
    CtlInitArgs.AppVersion = CTL_MAKE_VERSION(CTL_IMPL_MAJOR_VERSION, CTL_IMPL_MINOR_VERSION);
    CtlInitArgs.flags = CTL_INIT_FLAG_USE_LEVEL_ZERO;
    CtlInitArgs.Size = sizeof(CtlInitArgs);
    CtlInitArgs.Version = 0;
    ZeroMemory(&CtlInitArgs.ApplicationUID, sizeof(ctl_application_id_t));
    try
    {
        Result = ctlInit(&CtlInitArgs, &hAPIHandle);
        LOG_AND_EXIT_ON_ERROR(Result, "ctlInit");
    }
    catch (const std::bad_array_new_length& e)
    {
        printf("%s \n", e.what());
    }

    if (CTL_RESULT_SUCCESS == Result)
    {
        // Initialization successful
        // Get the list of Intel Adapters

        try
        {
            Result = ctlEnumerateDevices(hAPIHandle, &Adapter_count, hDevices);
            LOG_AND_EXIT_ON_ERROR(Result, "ctlEnumerateDevices");
        }
        catch (const std::bad_array_new_length& e)
        {
            printf("%s \n", e.what());
        }

        if (CTL_RESULT_SUCCESS == Result)
        {
            hDevices = (ctl_device_adapter_handle_t*)malloc(sizeof(ctl_device_adapter_handle_t) * Adapter_count);
            if (hDevices == NULL)
            {
                return ERROR;
            }
            try
            {
                Result = ctlEnumerateDevices(hAPIHandle, &Adapter_count, hDevices);
                LOG_AND_EXIT_ON_ERROR(Result, "ctlEnumerateDevices");
            }
            catch (const std::bad_array_new_length& e)
            {
                printf("%s \n", e.what());
            }
        }
        if (CTL_RESULT_SUCCESS != Result)
        {
            printf("ctlEnumerateDevices returned failure code: 0x%X\n", Result);
            goto Exit;
        }

        for (Index = 0; Index < Adapter_count; Index++)
        {
            if (NULL != hDevices[Index])
            {
                LUID AdapterID;
                StDeviceAdapterProperties.Size = sizeof(ctl_device_adapter_properties_t);
                StDeviceAdapterProperties.pDeviceID = malloc(sizeof(LUID));
                StDeviceAdapterProperties.device_id_size = sizeof(LUID);
                StDeviceAdapterProperties.Version = 2;

                if (NULL == StDeviceAdapterProperties.pDeviceID)
                {
                    return ERROR;
                }

                Result = ctlGetDeviceProperties(hDevices[Index], &StDeviceAdapterProperties);

                if (Result != CTL_RESULT_SUCCESS)
                {
                    printf("ctlGetDeviceProperties returned failure code: 0x%X\n", Result);
                    break;
                }

                if (CTL_DEVICE_TYPE_GRAPHICS != StDeviceAdapterProperties.device_type)
                {
                    printf("This is not a Graphics device \n");

                    if (NULL != StDeviceAdapterProperties.pDeviceID)
                    {
                        free(StDeviceAdapterProperties.pDeviceID);
                    }
                    continue;
                }

                if (NULL != StDeviceAdapterProperties.pDeviceID)
                {
                    AdapterID = *(reinterpret_cast<LUID*>(StDeviceAdapterProperties.pDeviceID));
                    PRINT_LOGS("\nAdapter ID %lu \n", AdapterID.LowPart);
                }

                if (0x8086 == StDeviceAdapterProperties.pci_vendor_id)
                {
                    PRINT_LOGS("\nIntel Adapter Name %s", StDeviceAdapterProperties.name);
                    PRINT_LOGS("\nVendor id  0x%X", StDeviceAdapterProperties.pci_vendor_id);
                    PRINT_LOGS("\nDevice id 0x%X", StDeviceAdapterProperties.pci_device_id);
                    PRINT_LOGS("\nSubSys id 0x%X", StDeviceAdapterProperties.pci_subsys_id);
                    PRINT_LOGS("\nSubSys Vendor id 0x%X", StDeviceAdapterProperties.pci_subsys_vendor_id);
                    PRINT_LOGS("\nRev id 0x%X", StDeviceAdapterProperties.rev_id);
                }

                //// Per Component Tests
                //PerComponentTest(hDevices[Index]);

                //// Overclocking Test
                //CtlOverclockPropertiesTest(hDevices[Index]);

                //std::vector<double> vFrequencies;
                //vFrequencies.push_back(10.0);
                //vFrequencies.push_back(30.0);
                //vFrequencies.push_back(50.0);
                //vFrequencies.push_back(75.0);
                //vFrequencies.push_back(100.0);
                //vFrequencies.push_back(150.0);
                //vFrequencies.push_back(200.0);
                //vFrequencies.push_back(250.0);

                //for (uint32_t ci = 0; ci < vFrequencies.size(); ci++)
                //{
                //    CtlOverclockFrequencyOffsetTest(hDevices[Index], vFrequencies[ci]);
                //}

                //CtlOverclockFrequencyAndVoltageOffsetTest(hDevices[Index]);

                //CtlOverclockPowerTest(hDevices[Index]);
                //CtlOverclockTemperatureTest(hDevices[Index]);

                // Telemetry Test
                // Polling during 1 second at 20 ms
                //for (uint32_t i = 0; i < 50; i++)
                //{
                //    try
                //    {
                //        CtlPowerTelemetryTest(hDevices[Index]);
                //    }
                //    catch (const std::bad_array_new_length& e)
                //    {
                //        printf("%s \n", e.what());
                //    }
                //    Sleep(20);
                //}

                if (NULL != StDeviceAdapterProperties.pDeviceID)
                {
                    free(StDeviceAdapterProperties.pDeviceID);
                }
            }
        }
    }

Exit:

    ctlClose(hAPIHandle);

    if (hDevices != nullptr)
    {
        free(hDevices);
        hDevices = nullptr;
    }

    return 0;
}

IgclHandler::IgclHandler()
{
    qDebug() << __FUNCTION__;
}

IgclHandler::~IgclHandler()
{
    qDebug() << __FUNCTION__;
}

bool IgclHandler::init()
{

    return m_initialized;
}

void IgclHandler::readStaticInfo()
{
    ctlTest();
    checkSupportedDynamicFunctions();
}

void IgclHandler::checkSupportedDynamicFunctions()
{
    m_functionsSupportStatus["NvAPI_GPU_GetDynamicPstatesInfoEx"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetAllClockFrequencies"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetThermalSettings"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetTachReading"] = false;
    m_functionsSupportStatus["NvAPI_GPU_GetTachReading"] = false;
}


void IgclHandler::readDynamicInfo()
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

void IgclHandler::readGpuUsage()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetDynamicPstatesInfoEx"])
    {

    }
}

void IgclHandler::readGpuFrequencies()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetAllClockFrequencies"])
    {

    }
}

void IgclHandler::readGpuMemory()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetMemoryInfoEx"])
    {

    }
}

void IgclHandler::readGpuTemperature()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetThermalSettings"])
    {

    }
}

void IgclHandler::readGpuFanSpeed()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetTachReading"])
    {

    }
}


void IgclHandler::readGpuCooler()
{
    if (m_functionsSupportStatus["NvAPI_GPU_GetCoolerSettings"])
    {

    }
}