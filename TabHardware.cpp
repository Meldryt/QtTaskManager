#include "TabHardware.h"

#include <QGridLayout>
#include <QPainter>
#include <QPoint>

TabHardware::TabHardware(ProcessDatabase* database, QWidget *parent)
    : QWidget{parent}, processDatabase(database)
{
    listWidget = new QListWidget(this);
    listWidget->addItem("CPU");
    listWidget->addItem("GPU");
    listWidget->addItem("RAM");
    listWidget->addItem("Disk");
    listWidget->addItem("Network");
    listWidget->setFixedWidth(100);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(listWidget,0,0);
    setLayout(layout);

    {
        cpuWidget = new QWidget(this);
        cpuInfoLabel= new QLabel(cpuWidget);

        QGridLayout *layout = new QGridLayout(cpuWidget);
        layout->addWidget(cpuInfoLabel,0,0);
        cpuWidget->setLayout(layout);
    }

    {
        gpuWidget = new QWidget(this);
        gpuInfoLabel= new QLabel(gpuWidget);

        QGridLayout *layout = new QGridLayout(gpuWidget);
        layout->addWidget(gpuInfoLabel,0,0);
        gpuWidget->setLayout(layout);
    }

    {
        ramWidget = new QWidget(this);
        ramInfoLabel= new QLabel(ramWidget);

        QGridLayout *layout = new QGridLayout(ramWidget);
        layout->addWidget(ramInfoLabel,0,0);
        ramWidget->setLayout(layout);
    }

    layout->addWidget(cpuWidget,0,1);
    layout->addWidget(gpuWidget,0,2);
    layout->addWidget(ramWidget,0,3);

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
}

void TabHardware::processCPU()
{
    const WorkerProcess::StaticSystemInfo& staticSystemInfo = processDatabase->getStaticSystemInfo();
    QString text;
    text += "CPU Brand : " + QString(staticSystemInfo.cpuBrand.c_str()) + "\n";
    text += "Number of Cores: " + QString::number(staticSystemInfo.processorCount) + "\n";
    cpuInfoLabel->setText(text);
}

void TabHardware::processGPU()
{
    const WorkerProcess::StaticSystemInfo& staticSystemInfo = processDatabase->getStaticSystemInfo();
    QString text;
    text += "GPU Brand : " + QString(staticSystemInfo.gpuBrand.c_str()) + "\n";
    //text += "Number of Cores: " + QString::number(staticSystemInfo.processorCount) + "\n";
    gpuInfoLabel->setText(text);
}

void TabHardware::processRAM()
{
    const WorkerProcess::StaticSystemInfo& staticSystemInfo = processDatabase->getStaticSystemInfo();
    QString text;
    text += "RAM Brand : " + QString("") + "\n";
    text += "Total Physical Memory: " + QString::number((staticSystemInfo.totalPhysicalMemory/1024)/1024) + "MB \n";
    ramInfoLabel->setText(text);
}

void TabHardware::showSelectionWidget()
{
    cpuWidget->hide();
    gpuWidget->hide();
    ramWidget->hide();

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
