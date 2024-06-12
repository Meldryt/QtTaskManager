#include "TabProcesses.h"
#include <QGridLayout>
#include <QDebug>
#include <QHeaderView>
#include <string>
#include <QTreeWidget>
#include <QScrollBar>

TabProcesses::TabProcesses(QWidget *parent) : QWidget(parent)
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
    //connect(processDatabase, &ProcessDatabase::receivedProcessList, this, &TabProcesses::updateTable);

    connect(tableProcesses->horizontalHeader(), &QHeaderView::sectionPressed, this, &TabProcesses::setSortMode);

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

    sortTable();
}

void TabProcesses::updateTable()
{
    if(processList.size() < tableProcesses->rowCount()-1)
    {
        tableProcesses->setRowCount(processList.size()+1);
    }


    for(uint32_t index = 0; index < processList.size(); ++index)
    {
        ProcessInfo::Process process = processList.at(index);

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
        if(newRow || tableProcesses->item(tableRow, 0)->text() != process.description.c_str())
        {
            tableProcesses->item(tableRow, 0)->setText(process.description.c_str());
            tableProcesses->item(tableRow, 1)->setText(process.baseName.c_str());
            tableProcesses->item(tableRow, 2)->setText(process.filePath.c_str());
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
//    const WorkerProcess::DynamicSystemInfo& dynamicSystemInfo = processDatabase->getDynamicSystemInfo();
//    const WorkerProcess::StaticSystemInfo& staticSystemInfo = processDatabase->getStaticSystemInfo();
//    uint64_t usedPhysicalMemory = (dynamicSystemInfo.usedPhysicalMemory)/(1024*1024);
//    uint64_t totalPhysicalMemory = (staticSystemInfo.totalPhysicalMemory)/(1024*1024);
//    tableProcesses->item(0, 3)->setText(QString::number(usedPhysicalMemory) + " / " +
//                                        QString::number(totalPhysicalMemory) + " MB");

//    QString text = QString::number(int(dynamicSystemInfo.totalCPULoad)) + " (";
//    for(uint8_t i = 0;i<dynamicSystemInfo.singleCoreLoads.size();++i)
//    {
//        text += QString::number(int(dynamicSystemInfo.singleCoreLoads[i])) + " | ";
//    }
//    text+=")";
//    tableProcesses->item(0, 4)->setText(text);

//    uint64_t usedVirtualMemory = (totalInfo.usedVirtualMemory)/(1024*1024);
//    uint64_t totalVirtualMemory = (totalInfo.totalVirtualMemory)/(1024*1024);
//    tableProcesses->setItem(0, 4, new QTableWidgetItem(QString::number(usedVirtualMemory) + " / " +
//                                                       QString::number(totalVirtualMemory) + " MB"));
}

void TabProcesses::slotProcesses(const std::map<uint32_t, ProcessInfo::Process>& processMap)
{
    if (processMap.empty())
    {
        return;
    }

    bool changed{false};
    uint8_t index = 0;
    auto process = std::begin(processMap);
    int64_t currTime = process->second.timestamp;

    while (process != std::end(processMap))
    {
        auto it = std::find_if(processList.begin(), processList.end(), [process](const ProcessInfo::Process& processInfo)
        {
            return (process->second.processID == processInfo.processID);
        });

        if(it == processList.end())
        {
            processList.push_back(process->second);
            changed = true;
        }
        else
        {
            it->usedPhysicalMemory = process->second.usedPhysicalMemory;
            it->usedVirtualMemory = process->second.usedVirtualMemory;
            it->usedCPULoad = process->second.usedCPULoad;
            it->timestamp = process->second.timestamp;
        }

        ++process;
        ++index;
    }

    for(auto it = std::begin(processList); it != std::end(processList); )
    {
        if(it->timestamp+1 < currTime)
        {
            it = processList.erase(it);
            changed = true;
        }
        else
        {
            ++it;
        }
    }

    changed = true;
}

void TabProcesses::sortTable()
{
    switch(sortMode)
    {
    case SortMode::SortProcessName:
        sortNames();
        break;
    case SortMode::SortFileName:
        sortFileNames();
        break;
    case SortMode::SortMemoryUsageHigh:
    case SortMode::SortMemoryUsageLow:
        sortMemoryUsage();
        break;
    case SortMode::SortCPUUsageHigh:
    case SortMode::SortCPUUsageLow:
        sortCPUUsage();
        break;
    default:
        break;
    }
}

void TabProcesses::setSortMode(int headerIndex)
{
    if(headerIndex == 0)
    {
        sortMode = SortMode::SortProcessName;
    }
    else if(headerIndex == 1)
    {
        sortMode = SortMode::SortFileName;
    }
    else if(headerIndex == 3)
    {
        if(sortMode == SortMode::SortMemoryUsageHigh)
        {
            sortMode = SortMode::SortMemoryUsageLow;
        }
        else
        {
            sortMode = SortMode::SortMemoryUsageHigh;
        }
    }
    else if(headerIndex == 4)
    {
        if(sortMode == SortMode::SortCPUUsageHigh)
        {
            sortMode = SortMode::SortCPUUsageLow;
        }
        else
        {
            sortMode = SortMode::SortCPUUsageHigh;
        }
    }

    sortTable();
}

bool compareNames(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    uint8_t i = 0;
    std::string firstString = first.description;
    std::string secondString = second.description;
    while(i < firstString.length() && i < second.description.length())
    {
        if(std::tolower(firstString.at(i))<std::tolower(secondString.at(i)))
        {
            return true;
        }
        else if(std::tolower(firstString.at(i))>std::tolower(secondString.at(i)))
        {
            return false;
        }
        else
        {
            ++i;
        }
    }
    return ( firstString.length() < secondString.length() );
}

bool compareFileNames(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    uint8_t i = 0;
    std::string firstString = first.baseName;
    std::string secondString = second.baseName;
    while(i < firstString.length() && i < second.baseName.length())
    {
        if(std::tolower(firstString.at(i))<std::tolower(secondString.at(i)))
        {
            return true;
        }
        else if(std::tolower(firstString.at(i))>std::tolower(secondString.at(i)))
        {
            return false;
        }
        else
        {
            ++i;
        }
    }
    return ( firstString.length() < secondString.length() );
}

bool compareMemoryHigher(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedPhysicalMemory > second.usedPhysicalMemory;
}

bool compareMemoryLower(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedPhysicalMemory < second.usedPhysicalMemory;
}

bool compareCPUHigher(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedCPULoad > second.usedCPULoad;
}

bool compareCPULower(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedCPULoad < second.usedCPULoad;
}

void TabProcesses::sortNames()
{
    std::sort(processList.begin(),processList.end(),compareNames);
}

void TabProcesses::sortFileNames()
{
    std::sort(processList.begin(),processList.end(),compareFileNames);
}

void TabProcesses::sortMemoryUsage()
{
    if(sortMode == SortMode::SortMemoryUsageHigh)
    {
        std::sort(processList.begin(),processList.end(),compareMemoryHigher);
    }
    else if(sortMode == SortMode::SortMemoryUsageLow)
    {
        std::sort(processList.begin(),processList.end(),compareMemoryLower);
    }
}

void TabProcesses::sortCPUUsage()
{
    if(sortMode == SortMode::SortCPUUsageHigh)
    {
        std::sort(processList.begin(),processList.end(),compareCPUHigher);
    }
    else if(sortMode == SortMode::SortCPUUsageLow)
    {
        std::sort(processList.begin(),processList.end(),compareCPULower);
    }
}
