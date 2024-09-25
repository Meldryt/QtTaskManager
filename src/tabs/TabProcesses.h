#pragma once

#include <QWidget>
#include <QTableWidget>

#include "../process/ProcessInfo.h"

#include <map>

class TabProcesses : public QWidget
{
public:
    TabProcesses(QWidget *parent = nullptr);
    ~TabProcesses();

    void process();

private:
    void updateTable();
    void updateTotalInfo();

    void sortProcessList();
    void sortByNames();
    void sortByIDs();
    void sortByDescriptions();
    void sortByFilePath();
    void sortByRamUsagePercent();
    void sortByCpuUsagePercent();
    void sortByGpuUsagePercent();
    void sortByVideoRamUsagePercent();
    void sortByRamUsageSize();
    void sortByVirtualRamUsageSize();
    void sortByVideoRamUsageSize();
    void setSortMode(int headerIndex);

    enum class SortMode : uint8_t
    {
        SortByProcessName = 0,
        SortByID,
        SortByDescription,
        SortByFilePath,
        SortByRamUsagePercent,
        SortByCpuUsagePercent,
        SortByGpuUsagePercent,
        SortByVideoRamUsagePercent,
        SortByRamUsageSize,
        SortByVirtualRamUsageSize,
        SortByVideoRamUsageSize,
        NoSort
    };

    enum class ColumnType : uint8_t
    {
        ProcessName = 0,
        ID,
        Description,
        FilePath,
        RamUsagePercent,
        CpuUsagePercent,
        GpuUsagePercent,
        VideoRamUsagePercent,
        RamUsageSize,
        VirtualRamUsageSize,
        VideoRamUsageSize,
        DiskUsage,
        NetworkUsage
    };

    const QStringList TableHeaderNames
    {
        "Name",
        "ID",
        "Description",
        "FilePath",
        "Ram %",
        "Cpu %",
        "Gpu %",
        "VRam %",
        "Ram (MB)",
        "VirtualRam (MB)",
        "VideoRam (MB)",
        "Disk %",
        "Network %",
    };

    const std::map<SortMode, bool> DefaultAscendingSortMode = {
        {SortMode::SortByProcessName, true},
        {SortMode::SortByID, true},
        {SortMode::SortByDescription, true},
        {SortMode::SortByFilePath, true},
        {SortMode::SortByRamUsagePercent, false},
        {SortMode::SortByCpuUsagePercent, false},
        {SortMode::SortByGpuUsagePercent, false},
        {SortMode::SortByVideoRamUsagePercent, false},
        {SortMode::SortByRamUsageSize, false},
        {SortMode::SortByVirtualRamUsageSize, false},
        {SortMode::SortByVideoRamUsageSize, false},
        {SortMode::NoSort, true}
    };

    QTableWidget* m_tableProcesses{nullptr};

    std::vector<ProcessInfo::Process> m_processList;

    SortMode m_sortMode{SortMode::SortByProcessName};
    bool m_sortAscending{true};

public slots:
    void slotProcessDynamicInfo(const QMap<uint8_t, QVariant>& dynamicInfo);
};
