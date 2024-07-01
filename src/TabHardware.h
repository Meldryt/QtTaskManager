#pragma once

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QTableWidget>

#include "Globals.h"

class TabHardware : public QWidget
{
public:
    explicit TabHardware(QWidget *parent = nullptr);

    void process();

private:
    void processCpu();
    void processGpu();
    void processMemory();
    void processDevices();

    QListWidget* m_listWidget{nullptr};

    QTableWidget* m_cpuTableWidget{ nullptr };
    QTableWidget* m_gpuTableWidget{ nullptr };
    QTableWidget* m_memoryTableWidget{ nullptr };
    QTableWidget* m_devicesTableWidget{nullptr};

    QTimer* m_timer{nullptr};

    const QStringList CpuParameterNames
    {
        "Cpu Brand",
        "Thread Count",
        "Processor Count",
        "L1Cache Size",
        "L2Cache Size",
        "L3Cache Size",
        "Base Frequency",
        "Turbo Frequency",
    };
    std::map<int, QString> m_cpuTableInfos;

    const QStringList GpuParameterNames
    {
        "Graphics Chip Designer",
        "Graphics Card Manufacturer",
        "Graphics Card Model",
        "Graphics Memory Vendor",
        "Graphics Memory Size",
        "Graphics Memory Type",
        "Graphics Memory Bandwidth",
        "Graphics Driver",
        "Graphics Driver Version"
    };
    std::map<int, QString> m_gpuTableInfos;

    const QStringList MemoryParameterNames
    {
        "Physical Memory",
        "RAM Brand"
    };
    std::map<int, QString> m_memoryTableInfos;

    const QStringList DevicesParameterNames
    {
        "Mainboard",
        "Mouse",
        "Keyboard",
        "Soundcard"
    };
    std::map<int, QString> m_devicesTableInfos;

public slots:
    void slotCpuStaticInfo(const Globals::CpuStaticInfo& staticInfo);
    void slotGpuStaticInfo(const Globals::GpuStaticInfo& staticInfo);

    void slotTotalPhysicalMemory(const uint32_t& val);

private slots:
    void showSelectionWidget();
};
