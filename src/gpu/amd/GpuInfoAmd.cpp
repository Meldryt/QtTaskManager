#include "GpuInfoAmd.h"

#include "AdlxHandler.h"
#include "AgsHandler.h"
#include "../../Globals.h"

#include <QDebug>

GpuInfoAmd::GpuInfoAmd()
{
    qDebug() << __FUNCTION__;

    m_adlxHandler = new AdlxHandler();
    m_agsHandler = new AgsHandler();
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
    if(!m_adlxHandler)
    {
        return {};
    }

    return m_adlxHandler->dynamicInfo();
}

bool GpuInfoAmd::init()
{
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

    return true;
}

void GpuInfoAmd::readStaticInfo()
{
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

    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_DriverInfo] = QString::fromStdString(m_agsHandler->driverVersion());
    m_staticInfo[Globals::SysInfoAttr::Key_Gpu_DriverVersion] = QString::fromStdString(m_agsHandler->softwareVersion());
}

void GpuInfoAmd::readDynamicInfo()
{
    if(!m_adlxHandler)
    {
        return;
    }

    m_adlxHandler->readDynamicInfo();
}