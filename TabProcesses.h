#pragma once

#include "ProcessDatabase.h"

#include <QWidget>
#include <QTableWidget>
#include <QTimer>

class TabProcesses : public QWidget
{
public:
    TabProcesses(ProcessDatabase* database, QWidget *parent = nullptr);

private:
    void process();
    void updateTable();
    void updateTotalInfo();

    ProcessDatabase* processDatabase{nullptr};
    QTableWidget* tableProcesses{nullptr};
    QTimer* timer{nullptr};
};
