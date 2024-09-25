#include "TabProcesses.h"

#include "../Globals.h"

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
    sortProcessList();

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
        const ProcessInfo::Process& process = m_processList.at(index);

        bool newRow{false};
        const uint32_t tableRow = index + 1;
        if(static_cast<uint32_t>(m_tableProcesses->rowCount()-1) < tableRow)
        {
            m_tableProcesses->insertRow(tableRow);
            for(uint8_t colIndex = 0; colIndex < m_tableProcesses->columnCount(); ++colIndex)
            {
                m_tableProcesses->setItem(tableRow, colIndex, new QTableWidgetItem());
            }
            newRow = true;
        }
        if(newRow || m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::ProcessName))->text() != process.processName.c_str())
        {
            m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::ProcessName))->setText(process.processName.c_str());
            m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::Description))->setText(process.description.c_str());
            m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::FilePath))->setText(process.filePath.c_str());
        }
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::ID))->setText(QString::number(process.processID));
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::RamUsagePercent))->setText(QString::number(process.ramUsagePercent, 'g', 2));
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::CpuUsagePercent))->setText(QString::number(process.cpuUsagePercent, 'g', 2));
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::GpuUsagePercent))->setText(QString::number(process.gpuUsagePercent, 'g', 2));
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::VideoRamUsagePercent))->setText(QString::number(process.videoRamUsagePercent, 'g', 2));

        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::RamUsageSize))->setText(QString::number(process.ramUsageSize / 1000, 'g', 9));
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::VirtualRamUsageSize))->setText(QString::number(process.virtualRamUsageSize / 1000, 'g', 9));
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::VideoRamUsageSize))->setText(QString::number(process.videoRamUsageSize / 1000, 'g', 9));
        m_tableProcesses->item(tableRow, static_cast<int>(ColumnType::DiskUsage))->setText(QString::number(process.videoRamUsageSize / 1000, 'g', 9));

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

void TabProcesses::slotProcessDynamicInfo(const QMap<uint8_t, QVariant>& dynamicInfo)
{
    QVariant variant = dynamicInfo[Globals::SysInfoAttr::Key_Process_Dynamic_Info];
    std::map<uint32_t, ProcessInfo::Process> processMap;
    if (variant.canConvert<std::map<uint32_t, ProcessInfo::Process>>())
    {
        processMap = variant.value<std::map<uint32_t, ProcessInfo::Process>>();
    }

    if (processMap.empty())
    {
        return;
    }

    bool changed{false};
    uint8_t index = 0;
    auto process = std::begin(processMap);

    while (process != std::end(processMap))
    {
        auto it = std::find_if(m_processList.begin(), m_processList.end(), [process](const ProcessInfo::Process& processInfo)
        {
            return (process->second.processID == processInfo.processID);
        });

        if(it == m_processList.end())
        {
            m_processList.push_back(process->second);
            m_processList.back().visited = true;
            changed = true;
        }
        else
        {
            it->ramUsagePercent = process->second.ramUsagePercent;
            it->cpuUsagePercent = process->second.cpuUsagePercent;
            it->gpuUsagePercent = process->second.gpuUsagePercent;
            it->videoRamUsagePercent = process->second.videoRamUsagePercent;
            it->ramUsageSize = process->second.ramUsageSize;
            it->virtualRamUsageSize = process->second.virtualRamUsageSize;
            it->videoRamUsageSize = process->second.videoRamUsageSize;
            it->cpuAverageUsagePercent = process->second.cpuAverageUsagePercent;
            it->gpuAverageUsagePercent = process->second.cpuAverageUsagePercent;
            it->timestamp = process->second.timestamp;
            it->visited = true;
        }

        ++process;
        ++index;
    }

    for(auto it = std::begin(m_processList); it != std::end(m_processList); )
    {
        if(!it->visited)
        {
            it = m_processList.erase(it);
            changed = true;
        }
        else
        {
            it->visited = false;
            ++it;
        }
    }

    changed = true;
}

