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
    void signalStaticInfo(const QMap<uint8_t, QVariant>&);
    void signalDynamicInfo(const QMap<uint8_t,QVariant>&);

private:
    std::unique_ptr<MemoryInfo> m_memoryInfo{nullptr};
};

