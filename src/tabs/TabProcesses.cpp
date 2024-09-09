#include "TabProcesses.h"
#include <QGridLayout>
#include <QDebug>
#include <QHeaderView>
#include <string>
#include <QTreeWidget>
#include <QScrollBar>

TabProcesses::TabProcesses(QWidget *parent) : QWidget(parent)
{
    qDebug() << __FUNCTION__;

    m_tableProcesses = new QTableWidget(this);
    m_tableProcesses->setColumnCount(TableHeaderNames.size());

    m_tableProcesses->setHorizontalHeaderLabels(TableHeaderNames);
    m_tableProcesses->verticalHeader()->hide();
    m_tableProcesses->setColumnWidth(0,200);
    m_tableProcesses->setColumnWidth(1,200);
    m_tableProcesses->setColumnWidth(2,200);
    m_tableProcesses->setWordWrap(false);
    m_tableProcesses->insertRow(0);

    for(uint8_t colIndex = 0; colIndex < TableHeaderNames.count(); ++colIndex)
    {
        m_tableProcesses->setItem(0,colIndex, new QTableWidgetItem());
    }
    m_tableProcesses->item(0,0)->setText("Total");

    connect(m_tableProcesses->horizontalHeader(), &QHeaderView::sectionPressed, this, &TabProcesses::setSortMode);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_tableProcesses,0,0);
}

TabProcesses::~TabProcesses()
{
    qDebug() << __FUNCTION__;
}

void TabProcesses::process()
{
    setUpdatesEnabled(false);

    updateTable();
    updateTotalInfo();
    sortTable();

    setUpdatesEnabled(true);
}

void TabProcesses::updateTable()
{
    if(m_processList.size() < m_tableProcesses->rowCount()-1)
    {
        m_tableProcesses->setRowCount(static_cast<int>(m_processList.size()+1));
    }


    for(uint32_t index = 0; index < m_processList.size(); ++index)
    {
        ProcessInfo::Process process = m_processList.at(index);

        bool newRow{false};
        uint8_t tableRow = index + 1;
        if(m_tableProcesses->rowCount()-1 < tableRow)
        {
            m_tableProcesses->insertRow(tableRow);
            for(uint8_t colIndex = 0; colIndex < m_tableProcesses->columnCount(); ++colIndex)
            {
                m_tableProcesses->setItem(tableRow, colIndex, new QTableWidgetItem());
            }
            newRow = true;
        }
        if(newRow || m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::ProcessName))->text() != process.description.c_str())
        {
            m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::ProcessName))->setText(process.description.c_str());
            m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::FileName))->setText(process.baseName.c_str());
            m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::FilePath))->setText(process.filePath.c_str());
        }
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::MemoryUsage))->setText(QString::number(process.usedPhysicalMemory / (1024 * 1024), 'g', 6) + " MB");
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::CpuUsage))->setText(QString::number(process.usedCpuLoad, 'f', 2) + " %");
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::GpuUsage))->setText(QString::number(process.usedGpuLoad, 'g', 2) + " %");
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::GpuMemoryUsed))->setText(QString::number(process.usedGpuMemory / 1000, 'g', 9) + " KB");

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

