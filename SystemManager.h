#pragma once

#include "WorkerProcess.h"
#include "TabHardware.h"
#include "TabPerformance.h"
#include "TabProcesses.h"
#include "SystemManager.h"

#include <QTabWidget>
#include <QThread>

class SystemManager : public QTabWidget
{
    Q_OBJECT
public:
    SystemManager(QWidget* parent);

private:
    TabHardware* tabHardware{nullptr};
    TabProcesses* tabProcesses{nullptr};
    TabPerformance* tabPerformance{nullptr};

    WorkerProcess* workerProcess{nullptr};
    QThread* threadProcesses{nullptr};
};

