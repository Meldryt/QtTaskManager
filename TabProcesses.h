#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QThread>
#include "WorkerProcess.h"

class TabProcesses : public QWidget
{
public:
    TabProcesses(QWidget *parent = nullptr);

private:

    QTableWidget* tableProcesses;
    QThread* threadProcesses;
    WorkerProcess* workerProcess;

private slots:
    void updateList();

    std::map<QString, int> processItems;
};
