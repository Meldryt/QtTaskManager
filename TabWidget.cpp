#include "TabWidget.h"

TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent)
{
    tabProcesses = new TabProcesses(parent);
    addTab(tabProcesses, QString("Processes"));
    tabApps = new TabApps(parent);
    addTab(tabApps, QString("Apps"));
}
