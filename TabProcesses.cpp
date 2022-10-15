#include "TabProcesses.h"
#include <QGridLayout>
#include <QDebug>
#include <QHeaderView>

TabProcesses::TabProcesses(QWidget *parent) : QWidget(parent)
{
    tableProcesses = new QTableWidget(this);
    tableProcesses->setColumnCount(5);
    QStringList headerNames;
    headerNames << "Name" << "FileName" << "FilePath" << "Memory Usage" << "CPU Usage";
    tableProcesses->setHorizontalHeaderLabels(headerNames);
    tableProcesses->verticalHeader()->hide();
    tableProcesses->setColumnWidth(0,200);
    tableProcesses->setColumnWidth(1,200);
    tableProcesses->setColumnWidth(2,200);
    tableProcesses->setWordWrap(false);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(tableProcesses,0,0);

    threadProcesses = new QThread();
    workerProcess = new WorkerProcess();
    workerProcess->moveToThread(threadProcesses);

    //connect( threadProcesses, &QThread::started, workerProcess, &WorkerProcess::process);
    connect( threadProcesses, &QThread::started, workerProcess, &WorkerProcess::start);
    connect( workerProcess, &WorkerProcess::finished, threadProcesses, &QThread::quit);
    connect( workerProcess, &WorkerProcess::finished, workerProcess, &WorkerProcess::deleteLater);
    connect( threadProcesses, &QThread::finished, threadProcesses, &QThread::deleteLater);

    connect( workerProcess, &WorkerProcess::receivedList, this, &TabProcesses::updateList);

    threadProcesses->start();
}

void TabProcesses::updateList()
{
    qDebug() << "updateList";
    std::vector<WorkerProcess::ProcessInfo> processList;
    workerProcess->getProcessList(processList);
    if(processList.size() < tableProcesses->rowCount())
    {
        tableProcesses->setRowCount(processList.size());
    }

    for(uint32_t index = 0; index < processList.size(); ++index)
    {
        bool newRow{false};
        if(index >= tableProcesses->rowCount())
        {
            tableProcesses->insertRow(index);
            newRow = true;
        }
        if(newRow || tableProcesses->item(index, 0)->text() != processList.at(index).description)
        {
            tableProcesses->setItem(index, 0, new QTableWidgetItem(processList.at(index).description));
            tableProcesses->setItem(index, 1, new QTableWidgetItem(processList.at(index).baseName));
            tableProcesses->setItem(index, 2, new QTableWidgetItem(processList.at(index).filePath));
        }
        uint64_t usedPhysicalMemory = (processList.at(index).usedPhysicalMemory)/(1024*1024);
        tableProcesses->setItem(index, 3, new QTableWidgetItem(QString::number(usedPhysicalMemory) + " MB"));
        uint64_t usedVirtualMemory = (processList.at(index).usedVirtualMemory)/(1024*1024);
        tableProcesses->setItem(index, 4, new QTableWidgetItem(QString::number(usedVirtualMemory) + " MB"));
    }
    //tableProcesses->clearContents();
    //tableProcesses->setRowCount(processList.size());
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

