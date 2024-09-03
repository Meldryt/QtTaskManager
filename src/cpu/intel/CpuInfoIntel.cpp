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
}

void CpuInfoIntel::readStaticInfo()
{
}

void CpuInfoIntel::readDynamicInfo()
{
}