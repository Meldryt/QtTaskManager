#include "GpuInfoAmd.h"

#ifdef _WIN32
#include "AdlxHandler.h"
#include "AgsHandler.h"
#endif

#include "../../Globals.h"

#include <QDebug>

GpuInfoAmd::GpuInfoAmd()
{
    qDebug() << __FUNCTION__;
#ifdef _WIN32
    m_adlxHandler = new AdlxHandler();
    m_agsHandler = new AgsHandler();
#endif

    for (uint8_t key = Globals::Key_Gpu_Static_Start + 1; key < Globals::Key_Gpu_Static_End; ++key)
    {
        m_staticInfo[key] = Globals::SysInfo_Uninitialized;
    }
    //for (uint8_t key = Globals::Key_Gpu_Dynamic_Start + 1; key < Globals::Key_Gpu_Dynamic_End; ++key)
    //{
    //    m_dynamicInfo[key] = Globals::SysInfo_Uninitialized;
    //}
}

GpuInfoAmd::~GpuInfoAmd()
{
    qDebug() << __FUNCTION__;
}

const QMap<uint8_t, QVariant>& GpuInfoAmd::staticInfo() const
{
    return m_staticInfo;
}

const QMap<uint8_t, QVariant>& GpuInfoAmd::dynamicInfo() const
{
#ifdef _WIN32
    if(!m_adlxHandler)
    {
        return {};
    }

    return m_adlxHandler->dynamicInfo();
#else
    return QMap<uint8_t, QVariant>();
#endif
}

bool GpuInfoAmd::init()
{
    for (uint8_t i = Globals::Key_Gpu_Static_Start + 1; i < Globals::Key_Gpu_Static_End; ++i)
    {
        m_staticInfo[i] = Globals::SysInfo_Uninitialized;
    }

//    for (uint8_t i = Globals::Key_Gpu_Dynamic_Start + 1; i < Globals::Key_Gpu_Dynamic_End; ++i)
//    {
//        m_dynamicInfo[i] = Globals::SysInfo_Uninitialized;
//    }

#ifdef _WIN32
    if(!m_adlxHandler)
    {
        return false;
    }

    m_adlxHandler->init();

    if(!m_agsHandler)
    {
        return false;
    }

    m_agsHandler->init();
#endif
    return true;
}

void GpuInfoAmd::readStaticInfo()
{
#ifdef _WIN32
    if(!m_adlxHandler)
    {
        return;
    }

    m_adlxHandler->readStaticInfo();
    m_staticInfo = m_adlxHandler->staticInfo();

    if(!m_agsHandler)
    {
        return;
    }

    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_DriverInfo] = QString::fromStdString(m_agsHandler->driverVersion());
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_Static_DriverVersion] = QString::fromStdString(m_agsHandler->softwareVersion());
#endif
}

void GpuInfoAmd::readDynamicInfo()
{
#ifdef _WIN32
    if(!m_adlxHandler)
    {
        return;
    }

    m_adlxHandler->readDynamicInfo();
#endif
}
