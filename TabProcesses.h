#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QTimer>

#include "ProcessInfo.h"

class TabProcesses : public QWidget
{
public:
    TabProcesses(QWidget *parent = nullptr);

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

private:
    void process();
    void updateTable();
    void updateTotalInfo();

    void sortTable();
    void sortNames();
    void sortFileNames();
    void sortMemoryUsage();
    void sortCpuUsage();
    void setSortMode(int headerIndex);

    QTableWidget* tableProcesses{nullptr};
    QTimer* timer{nullptr};

    std::vector<ProcessInfo::Process> processList;

    SortMode sortMode{SortMode::NoSort};

public slots:
    void slotProcesses(const std::map<uint32_t, ProcessInfo::Process>& processMap);
};
