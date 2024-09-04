#include "CpuInfoIntel.h"

#include "../../Globals.h"

#ifdef __linux__
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#endif

#include <QDebug>

CpuInfoIntel::CpuInfoIntel()
{
    qDebug() << __FUNCTION__;
}

CpuInfoIntel::~CpuInfoIntel()
{
    qDebug() << __FUNCTION__;
}

const QMap<uint8_t, QVariant>& CpuInfoIntel::staticInfo() const
{
    return m_staticInfo;
}

const QMap<uint8_t, QVariant>& CpuInfoIntel::dynamicInfo() const
{
    return m_dynamicInfo;
}

void CpuInfoIntel::init()
{
    for (uint8_t i = Globals::Key_Cpu_Static_Start + 1; i < Globals::Key_Cpu_Static_End; ++i)
    {
        m_staticInfo[i] = Globals::SysInfo_Uninitialized;
    }

    for (uint8_t i = Globals::Key_Cpu_Dynamic_Start + 1; i < Globals::Key_Cpu_Dynamic_End; ++i)
    {
        m_dynamicInfo[i] = Globals::SysInfo_Uninitialized;
    }
}

void CpuInfoIntel::readStaticInfo()
{
}

void CpuInfoIntel::readDynamicInfo()
{
}
