#pragma once

#include "../Worker.h"

class SystemInfo;
 
class SystemWorker : public Worker
{
    Q_OBJECT
public:
    explicit SystemWorker(int timerInterval, QObject* parent = nullptr);
    ~SystemWorker();

public slots:
    virtual void start() override;
    virtual void stop() override;
    virtual void update() override;

signals:
    void signalStaticInfo(const QMap<uint8_t, QVariant>&);
    void signalDynamicInfo(const QMap<uint8_t,QVariant>&);

private:
    std::unique_ptr<SystemInfo> m_systemInfo{nullptr};
};

