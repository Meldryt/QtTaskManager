#pragma once

#include "Worker.h"

#include "Globals.h"

#include <QMap>
#include <QVariant>
#include <QElapsedTimer>

class WmiInfo;

class WmiWorker : public Worker
{
    Q_OBJECT
public:
    explicit WmiWorker(int timerInterval, QObject *parent = nullptr);
    ~WmiWorker();

public slots:
    virtual void start() override;
    virtual void stop() override;
    virtual void update() override;

signals:
    void signalStaticInfo(const QMap<uint8_t, QVariant>&);
    void signalDynamicInfo(const QMap<uint8_t, QVariant>&);

private:
    std::unique_ptr<WmiInfo> m_wmiInfo{nullptr};
    QElapsedTimer* m_elapsedTimer{ nullptr };
};