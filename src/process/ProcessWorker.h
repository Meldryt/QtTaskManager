#pragma once

#include "../Worker.h"

#include "ProcessInfo.h"

#include <map>

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

    void slotCoreCount(uint16_t newCoreCount);

signals:
    void signalDynamicInfo(const std::map<uint32_t, ProcessInfo::Process>&);

private:
    std::unique_ptr<ProcessInfo> m_processInfo{nullptr};
};

