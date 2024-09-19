#pragma once

#include "../Worker.h"

class NetworkInfo;
 
class NetworkWorker : public Worker
{
    Q_OBJECT
public:
    explicit NetworkWorker(int timerInterval, QObject* parent = nullptr);
    ~NetworkWorker();

public slots:
    virtual void start() override;
    virtual void stop() override;
    virtual void update() override;

signals:
    void signalStaticInfo(const QMap<uint8_t, QVariant>&);
    void signalDynamicInfo(const QMap<uint8_t,QVariant>&);

private:
    std::unique_ptr<NetworkInfo> m_networkInfo{nullptr};
};

