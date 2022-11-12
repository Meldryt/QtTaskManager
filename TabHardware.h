#pragma once

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QTableWidget>
#include <QTimer>

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
    QLabel* cpuInfoLabel{nullptr};

    QWidget* gpuWidget{nullptr};
    QLabel* gpuInfoLabel{nullptr};

    QWidget* ramWidget{nullptr};
    QLabel* ramInfoLabel{nullptr};

    QWidget* devicesWidget{nullptr};
    QTableWidget* devicesInfoTable{nullptr};

    QTimer* timer{nullptr};

    QString cpuBrand{""};
    QString processorCount{""};
    QString threadCount{""};
    QString l1CacheSize{""};
    QString l2CacheSize{""};
    QString l3CacheSize{""};
    QString gpuBrand{""};
    QString totalPhysicalMemory{""};

public slots:
    void slotCPUBrand(const std::string& val);
    void slotProcessorCount(const uint8_t& val);  
    void slotThreadCount(const uint8_t& val);
    void slotL1CacheSize(const uint32_t& val);
    void slotL2CacheSize(const uint32_t& val);
    void slotL3CacheSize(const uint32_t& val);

    void slotGPUBrand(const std::string& val);
    void slotTotalPhysicalMemory(const uint32_t& val);

private slots:
    void showSelectionWidget();
};
