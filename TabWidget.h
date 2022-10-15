#pragma once

#include <QTabWidget>
#include "TabApps.h"
#include "TabProcesses.h"

class TabWidget : public QTabWidget
{
public:
    TabWidget(QWidget* parent);
private:
    TabProcesses* tabProcesses;
    TabApps* tabApps;
};
