#include "TabHardware.h"

#include <QGridLayout>
#include <QPainter>
#include <QPoint>

TabHardware::TabHardware(QWidget *parent) : QWidget{parent}
{
    listWidget = new QListWidget(this);
    listWidget->addItem("CPU");
    listWidget->addItem("GPU");
    listWidget->addItem("RAM");
    listWidget->addItem("Devices");
    //listWidget->addItem("Disk");
    //listWidget->addItem("Network");
    listWidget->setFixedWidth(100);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(listWidget,0,0);
    setLayout(layout);

    {
        cpuWidget = new QWidget(this);
        CpuInfoLabel= new QLabel(cpuWidget);

        QGridLayout *layout = new QGridLayout(cpuWidget);
        layout->addWidget(CpuInfoLabel,0,0);
        cpuWidget->setLayout(layout);
    }

    {
        gpuWidget = new QWidget(this);
        m_gpuInfoLabel= new QLabel(gpuWidget);

        QGridLayout *layout = new QGridLayout(gpuWidget);
        layout->addWidget(m_gpuInfoLabel,0,0);
        gpuWidget->setLayout(layout);
    }

    {
        ramWidget = new QWidget(this);
        ramInfoLabel= new QLabel(ramWidget);

        QGridLayout *layout = new QGridLayout(ramWidget);
        layout->addWidget(ramInfoLabel,0,0);
        ramWidget->setLayout(layout);
    }

    {
        devicesWidget = new QWidget(this);
        devicesInfoTable= new QTableWidget(devicesWidget);

        QGridLayout *layout = new QGridLayout(devicesWidget);
        layout->addWidget(devicesInfoTable,0,0);
        devicesWidget->setLayout(layout);
    }

    layout->addWidget(cpuWidget,0,1);
    layout->addWidget(gpuWidget,0,2);
    layout->addWidget(ramWidget,0,3);
    layout->addWidget(devicesWidget,0,4);

    connect(listWidget,&QListWidget::itemSelectionChanged, this, &TabHardware::showSelectionWidget);
    listWidget->setCurrentRow(0);

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &TabHardware::process);
    m_timer->start();
}

void TabHardware::process()
{
    processCPU();
    processGPU();
    processRAM();
    processDevices();
}

void TabHardware::processCPU()
{
    QString text;
    text += "CPU Brand : " + m_cpuBrand + "\n";
    text += "Number of Cores: " + m_processorCount + "\n";
    text += "Number of Threads: " + m_threadCount + "\n";
    text += "L1 Cache Size: " + m_l1CacheSize + " KB" + "\n";
    text += "L2 Cache Size: " + m_l2CacheSize + " KB" + "\n";
    text += "L3 Cache Size: " + m_l3CacheSize + " KB" + "\n";
    CpuInfoLabel->setText(text);
}

void TabHardware::processGPU()
{
    m_gpuInfoLabel->setText(m_gpuStaticInfoText);
}

void TabHardware::processRAM()
{
    QString text;
    text += "RAM Brand : " + QString("") + "\n";
    text += "Total Physical Memory: " + m_totalPhysicalMemory + "MB \n";
    ramInfoLabel->setText(text);
}

void TabHardware::processDevices()
{

}

void TabHardware::slotCpuStaticInfo(const Globals::CpuStaticInfo& staticInfo)
{
    m_cpuBrand = staticInfo.cpuBrand.c_str();
    m_processorCount = QString::number(staticInfo.processorCount);
    m_threadCount = QString::number(staticInfo.threadCount);
    m_l1CacheSize = QString::number(staticInfo.l1CacheSize);
    m_l2CacheSize = QString::number(staticInfo.l2CacheSize);
    m_l3CacheSize = QString::number(staticInfo.l3CacheSize);
}

void TabHardware::slotGpuStaticInfo(const Globals::GpuStaticInfo& staticInfo)
{
    std::string text;
    text += "Graphics Chip Designer : " + staticInfo.chipDesigner +"\n";
    text += "Graphics Card Manufacturer : " + staticInfo.cardManufacturer + "\n";
    text += "Graphics Card Model : " + staticInfo.gpuModel + "\n";
    text += "Graphics Memory Vendor : " + staticInfo.memoryVendor + "\n";
    text += "Graphics Memory Size : " + std::to_string(staticInfo.memorySize) + "\n";
    text += "Graphics Memory Type : " + staticInfo.memoryType + "\n";
    text += "Graphics Memory Bandwidth : " + std::to_string(staticInfo.memoryBandwidth) + "\n";
    text += "Graphics Driver : " + staticInfo.driverInfo + "\n";
    text += "Graphics Driver Version : " + staticInfo.driverVersion + "\n";

    m_gpuStaticInfoText = QString(text.c_str());
}

void TabHardware::slotTotalPhysicalMemory(const uint32_t& val)
{
    m_totalPhysicalMemory = QString::number(val);
}

void TabHardware::showSelectionWidget()
{
    cpuWidget->hide();
    gpuWidget->hide();
    ramWidget->hide();
    devicesWidget->hide();

    switch(listWidget->currentRow())
    {
    case 0:
        cpuWidget->show();
        break;
    case 1:
        gpuWidget->show();
        break;
    case 2:
        ramWidget->show();
        break;
    default:
        break;
    }
}
