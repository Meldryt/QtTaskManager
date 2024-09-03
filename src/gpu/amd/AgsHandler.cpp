#include "AgsHandler.h"

#include "../../Globals.h"

#ifdef _WIN32
#include <windows.h>
#include <amd_ags.h>
#endif

#include <QDebug>

//#pragma comment(lib, "amd_ags_x64.lib")
//#pragma comment(lib, "amd_ags_x64_2022_MDd.lib")

AgsHandler::AgsHandler()
{
    qDebug() << __FUNCTION__;
}

AgsHandler::~AgsHandler()
{
    qDebug() << __FUNCTION__;
}

bool AgsHandler::init()
{
#ifdef _WIN32
    AGSContext* agsContext = nullptr;
    AGSGPUInfo gpuInfo = {};
    AGSConfiguration config = {};

    if (agsInitialize(AGS_CURRENT_VERSION, &config, &agsContext, &gpuInfo) == AGS_SUCCESS)
    {
        qDebug() << "AGS Library initialized: " << AMD_AGS_VERSION_MAJOR << "." << AMD_AGS_VERSION_MINOR << "." << AMD_AGS_VERSION_PATCH;
        qDebug() << "Radeon Software Version: " << gpuInfo.radeonSoftwareVersion;
        qDebug() << "Driver Version:          " << gpuInfo.driverVersion;

        m_agsVersion = std::to_string(AMD_AGS_VERSION_MAJOR) + std::string(".") + std::to_string(AMD_AGS_VERSION_MINOR) + std::string(".") + std::to_string(AMD_AGS_VERSION_PATCH);
        m_driverVersion = std::string(gpuInfo.driverVersion);
        m_softwareVersion = std::string(gpuInfo.radeonSoftwareVersion);

        if (agsDeInitialize(agsContext) != AGS_SUCCESS)
        {
            qWarning() << "Failed to cleanup AGS Library";
        }

        m_initialized = true;
    }
#endif

    return m_initialized;
}