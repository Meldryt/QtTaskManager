#include "BaseInfo.h"

#include "../Globals.h"

#include <QDebug>

BaseInfo::BaseInfo()
{
    qDebug() << __FUNCTION__;

    for (uint8_t key = Globals::Key_Start+1; key < Globals::Key_End; ++key)
    {
        m_staticInfo[key] = Globals::SysInfo_Uninitialized;
    }
    for (uint8_t key = Globals::Key_Start+1; key < Globals::Key_End; ++key)
    {
        m_dynamicInfo[key] = Globals::SysInfo_Uninitialized;
    }
}

BaseInfo::~BaseInfo()
{
    qDebug() << __FUNCTION__;
}

const QMap<uint8_t, QVariant>& BaseInfo::staticInfo() const
{
    return m_staticInfo;
}

const QMap<uint8_t, QVariant>& BaseInfo::dynamicInfo() const
{
    return m_dynamicInfo;
}

void BaseInfo::setStaticInfo(const uint8_t key, const QVariant value)
{
    m_staticInfo[key] = value;
}

void BaseInfo::setDynamicInfo(const uint8_t key, const QVariant value)
{
    m_dynamicInfo[key] = value;
}
