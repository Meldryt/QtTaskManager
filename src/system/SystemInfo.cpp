#include "SystemInfo.h"

#include "../Globals.h"

#ifdef _WIN32
#include "SystemInfoWindows.h"
#elif __linux__
#include "SystemInfoLinux.h"
#endif

SystemInfo::SystemInfo() : BaseInfo("SystemInfo", InfoType::System)
{
    qDebug() << __FUNCTION__;
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
    if(m_systemInfoWindows)
    {
        m_systemInfoWindows->readStaticInfo();
        setStaticValue(Globals::SysInfoAttr::Key_SysInfo_OS_Name,m_systemInfoWindows->osName().c_str());
        setStaticValue(Globals::SysInfoAttr::Key_SysInfo_OS_CodeName,m_systemInfoWindows->osCodeName().c_str());
        setStaticValue(Globals::SysInfoAttr::Key_SysInfo_OS_Version,m_systemInfoWindows->osVersion().c_str());
    }
#elif __linux__
    if(m_systemInfoLinux)
    {
        m_systemInfoLinux->readStaticInfo();
        setStaticValue(Globals::SysInfoAttr::Key_SysInfo_OS_Name,QString::fromStdString(m_systemInfoLinux->osName()));
        setStaticValue(Globals::SysInfoAttr::Key_SysInfo_OS_CodeName,QString::fromStdString(m_systemInfoLinux->osCodeName()));
        setStaticValue(Globals::SysInfoAttr::Key_SysInfo_OS_Version,QString::fromStdString(m_systemInfoLinux->osVersion()));
    }
#endif
}

void SystemInfo::readDynamicInfo()
{

}
