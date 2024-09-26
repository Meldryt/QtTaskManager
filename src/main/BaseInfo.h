#pragma once

#include <QMap>
#include <QVariant>

class BaseInfo
{
public:
    enum class InfoType : uint8_t
    {
        None = 0,
        Cpu,
        Gpu,
        Memory,
        Network,
        Process,
        System,
        Wmi
    };

    BaseInfo(const std::string className="", const InfoType infoType = InfoType::None);
    ~BaseInfo();

    BaseInfo(const BaseInfo&) = delete;
    BaseInfo& operator =(const BaseInfo&) = delete;

    virtual void init()=0;
    virtual void update()=0;

    const std::string& name() const;
    const InfoType& infoType() const;
    const QMap<uint8_t, QVariant>& staticInfo() const;
    const QMap<uint8_t, QVariant>& dynamicInfo() const;

    void setStaticInfo(const QMap<uint8_t, QVariant>& staticInfo);
    void setDynamicInfo(const QMap<uint8_t, QVariant>& dynamicInfo);
    void setStaticValue(const uint8_t key, const QVariant value);
    void setDynamicValue(const uint8_t key, const QVariant value);

private:
    std::string m_name{"BaseInfo"};
    InfoType m_infoType{ InfoType::None };
    QMap<uint8_t, QVariant> m_staticInfo;
    QMap<uint8_t, QVariant> m_dynamicInfo;
};
