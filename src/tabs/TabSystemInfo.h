#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QMap>

class TabSystemInfo : public QWidget
{
public:
    explicit TabSystemInfo(QWidget *parent = nullptr);
    ~TabSystemInfo();

    void process();

private:

    const QStringList SystemParameterNames
    {
        "OS Name",
        "OS Code Name",
        "OS Version",
    };

    QTableWidget* m_tableWidget{nullptr};

    QTimer* m_timer{nullptr};


public slots:
    void slotOSInfo(const QMap<uint8_t,QVariant>& staticInfo);
};
