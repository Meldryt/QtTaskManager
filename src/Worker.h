#pragma once

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QTimer>

#include "main/BaseInfo.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(const int timerInterval, std::initializer_list<BaseInfo::InfoType> infoTypes, QObject* parent=nullptr);
    ~Worker();

public slots:
    void start();
    void stop();
    void update();

signals:
    void signalStarted();
    void signalStopped();
    void signalFinished();

    void signalStaticInfo(const BaseInfo::InfoType& infoType, const QMap<uint8_t,QVariant>&);
    void signalDynamicInfo(const BaseInfo::InfoType& infoType, const QMap<uint8_t,QVariant>&);

private:
    void createInfoObjects(std::initializer_list<BaseInfo::InfoType>& infoTypes);

    QTimer* m_timer{nullptr};
    std::vector<std::unique_ptr<BaseInfo>> m_infoObjects;
};

