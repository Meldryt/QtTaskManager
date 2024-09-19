#pragma once

#include <QMap>
#include <QVariant>

class BaseInfo
{
public:

    BaseInfo();
    ~BaseInfo();

    virtual void init()=0;
    virtual void update()=0;

    const QMap<uint8_t, QVariant>& staticInfo() const;
    const QMap<uint8_t, QVariant>& dynamicInfo() const;

    void setStaticInfo(const uint8_t key, const QVariant value);
    void setDynamicInfo(const uint8_t key, const QVariant value);

private:
    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;
};