//    QString text = QString::number(int(dynamicSystemInfo.totalCpuLoad)) + " (";
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
        auto it = std::find_if(m_processList.begin(), m_processList.end(), [process](const ProcessInfo::Process& processInfo)
        {
            return (process->second.processID == processInfo.processID);
        });

        if(it == m_processList.end())
        {
            m_processList.push_back(process->second);
            changed = true;
        }
        else
        {
            it->usedPhysicalMemory = process->second.usedPhysicalMemory;
            it->usedVirtualMemory = process->second.usedVirtualMemory;
            it->usedCpuLoad = process->second.usedCpuLoad;
            it->usedGpuLoad = process->second.usedGpuLoad;
            it->usedGpuMemory = process->second.usedGpuMemory;
            it->timestamp = process->second.timestamp;
        }

        ++process;
        ++index;
    }

    for(auto it = std::begin(m_processList); it != std::end(m_processList); )
    {
        if(it->timestamp+1 < currTime)
        {
            it = m_processList.erase(it);
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
    switch(m_sortMode)
    {
    case SortMode::SortProcessName:
        sortNames();
        break;
    case SortMode::SortFileName:
        sortFileNames();
        break;
    case SortMode::SortPhysicalMemoryUsageHigh:
    case SortMode::SortPhysicalMemoryUsageLow:
        sortMemoryUsage();
        break;
    case SortMode::SortCpuUsageHigh:
    case SortMode::SortCpuUsageLow:
        sortCpuUsage();
        break;
    case SortMode::SortGpuUsageHigh:
    case SortMode::SortGpuUsageLow:
        sortGpuUsage();
        break;
    case SortMode::SortGpuMemoryUsedHigh:
    case SortMode::SortGpuMemoryUsedLow:
        sortGpuMemoryUsed();
        break;
    default:
        break;
    }
}

void TabProcesses::setSortMode(int headerIndex)
{
    if(headerIndex == static_cast<int>(ColumnType::ProcessName))
    {
        m_sortMode = SortMode::SortProcessName;
    }
    else if(headerIndex == static_cast<int>(ColumnType::FileName))
    {
        m_sortMode = SortMode::SortFileName;
    }
    else if(headerIndex == static_cast<int>(ColumnType::MemoryUsage))
    {
        if(m_sortMode == SortMode::SortPhysicalMemoryUsageHigh)
        {
            m_sortMode = SortMode::SortPhysicalMemoryUsageLow;
        }
        else
        {
            m_sortMode = SortMode::SortPhysicalMemoryUsageHigh;
        }
    }
    else if(headerIndex == static_cast<int>(ColumnType::CpuUsage))
    {
        if(m_sortMode == SortMode::SortCpuUsageHigh)
        {
            m_sortMode = SortMode::SortCpuUsageLow;
        }
        else
        {
            m_sortMode = SortMode::SortCpuUsageHigh;
        }
    }
    else if (headerIndex == static_cast<int>(ColumnType::GpuUsage))
    {
        if (m_sortMode == SortMode::SortGpuUsageHigh)
        {
            m_sortMode = SortMode::SortGpuUsageLow;
        }
        else
        {
            m_sortMode = SortMode::SortGpuUsageHigh;
        }
    }
    else if (headerIndex == static_cast<int>(ColumnType::GpuMemoryUsed))
    {
        if (m_sortMode == SortMode::SortGpuMemoryUsedHigh)
        {
            m_sortMode = SortMode::SortGpuMemoryUsedLow;
        }
        else
        {
            m_sortMode = SortMode::SortGpuMemoryUsedHigh;
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

bool compareMaxPhysicalMemory(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedPhysicalMemory > second.usedPhysicalMemory;
}

bool compareMinPhysicalMemory(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedPhysicalMemory < second.usedPhysicalMemory;
}

bool compareMaxCpuLoad(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedCpuLoad > second.usedCpuLoad;
}

bool compareMinCpuLoad(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedCpuLoad < second.usedCpuLoad;
}

bool compareMaxGpuLoad(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedGpuLoad > second.usedGpuLoad;
}

bool compareMinGpuLoad(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedGpuLoad < second.usedGpuLoad;
}

bool compareMaxGpuMemoryUsed(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedGpuMemory > second.usedGpuMemory;
}

bool compareMinGpuMemoryUsed(const ProcessInfo::Process& first, const ProcessInfo::Process& second)
{
    return first.usedGpuMemory < second.usedGpuMemory;
}

void TabProcesses::sortNames()
{
    std::sort(m_processList.begin(),m_processList.end(),compareNames);
}

void TabProcesses::sortFileNames()
{
    std::sort(m_processList.begin(),m_processList.end(),compareFileNames);
}

void TabProcesses::sortMemoryUsage()
{
    if(m_sortMode == SortMode::SortPhysicalMemoryUsageHigh)
    {
        std::sort(m_processList.begin(),m_processList.end(), compareMaxPhysicalMemory);
    }
    else if(m_sortMode == SortMode::SortPhysicalMemoryUsageLow)
    {
        std::sort(m_processList.begin(),m_processList.end(), compareMinPhysicalMemory);
    }
}

void TabProcesses::sortCpuUsage()
{
    if(m_sortMode == SortMode::SortCpuUsageHigh)
    {
        std::sort(m_processList.begin(),m_processList.end(), compareMaxCpuLoad);
    }
    else if(m_sortMode == SortMode::SortCpuUsageLow)
    {
        std::sort(m_processList.begin(),m_processList.end(), compareMinCpuLoad);
    }
}

void TabProcesses::sortGpuUsage()
{
    if (m_sortMode == SortMode::SortGpuUsageHigh)
    {
        std::sort(m_processList.begin(), m_processList.end(), compareMaxGpuLoad);
    }
    else if (m_sortMode == SortMode::SortGpuUsageLow)
    {
        std::sort(m_processList.begin(), m_processList.end(), compareMinGpuLoad);
    }
}

void TabProcesses::sortGpuMemoryUsed()
{
    if (m_sortMode == SortMode::SortGpuMemoryUsedHigh)
    {
        std::sort(m_processList.begin(), m_processList.end(), compareMaxGpuMemoryUsed);
    }
    else if (m_sortMode == SortMode::SortGpuMemoryUsedLow)
    {
        std::sort(m_processList.begin(), m_processList.end(), compareMinGpuMemoryUsed);
    }
}
