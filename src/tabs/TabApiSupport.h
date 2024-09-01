#pragma once

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QTableWidget>
#include <QMap>

class TabApiSupport : public QWidget
{
public:
    explicit TabApiSupport(QWidget *parent = nullptr);
    ~TabApiSupport();

    void process();

private:

    QListWidget* m_listWidget{nullptr};

    QMap<uint8_t, QTableWidget*> m_apiFunctionsWidgets;

    QTimer* m_timer{nullptr};

    enum ApiType
    {
        ApiType_Adlx=0,
        ApiType_Nvapi,
        ApiType_Nvml,
        ApiType_RyzenMasterSDK,
        ApiType_Wmi,
        ApiType_End
    };

public slots:
    void slotApiSupportStaticInfo(const QMap<uint8_t,QVariant>& staticInfo);

private slots:
    void showSelectionWidget();
};
