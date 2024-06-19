#pragma once

#include "Worker.h"
#include "Globals.h"

class GpuInfo;

class GpuWorker : public Worker
{
    Q_OBJECT
public:
    explicit GpuWorker(int timerInterval, QObject* parent = nullptr);

public slots:
    virtual void start() override;
    virtual void stop() override;
    virtual void update() override;

signals:
    void signalStaticInfo(const Globals::GpuStaticInfo&);
    void signalDynamicInfo(const Globals::GpuDynamicInfo&);

private:
    std::unique_ptr<GpuInfo> m_gpuInfo{nullptr};
};

