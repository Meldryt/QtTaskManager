#include "TabProcesses.h"
#include <QGridLayout>
#include <QDebug>
#include <QHeaderView>
#include <string>
#include <QTreeWidget>
#include <QScrollBar>

TabProcesses::TabProcesses(ProcessDatabase* database, QWidget *parent) : processDatabase(database), QWidget(parent)
{
    tableProcesses = new QTableWidget(this);
    tableProcesses->setColumnCount(5);
    QStringList headerNames;
    headerNames << "Name" << "FileName" << "FilePath" << "Memory Usage" << "CPU Usage" << "Disk Usage" << "Network Usage" << "GPU Usage";
    tableProcesses->setHorizontalHeaderLabels(headerNames);
    tableProcesses->verticalHeader()->hide();
    tableProcesses->setColumnWidth(0,200);
    tableProcesses->setColumnWidth(1,200);
    tableProcesses->setColumnWidth(2,200);
    tableProcesses->setWordWrap(false);
    tableProcesses->insertRow(0);

    for(uint8_t colIndex = 0; colIndex < headerNames.count(); ++colIndex)
    {
        tableProcesses->setItem(0,colIndex, new QTableWidgetItem());
    }
    tableProcesses->item(0,0)->setText("Total");
    //QTreeWidget* treeWidget = new QTreeWidget();
    //treeWidget->horizontalScrollBar()->hide();
    //treeWidget->horizontalScrollBar()->setDisabled(true);
    //treeWidget->setHeaderHidden(true);
    //treeWidget->setColumnCount(2);
    //tableProcesses->setCellWidget(0, 1, treeWidget);
    //auto widget = dynamic_cast<QTreeWidget*>(tableProcesses->cellWidget(0, 5));
//    QTreeWidgetItem* topLevel = new QTreeWidgetItem();
//    topLevel->setText(0, "A");
//    QTreeWidgetItem* child = new QTreeWidgetItem();
//    child->setText(0, "B");
//    treeWidget->addTopLevelItem(topLevel);
//    topLevel->addChild(child);
//    //treeWidget->setFixedSize(200,200);
//    tableProcesses->setCellWidget(0, 1, treeWidget);
//    tableProcesses->setRowHeight(0, 100);
    connect(tableProcesses->horizontalHeader(), &QHeaderView::sectionPressed, processDatabase, &ProcessDatabase::setSortMode);
    //connect(processDatabase, &ProcessDatabase::receivedProcessList, this, &TabProcesses::updateTable);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(tableProcesses,0,0);

    timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer, &QTimer::timeout, this, &TabProcesses::process);
    timer->start();
}

void TabProcesses::process()
{
    updateTable();
    updateTotalInfo();
}

void TabProcesses::updateTable()
{
    if(!processDatabase)
    {
        return;
    }

    const std::vector<WorkerProcess::ProcessInfo>& processList = processDatabase->getProcessList();

    if(processList.size() < tableProcesses->rowCount()-1)
    {
        tableProcesses->setRowCount(processList.size()+1);
    }


    for(uint32_t index = 0; index < processList.size(); ++index)
    {
        WorkerProcess::ProcessInfo process = processList.at(index);

        bool newRow{false};
        uint8_t tableRow = index + 1;
        if(tableProcesses->rowCount()-1 < tableRow)
        {
            tableProcesses->insertRow(tableRow);
            for(uint8_t colIndex = 0; colIndex < tableProcesses->columnCount(); ++colIndex)
            {
                tableProcesses->setItem(tableRow, colIndex, new QTableWidgetItem());
            }
            newRow = true;
        }
        if(newRow || tableProcesses->item(tableRow, 0)->text() != process.description)
        {
            tableProcesses->item(tableRow, 0)->setText(process.description);
            tableProcesses->item(tableRow, 1)->setText(process.baseName);
            tableProcesses->item(tableRow, 2)->setText(process.filePath);
        }
        uint64_t usedPhysicalMemory = (process.usedPhysicalMemory)/(1024*1024);
        tableProcesses->item(tableRow, 3)->setText(QString::number(usedPhysicalMemory) + " MB");
        tableProcesses->item(tableRow, 4)->setText(QString::number(process.usedCPULoad, 'f', 2) + " %");
        //uint64_t usedVirtualMemory = (process.usedVirtualMemory)/(1024*1024);
        //tableProcesses->setItem(tableRow, 4, new QTableWidgetItem(QString::number(usedVirtualMemory) + " MB"));
    }
}

void TabProcesses::updateTotalInfo()
{
    if(!processDatabase)
    {
        return;
    }

    const WorkerProcess::TotalInfo& totalInfo = processDatabase->getTotalInfo();

    uint64_t usedPhysicalMemory = (totalInfo.usedPhysicalMemory)/(1024*1024);
    uint64_t totalPhysicalMemory = (totalInfo.totalPhysicalMemory)/(1024*1024);
    tableProcesses->item(0, 3)->setText(QString::number(usedPhysicalMemory) + " / " +
                                        QString::number(totalPhysicalMemory) + " MB");

    QString text = QString::number(int(totalInfo.totalCPULoad)) + " (";
    for(uint8_t i = 0;i<totalInfo.singleCoreLoads.size();++i)
    {
        text += QString::number(int(totalInfo.singleCoreLoads[i])) + " | ";
    }
    text+=")";
    tableProcesses->item(0, 4)->setText(text);

//    uint64_t usedVirtualMemory = (totalInfo.usedVirtualMemory)/(1024*1024);
//    uint64_t totalVirtualMemory = (totalInfo.totalVirtualMemory)/(1024*1024);
//    tableProcesses->setItem(0, 4, new QTableWidgetItem(QString::number(usedVirtualMemory) + " / " +
//                                                       QString::number(totalVirtualMemory) + " MB"));
}

//void TabProcesses::addProcessItem(QString& baseName, QString& filePath)
//{
//    std::map<QString, int>::iterator itMap = processItems.find(baseName);
//    if(itMap == processItems.end())
//    {
//        uint32_t rowCount = tableProcesses->rowCount();
//        tableProcesses->setRowCount(rowCount+1);
//        tableProcesses->setItem(rowCount, 0, new QTableWidgetItem(baseName));
//        tableProcesses->setItem(rowCount, 1, new QTableWidgetItem(filePath));
//        processItems.insert({baseName, rowCount});
//    }
//}

//void TabProcesses::removeProcessItem(QString& baseName)
//{
//    std::map<QString, int>::iterator itMap = processItems.find(baseName);
//    if(itMap != processItems.end())
//    {
//        tableProcesses->removeRow(itMap->second);
//        processItems.erase(itMap);
//    }
//}

