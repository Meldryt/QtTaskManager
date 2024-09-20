#include "SystemInfoWindows.h"

//#include "../Utils.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <QDebug>

#ifdef _WIN32
typedef LONG NTSTATUS, * PNTSTATUS;
#define STATUS_SUCCESS (0x00000000)

typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

RTL_OSVERSIONINFOW GetRealOSVersion() {
    HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
    if (hMod) {
        RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
        if (fxPtr != nullptr) {
            RTL_OSVERSIONINFOW rovi = { 0 };
            rovi.dwOSVersionInfoSize = sizeof(rovi);
            if (STATUS_SUCCESS == fxPtr(&rovi)) {
                return rovi;
            }
        }
    }
    RTL_OSVERSIONINFOW rovi = { 0 };
    return rovi;
}

#endif

SystemInfoWindows::SystemInfoWindows()
{
    qDebug() << __FUNCTION__;

    m_osName = "Windows";
}

SystemInfoWindows::~SystemInfoWindows()
{
    qDebug() << __FUNCTION__;
}


bool SystemInfoWindows::init()
{
    return true;
}

void SystemInfoWindows::readStaticInfo()
{
#ifdef _WIN32
    RTL_OSVERSIONINFOW osInfo = GetRealOSVersion();

    const std::string strVersion = std::to_string(osInfo.dwMajorVersion) + "." + std::to_string(osInfo.dwMinorVersion);
    const std::string strBuildNumber = std::to_string(osInfo.dwBuildNumber);

    m_osName = "Windows " + strVersion;
    m_osVersion = strBuildNumber;
#endif
}

void SystemInfoWindows::readDynamicInfo()
{

}
