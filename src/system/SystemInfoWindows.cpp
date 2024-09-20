#include "SystemInfoWindows.h"

//#include "../Utils.h"

#include <QDebug>

SystemInfoWindows::SystemInfoWindows()
{
    qDebug() << __FUNCTION__;
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
    //const std::string strPrettyName = "PRETTY_NAME=";
    //m_osName = Utils::exec("cat /etc/os-release | grep " + strPrettyName);
    //if(!m_osName.empty())
    //{
    //    m_osName.erase(std::remove(m_osName.begin(), m_osName.end(), '"'), m_osName.end());
    //    m_osName.erase(m_osName.find(strPrettyName), strPrettyName.length());
    //}
//
    //const std::string strVersion = "VERSION=";
    //m_osVersion = Utils::exec("cat /etc/os-release | grep " + strVersion);
    //if(!m_osVersion.empty())
    //{
    //    m_osVersion.erase(std::remove(m_osVersion.begin(), m_osVersion.end(), '"'), m_osVersion.end());
    //    m_osVersion.erase(m_osVersion.find(strVersion), strVersion.length());
    //    m_osCodeName = m_osVersion;
    //    const size_t pos = m_osCodeName.find_first_of("(");
    //    m_osCodeName = m_osCodeName.substr(pos+1, (m_osCodeName.find_last_of(")") - pos)-1);
    //    m_osVersion = m_osVersion.substr(0, pos-1);
    //}
//
    //qDebug() << __FUNCTION__ << " m_osName:" << m_osName << " m_osVersion:" << m_osVersion << " m_osCodeName:" << m_osCodeName;
}//

void SystemInfoWindows::readDynamicInfo()
{

}
