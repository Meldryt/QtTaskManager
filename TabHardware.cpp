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
        GpuInfoLabel= new QLabel(gpuWidget);

        QGridLayout *layout = new QGridLayout(gpuWidget);
        layout->addWidget(GpuInfoLabel,0,0);
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

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &TabHardware::process);
    timer->start();
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
    text += "CPU Brand : " + cpuBrand + "\n";
    text += "Number of Cores: " + processorCount + "\n";
    text += "Number of Threads: " + threadCount + "\n";
    text += "L1 Cache Size: " + l1CacheSize + " KB" + "\n";
    text += "L2 Cache Size: " + l2CacheSize + " KB" + "\n";
    text += "L3 Cache Size: " + l3CacheSize + " KB" + "\n";
    CpuInfoLabel->setText(text);
}

void TabHardware::processGPU()
{
    QString text;
    text += "GPU Brand : " + gpuBrand + "\n";
    //text += "Number of Cores: " + QString::number(staticSystemInfo.processorCount) + "\n";
    GpuInfoLabel->setText(text);
}

void TabHardware::processRAM()
{
    QString text;
    text += "RAM Brand : " + QString("") + "\n";
    text += "Total Physical Memory: " + totalPhysicalMemory + "MB \n";
    ramInfoLabel->setText(text);
}

void TabHardware::processDevices()
{

}

void TabHardware::slotCPUBrand(const std::string& val)
{
    cpuBrand = QString(val.c_str());
}

void TabHardware::slotProcessorCount(const uint8_t& val)
{
    processorCount = QString::number(val);
}

void TabHardware::slotThreadCount(const uint8_t& val)
{
    threadCount = QString::number(val);
}

void TabHardware::slotL1CacheSize(const uint32_t& val)
{
    l1CacheSize = QString::number(val);
}

void TabHardware::slotL2CacheSize(const uint32_t& val)
{
    l2CacheSize = QString::number(val);
}

void TabHardware::slotL3CacheSize(const uint32_t& val)
{
    l3CacheSize = QString::number(val);
}

void TabHardware::slotGPUBrand(const std::string& val)
{
    gpuBrand = QString(val.c_str());
}

void TabHardware::slotTotalPhysicalMemory(const uint32_t& val)
{
    totalPhysicalMemory = QString::number((val/1024)/1024);
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
