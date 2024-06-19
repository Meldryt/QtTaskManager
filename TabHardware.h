#pragma once

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QTableWidget>
#include <QTimer>

#include <Globals.h>

class TabHardware : public QWidget
{
public:
    explicit TabHardware(QWidget *parent = nullptr);

private:
    void process();
    void processCPU();
    void processGPU();
    void processRAM();
    void processDevices();

    QListWidget* listWidget{nullptr};

    QWidget* cpuWidget{nullptr};
    QLabel* CpuInfoLabel{nullptr};

    QWidget* gpuWidget{nullptr};
    QLabel* m_gpuInfoLabel{nullptr};

    QWidget* ramWidget{nullptr};
    QLabel* ramInfoLabel{nullptr};

    QWidget* devicesWidget{nullptr};
    QTableWidget* devicesInfoTable{nullptr};

    QTimer* m_timer{nullptr};

    QString m_gpuStaticInfoText;

    QString m_cpuBrand{""};
    QString m_processorCount{""};
    QString m_threadCount{""};
    QString m_l1CacheSize{""};
    QString m_l2CacheSize{""};
    QString m_l3CacheSize{""};
    QString m_totalPhysicalMemory{""};

public slots:
    void slotCpuStaticInfo(const Globals::CpuStaticInfo& staticInfo);
    void slotGpuStaticInfo(const Globals::GpuStaticInfo& staticInfo);

    void slotTotalPhysicalMemory(const uint32_t& val);

private slots:
    void showSelectionWidget();
};
