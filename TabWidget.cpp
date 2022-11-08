#include "TabWidget.h"

TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent)
{
    processData = new ProcessDatabase();
    tabHardware = new TabHardware(processData, parent);
    tabProcesses = new TabProcesses(processData, parent);
    tabPerformance = new TabPerformance(processData, parent);
    addTab(tabProcesses, QString("Processes"));
    addTab(tabPerformance, QString("Performance"));
    addTab(tabHardware, QString("Hardware"));
}
