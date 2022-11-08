#pragma once

#include <QTabWidget>
#include "TabHardware.h"
#include "TabPerformance.h"
#include "TabProcesses.h"
#include "ProcessDatabase.h"

class TabWidget : public QTabWidget
{
public:
    TabWidget(QWidget* parent);

private:
    ProcessDatabase* processData{nullptr};
    TabHardware* tabHardware{nullptr};
    TabProcesses* tabProcesses{nullptr};
    TabPerformance* tabPerformance{nullptr};
};
