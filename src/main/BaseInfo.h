#pragma once

#include <QMap>
#include <QVariant>

class BaseInfo
{
public:

    BaseInfo(const std::string className="");
    ~BaseInfo();

    virtual void init()=0;
    virtual void update()=0;

    const std::string& name() const;
    const QMap<uint8_t, QVariant>& staticInfo() const;
    const QMap<uint8_t, QVariant>& dynamicInfo() const;

    void setStaticInfo(const QMap<uint8_t, QVariant>& staticInfo);
    void setDynamicInfo(const QMap<uint8_t, QVariant>& dynamicInfo);
    void setStaticValue(const uint8_t key, const QVariant value);
    void setDynamicValue(const uint8_t key, const QVariant value);

private:
    std::string m_name{"BaseInfo"};
    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;
};
