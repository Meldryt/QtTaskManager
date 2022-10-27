#pragma once

#include <QTabWidget>
#include "TabPerformance.h"
#include "TabProcesses.h"
#include "ProcessDatabase.h"

class TabWidget : public QTabWidget
{
public:
    TabWidget(QWidget* parent);

private:
    ProcessDatabase* processData{nullptr};
    TabProcesses* tabProcesses{nullptr};
    TabPerformance* tabPerformance{nullptr};
};