void TabProcesses::sortProcessList()
{
    switch(m_sortMode)
    {
    case SortMode::SortByProcessName:
        sortByNames();
        break;
    case SortMode::SortByID:
        sortByIDs();
        break;
    case SortMode::SortByDescription:
        sortByDescriptions();
        break;
    case SortMode::SortByFilePath:
        sortByFilePath();
        break;
    case SortMode::SortByRamUsagePercent:
        sortByRamUsagePercent();
        break;
    case SortMode::SortByCpuUsagePercent:
        sortByCpuUsagePercent();
        break;
    case SortMode::SortByGpuUsagePercent:
        sortByGpuUsagePercent();
        break;
    case SortMode::SortByVideoRamUsagePercent:
        sortByVideoRamUsagePercent();
        break;
    case SortMode::SortByRamUsageSize:
        sortByRamUsageSize();
        break;
    case SortMode::SortByVirtualRamUsageSize:
        sortByVirtualRamUsageSize();
        break;
    case SortMode::SortByVideoRamUsageSize:
        sortByVideoRamUsageSize();
        break;
    default:
        break;
    }
}

void TabProcesses::setSortMode(int headerIndex)
{
    const SortMode prevSortMode = m_sortMode;
    m_sortMode = static_cast<SortMode>(headerIndex);

    if(prevSortMode == m_sortMode)
    {
        m_sortAscending = !m_sortAscending;
    }
    else
    {
        m_sortAscending = DefaultAscendingSortMode.at(m_sortMode);
    }

    sortProcessList();
}

bool isStringHigher(const std::string first, const std::string second)
{
    uint8_t i = 0;
    while(i < first.length() && i < second.length())
    {
        if(std::tolower(first.at(i))<std::tolower(second.at(i)))
        {
            return true;
        }
        else if(std::tolower(first.at(i))>std::tolower(second.at(i)))
        {
            return false;
        }
        else
        {
            ++i;
        }
    }
    return ( first.length() < second.length() );
}

bool isStringLower(const std::string first, const std::string second)
{
    uint8_t i = 0;
    while(i < first.length() && i < second.length())
    {
        if(std::tolower(first.at(i))>std::tolower(second.at(i)))
        {
            return true;
        }
        else if(std::tolower(first.at(i))<std::tolower(second.at(i)))
        {
            return false;
        }
        else
        {
            ++i;
        }
    }
    return ( first.length() < second.length() );
}

void TabProcesses::sortByNames()
{
    if(m_sortAscending)
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return isStringHigher(a.processName, b.processName);});
    }
    else
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return isStringLower(a.processName, b.processName);});
    }
}

void TabProcesses::sortByIDs()
{
    if(m_sortAscending)
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.processID < b.processID;});
    }
    else
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.processID > b.processID;});
    }
}

void TabProcesses::sortByDescriptions()
{
    if(m_sortAscending)
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return isStringHigher(a.description, b.description);});
    }
    else
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return isStringLower(a.description, b.description);});
    }
}

void TabProcesses::sortByFilePath()
{
    if(m_sortAscending)
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return isStringHigher(a.filePath, b.filePath);});
    }
    else
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return isStringLower(a.filePath, b.filePath);});
    }
}

void TabProcesses::sortByRamUsagePercent()
{
    if(m_sortAscending)
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.ramUsagePercent < b.ramUsagePercent;});
    }
    else
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.ramUsagePercent > b.ramUsagePercent;});
    }
}

void TabProcesses::sortByCpuUsagePercent()
{
    if(m_sortAscending)
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.cpuUsagePercent < b.cpuUsagePercent;});
    }
    else
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.cpuUsagePercent > b.cpuUsagePercent;});
    }
}

void TabProcesses::sortByGpuUsagePercent()
{
    if(m_sortAscending)
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.gpuUsagePercent < b.gpuUsagePercent;});
    }
    else
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.gpuUsagePercent > b.gpuUsagePercent;});
    }
}

void TabProcesses::sortByVideoRamUsagePercent()
{
    if(m_sortAscending)
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.videoRamUsagePercent < b.videoRamUsagePercent;});
    }
    else
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.videoRamUsagePercent > b.videoRamUsagePercent;});
    }
}

void TabProcesses::sortByRamUsageSize()
{
    if(m_sortAscending)
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.ramUsageSize < b.ramUsageSize;});
    }
    else
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.ramUsageSize > b.ramUsageSize;});
    }
}

void TabProcesses::sortByVirtualRamUsageSize()
{
    if(m_sortAscending)
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.virtualRamUsageSize < b.virtualRamUsageSize;});
    }
    else
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.virtualRamUsageSize > b.virtualRamUsageSize;});
    }
}

void TabProcesses::sortByVideoRamUsageSize()
{
    if(m_sortAscending)
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.videoRamUsageSize < b.videoRamUsageSize;});
    }
    else
    {
        std::sort(m_processList.begin(),m_processList.end(), [](const ProcessInfo::Process& a, const ProcessInfo::Process& b){return a.videoRamUsageSize > b.videoRamUsageSize;});
    }
}
