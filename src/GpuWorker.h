#pragma once

#include "Worker.h"
#include "Globals.h"

class GpuInfo;

class GpuWorker : public Worker
{
    Q_OBJECT
public:
    explicit GpuWorker(int timerInterval, QObject* parent = nullptr);
    ~GpuWorker();

public slots:
    virtual void start() override;
    virtual void stop() override;
    virtual void update() override;

signals:
    void signalStaticInfo(const QMap<uint8_t,QVariant>&);
    void signalDynamicInfo(const QMap<uint8_t,QVariant>&);

private:
    std::unique_ptr<GpuInfo> m_gpuInfo{nullptr};
};

