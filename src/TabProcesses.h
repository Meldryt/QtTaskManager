#pragma once

#include <QWidget>
#include <QTableWidget>

#include "ProcessInfo.h"

class TabProcesses : public QWidget
{
public:
    TabProcesses(QWidget *parent = nullptr);

    void process();

private:
    void updateTable();
    void updateTotalInfo();

    void sortTable();
    void sortNames();
    void sortFileNames();
    void sortMemoryUsage();
    void sortCpuUsage();
    void setSortMode(int headerIndex);

    enum class SortMode : uint8_t
    {
        SortProcessName = 0,
        SortFileName,
        SortMemoryUsageHigh,
        SortMemoryUsageLow,
        SortCpuUsageHigh,
        SortCpuUsageLow,
        NoSort
    };

    const QStringList TableHeaderNames
    {
        "Name",
        "FileName",
        "FilePath",
        "Memory Usage",
        "Cpu Usage",
        "Disk Usage",
        "Network Usage",
        "Gpu Usage",
    };

    QTableWidget* m_tableProcesses{nullptr};

    std::vector<ProcessInfo::Process> m_processList;

    SortMode m_sortMode{SortMode::NoSort};

public slots:
    void slotProcesses(const std::map<uint32_t, ProcessInfo::Process>& processMap);
};
