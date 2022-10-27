#include "ProcessDatabase.h"

ProcessDatabase::ProcessDatabase()
{
    threadProcesses = new QThread();
    workerProcess = new WorkerProcess();
    workerProcess->moveToThread(threadProcesses);

    //connect( threadProcesses, &QThread::started, workerProcess, &WorkerProcess::process);
    connect( threadProcesses, &QThread::started, workerProcess, &WorkerProcess::start);
    connect( workerProcess, &WorkerProcess::finished, threadProcesses, &QThread::quit);
    connect( workerProcess, &WorkerProcess::finished, workerProcess, &WorkerProcess::deleteLater);
    connect( threadProcesses, &QThread::finished, threadProcesses, &QThread::deleteLater);

    connect( workerProcess, &WorkerProcess::receivedProcessList, this, &ProcessDatabase::updateProcessList);
    connect( workerProcess, &WorkerProcess::receivedTotalInfo, this, &ProcessDatabase::updateTotalInfo);

    threadProcesses->start();
}

const std::vector<WorkerProcess::ProcessInfo>& ProcessDatabase::getProcessList()
{
    return processList;
}

const WorkerProcess::TotalInfo& ProcessDatabase::getTotalInfo()
{
    return totalInfo;
}

void ProcessDatabase::updateProcessList()
{
    bool listChanged = workerProcess->fillProcessList(processList);

    if(!listChanged)
    {
    }

    sortTable();
}

void ProcessDatabase::updateTotalInfo()
{
    bool listChanged = workerProcess->fillTotalInfo(totalInfo);

//    uint64_t usedPhysicalMemory = (totalInfo.usedPhysicalMemory)/(1024*1024);
//    uint64_t totalPhysicalMemory = (totalInfo.totalPhysicalMemory)/(1024*1024);
//    //tableProcesses->item(0, 3)->setText(QString::number(usedPhysicalMemory) + " / " +
//    //                                    QString::number(totalPhysicalMemory) + " MB");

//    QString text = QString::number(int(totalInfo.totalCPULoad)) + " (";
//    for(uint8_t i = 0;i<totalInfo.singleCoreLoads.size();++i)
//    {
//        text += QString::number(int(totalInfo.singleCoreLoads[i])) + " | ";
//    }
//    text+=")";
//    //tableProcesses->item(0, 4)->setText(text);

//    uint64_t usedVirtualMemory = (totalInfo.usedVirtualMemory)/(1024*1024);
//    uint64_t totalVirtualMemory = (totalInfo.totalVirtualMemory)/(1024*1024);
//    //tableProcesses->setItem(0, 4, new QTableWidgetItem(QString::number(usedVirtualMemory) + " / " +
//    //                                                   QString::number(totalVirtualMemory) + " MB"));
}

void ProcessDatabase::sortTable()
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

void ProcessDatabase::setSortMode(int headerIndex)
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

bool compareNames(const WorkerProcess::ProcessInfo& first, const WorkerProcess::ProcessInfo& second)
{
    uint8_t i = 0;
    std::string firstString = first.description.toStdString();
    std::string secondString = second.description.toStdString();
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

bool compareFileNames(const WorkerProcess::ProcessInfo& first, const WorkerProcess::ProcessInfo& second)
{
    uint8_t i = 0;
    std::string firstString = first.baseName.toStdString();
    std::string secondString = second.baseName.toStdString();
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

bool compareMemoryHigher(const WorkerProcess::ProcessInfo& first, const WorkerProcess::ProcessInfo& second)
{
    return first.usedPhysicalMemory > second.usedPhysicalMemory;
}

bool compareMemoryLower(const WorkerProcess::ProcessInfo& first, const WorkerProcess::ProcessInfo& second)
{
    return first.usedPhysicalMemory < second.usedPhysicalMemory;
}

bool compareCPUHigher(const WorkerProcess::ProcessInfo& first, const WorkerProcess::ProcessInfo& second)
{
    return first.usedCPULoad > second.usedCPULoad;
}

bool compareCPULower(const WorkerProcess::ProcessInfo& first, const WorkerProcess::ProcessInfo& second)
{
    return first.usedCPULoad < second.usedCPULoad;
}

void ProcessDatabase::sortNames()
{
    std::sort(processList.begin(),processList.end(),compareNames);
}

void ProcessDatabase::sortFileNames()
{
    std::sort(processList.begin(),processList.end(),compareFileNames);
}

void ProcessDatabase::sortMemoryUsage()
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

void ProcessDatabase::sortCPUUsage()
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
