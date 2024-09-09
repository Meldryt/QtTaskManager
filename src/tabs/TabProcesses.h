#pragma once

#include <QWidget>
#include <QTableWidget>

#include "../process/ProcessInfo.h"

class TabProcesses : public QWidget
{
public:
    TabProcesses(QWidget *parent = nullptr);
    ~TabProcesses();

    void process();

private:
    void updateTable();
    void updateTotalInfo();

    void sortTable();
    void sortNames();
    void sortFileNames();
    void sortMemoryUsage();
    void sortCpuUsage();
    void sortGpuUsage();
    void sortGpuMemoryUsed();
    void setSortMode(int headerIndex);

    enum class SortMode : uint8_t
    {
        SortProcessName = 0,
        SortFileName,
        SortPhysicalMemoryUsageHigh,
        SortPhysicalMemoryUsageLow,
        SortCpuUsageHigh,
        SortCpuUsageLow,
        SortGpuUsageHigh,
        SortGpuUsageLow,
        SortGpuMemoryUsedHigh,
        SortGpuMemoryUsedLow,
        NoSort
    };

    enum class ColumnType : uint8_t
    {
        ProcessName = 0,
        FileName,
        FilePath,
        MemoryUsage,
        CpuUsage,
        GpuUsage,
        GpuMemoryUsed,
        DiskUsage,
        NetworkUsage
    };

    const QStringList TableHeaderNames
    {
        "Name",
        "FileName",
        "FilePath",
        "Memory Usage",
        "Cpu Usage",
        "Gpu Usage",
        "Gpu Memory Used",
        "Disk Usage",
        "Network Usage",
    };

    QTableWidget* m_tableProcesses{nullptr};

    std::vector<ProcessInfo::Process> m_processList;

    SortMode m_sortMode{SortMode::NoSort};

public slots:
    void slotProcesses(const std::map<uint32_t, ProcessInfo::Process>& processMap);
};
