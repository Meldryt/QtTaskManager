#include "SystemInfo.h"

#include "../Globals.h"

#ifdef _WIN32
#include "SystemInfoWindows.h"
#elif __linux__
#include "SystemInfoLinux.h"
#endif

SystemInfo::SystemInfo()
{
    qDebug() << __FUNCTION__;

    for (uint8_t key = Globals::Key_Cpu_Static_Start + 1; key < Globals::Key_Cpu_Static_End; ++key)
    {
        m_staticInfo[key] = Globals::SysInfo_Uninitialized;
    }
    for (uint8_t key = Globals::Key_Cpu_Dynamic_Start + 1; key < Globals::Key_Cpu_Dynamic_End; ++key)
    {
        m_dynamicInfo[key] = Globals::SysInfo_Uninitialized;
    }
}

SystemInfo::~SystemInfo()
{
    qDebug() << __FUNCTION__;
}

void SystemInfo::init()
{
#ifdef _WIN32
    m_systemInfoWindows = new SystemInfoWindows();
    m_systemInfoWindows->init();
#elif __linux__
    m_systemInfoLinux = new SystemInfoLinux();
    m_systemInfoLinux->init();
#endif

    readStaticInfo();
}

void SystemInfo::update()
{
    readDynamicInfo();
}

void SystemInfo::readStaticInfo()
{
#ifdef _WIN32
    if(m_cpuInfoWindows)
    {
        m_systemInfoWindows->readStaticInfo();
        m_staticInfo[Globals::SysInfoAttr::Key_SysInfo_OS_Name] = m_systemInfoWindows->osName();
        m_staticInfo[Globals::SysInfoAttr::Key_SysInfo_OS_CodeName] = m_systemInfoWindows->osCodeName();
        m_staticInfo[Globals::SysInfoAttr::Key_SysInfo_OS_Version] = m_systemInfoWindows->osVersion();
    }
#elif __linux__
    if(m_systemInfoLinux)
    {
        m_systemInfoLinux->readStaticInfo();
        m_staticInfo[Globals::SysInfoAttr::Key_SysInfo_OS_Name] = QString::fromStdString(m_systemInfoLinux->osName());
        m_staticInfo[Globals::SysInfoAttr::Key_SysInfo_OS_CodeName] = QString::fromStdString(m_systemInfoLinux->osCodeName());
        m_staticInfo[Globals::SysInfoAttr::Key_SysInfo_OS_Version] = QString::fromStdString(m_systemInfoLinux->osVersion());
    }
#endif
}

void SystemInfo::readDynamicInfo()
{

}
