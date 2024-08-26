#pragma once

#include <QObject>
#include <QMap>
#include <QVariant>
#include <QTimer>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(int timerInterval, QObject *parent = nullptr);
    virtual ~Worker();

public slots:
    virtual void start();
    virtual void stop();
    virtual void update() = 0;

signals:
    void signalStarted();
    void signalStopped();
    void signalFinished();

    void signalStaticInfo(const QMap<uint8_t,QVariant>&);
    void signalDynamicInfo(const QMap<uint8_t,QVariant>&);

private:
    QTimer* m_timer{nullptr};
};

