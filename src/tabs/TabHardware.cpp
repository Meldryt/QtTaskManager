#include "TabHardware.h"

#include <QGridLayout>
#include <QPainter>
#include <QPoint>
#include <QHeaderView>

TabHardware::TabHardware(QWidget *parent) : QWidget{parent}
{
    qDebug() << __FUNCTION__;

    m_listWidget = new QListWidget(this);
    m_listWidget->addItem("CPU");
    m_listWidget->addItem("GPU");
    m_listWidget->addItem("RAM");
    m_listWidget->addItem("Devices");
    //listWidget->addItem("Disk");
    //listWidget->addItem("Network");
    m_listWidget->setFixedWidth(100);

    m_cpuTableWidget = new QTableWidget(this);
    m_cpuTableWidget->setRowCount(CpuParameterNames.size());
    m_cpuTableWidget->setColumnCount(2);
    m_cpuTableWidget->horizontalHeader()->setStretchLastSection(true);
    m_cpuTableWidget->horizontalHeader()->hide();
    m_cpuTableWidget->verticalHeader()->hide();
    m_cpuTableWidget->setColumnWidth(0, 200);

    for (int i = 0; i < CpuParameterNames.size(); ++i)
    {
        m_cpuTableWidget->setItem(i, 0, new QTableWidgetItem(CpuParameterNames.at(i)));
        m_cpuTableWidget->setItem(i, 1, new QTableWidgetItem());
    }

    m_gpuTableWidget = new QTableWidget(this);
    m_gpuTableWidget->setRowCount(GpuParameterNames.size());
    m_gpuTableWidget->setColumnCount(2);
    m_gpuTableWidget->horizontalHeader()->setStretchLastSection(true);
    m_gpuTableWidget->horizontalHeader()->hide();
    m_gpuTableWidget->verticalHeader()->hide();
    m_gpuTableWidget->setColumnWidth(0, 200);

    for (int i = 0; i < GpuParameterNames.size(); ++i)
    {
        m_gpuTableWidget->setItem(i, 0, new QTableWidgetItem(GpuParameterNames.at(i)));
        m_gpuTableWidget->setItem(i, 1, new QTableWidgetItem());
    }

    m_memoryTableWidget = new QTableWidget(this);
    m_memoryTableWidget->setRowCount(MemoryParameterNames.size());
    m_memoryTableWidget->setColumnCount(2);
    m_memoryTableWidget->horizontalHeader()->setStretchLastSection(true);
    m_memoryTableWidget->horizontalHeader()->hide();
    m_memoryTableWidget->verticalHeader()->hide();
    m_memoryTableWidget->setColumnWidth(0, 200);

    for (int i = 0; i < MemoryParameterNames.size(); ++i)
    {
        m_memoryTableWidget->setItem(i, 0, new QTableWidgetItem(MemoryParameterNames.at(i)));
        m_memoryTableWidget->setItem(i, 1, new QTableWidgetItem());
    }

    m_devicesTableWidget = new QTableWidget(this);
    m_devicesTableWidget->setRowCount(DevicesParameterNames.size());
    m_devicesTableWidget->setColumnCount(2);
    m_devicesTableWidget->horizontalHeader()->setStretchLastSection(true);
    m_devicesTableWidget->horizontalHeader()->hide();
    m_devicesTableWidget->verticalHeader()->hide();
    m_devicesTableWidget->setColumnWidth(0, 200);

    for (int i = 0; i < DevicesParameterNames.size(); ++i)
    {
        m_devicesTableWidget->setItem(i, 0, new QTableWidgetItem(DevicesParameterNames.at(i)));
        m_devicesTableWidget->setItem(i, 1, new QTableWidgetItem());
    }

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(m_listWidget, 0, 0);
    layout->addWidget(m_cpuTableWidget,0,1);
    layout->addWidget(m_gpuTableWidget,0,2);
    layout->addWidget(m_memoryTableWidget,0,3);
    layout->addWidget(m_devicesTableWidget,0,4);
    setLayout(layout);

    connect(m_listWidget,&QListWidget::itemSelectionChanged, this, &TabHardware::showSelectionWidget);
    m_listWidget->setCurrentRow(0);
}

TabHardware::~TabHardware()
{
    qDebug() << __FUNCTION__;
}

void TabHardware::process()
{
    processCpu();
    processGpu();
    processMemory();
    processDevices();
}

void TabHardware::processCpu()
{
    for (int i = 0; i < m_cpuTableInfos.size(); ++i)
    {
        m_cpuTableWidget->item(i, 1)->setText(m_cpuTableInfos[i]);
    }
}

void TabHardware::processGpu()
{
    for (int i = 0; i < m_gpuTableInfos.size(); ++i)
    {
        m_gpuTableWidget->item(i, 1)->setText(m_gpuTableInfos[i]);
    }
}

void TabHardware::processMemory()
{
    for (int i = 0; i < m_memoryTableInfos.size(); ++i)
    {
        m_memoryTableWidget->item(i, 1)->setText(m_memoryTableInfos[i]);
    }
}

void TabHardware::processDevices()
{

}

void TabHardware::slotCpuStaticInfo(const QMap<uint8_t,QVariant>& staticInfo)
{
    uint8_t entryIdx = 0;
    for (uint8_t key = Globals::Key_Cpu_Static_Start + 1; key < Globals::Key_Cpu_Static_End; ++key)
    {
        m_cpuTableInfos[entryIdx] = staticInfo[key].toString();
        ++entryIdx;
    }
}

void TabHardware::slotGpuStaticInfo(const QMap<uint8_t,QVariant>& staticInfo)
{
    uint8_t entryIdx = 0;
    for (uint8_t key = Globals::Key_Gpu_Static_Start + 1; key < Globals::Key_Gpu_Static_End; ++key)
    {
        m_gpuTableInfos[entryIdx] = staticInfo[key].toString();
        ++entryIdx;
    }
}

void TabHardware::slotTotalPhysicalMemory(const uint32_t& val)
{
    m_memoryTableInfos[0] = QString::number(val);
    m_memoryTableInfos[1] = "";
}

void TabHardware::showSelectionWidget()
{
    m_cpuTableWidget->hide();
    m_gpuTableWidget->hide();
    m_memoryTableWidget->hide();
    m_devicesTableWidget->hide();

    switch(m_listWidget->currentRow())
    {
    case 0:
        m_cpuTableWidget->show();
        break;
    case 1:
        m_gpuTableWidget->show();
        break;
    case 2:
        m_memoryTableWidget->show();
        break;
    case 3:
        m_devicesTableWidget->show();
        break;
    default:
        break;
    }
}