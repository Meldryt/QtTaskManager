#pragma once

#include "Worker.h"

#include "Globals.h"

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
    void signalStaticInfo(const Globals::CpuStaticInfo&);
    void signalDynamicInfo(const Globals::CpuDynamicInfo&);

private:
    std::unique_ptr<CpuInfo> m_cpuInfo{nullptr};
};

