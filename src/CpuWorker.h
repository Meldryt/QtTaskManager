#pragma once

#include "Worker.h"

#include "Globals.h"

#include <QElapsedTimer>

class CpuInfo;

class CpuWorker : public Worker
{
    Q_OBJECT
public:
    explicit CpuWorker(int timerInterval, QObject *parent = nullptr);

public slots:
    virtual void start() override;
    virtual void stop() override;
    virtual void update() override;

signals:
    void signalStaticInfo(const QMap<uint8_t,QVariant>&);
    void signalDynamicInfo(const QMap<uint8_t,QVariant>&);

private:
    std::unique_ptr<CpuInfo> m_cpuInfo{nullptr};
    QElapsedTimer* m_elapsedTimer{ nullptr };

};

