#pragma once

#include "Worker.h"
#include "ProcessInfo.h"

class ProcessWorker : public Worker
{
    Q_OBJECT
public:
    explicit ProcessWorker(int timerInterval, QObject* parent = nullptr);
    ~ProcessWorker();

public slots:
    virtual void start() override;
    virtual void stop() override;
    virtual void update() override;

    void slotProcessorCount(uint8_t newProcessorCount);

signals:
    void signalDynamicInfo(const std::map<uint32_t, ProcessInfo::Process>&);

private:
    std::unique_ptr<ProcessInfo> m_processInfo{nullptr};
};

