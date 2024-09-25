#pragma once

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QTimer>

//class ProcessInfo;
//class CpuInfo;
//class GpuInfo;
//class NetworkInfo;
//class MemoryInfo;
class BaseInfo;

class Worker : public QObject
{
    Q_OBJECT
public:
    enum InfoType : uint8_t
    {
        Cpu,
        Gpu,
        Memory,
        Network,
        Process,
        System,
        Wmi
    };

    explicit Worker(const int timerInterval, const InfoType type, QObject* parent=nullptr);
    ~Worker();

public slots:
    void start();
    void stop();
    void update();

signals:
    void signalStarted();
    void signalStopped();
    void signalFinished();

    void signalStaticInfo(const QMap<uint8_t,QVariant>&);
    void signalDynamicInfo(const QMap<uint8_t,QVariant>&);

private:
    QTimer* m_timer{nullptr};
    InfoType m_infoType{Cpu};
    std::unique_ptr<BaseInfo> m_baseInfo{nullptr};
};

