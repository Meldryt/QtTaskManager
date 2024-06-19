#pragma once

#include <QObject>
#include <QTimer>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(int timerInterval, QObject *parent = nullptr);

public slots:
    virtual void start();
    virtual void stop();
    virtual void update() = 0;

signals:
    void started();
    void stopped();
    void finished();

protected:
    bool m_finished{ true };

private:
    QTimer* m_timer{nullptr};

};

