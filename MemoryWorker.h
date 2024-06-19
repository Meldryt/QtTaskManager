#pragma once

#include "Worker.h"

#include "Globals.h"

class MemoryInfo;
 
class MemoryWorker : public Worker
{
    Q_OBJECT
public:
    explicit MemoryWorker(int timerInterval, QObject* parent = nullptr);

public slots:
    virtual void start() override;
    virtual void stop() override;
    virtual void update() override;

signals:
    void signalStaticInfo(const Globals::MemoryStaticInfo&);
    void signalDynamicInfo(const Globals::MemoryDynamicInfo&);

private:
    std::unique_ptr<MemoryInfo> m_memoryInfo{nullptr};
};

