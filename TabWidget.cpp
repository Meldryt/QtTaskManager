#include "TabWidget.h"

TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent)
{
    processData = new ProcessDatabase();
    tabProcesses = new TabProcesses(processData, parent);
    addTab(tabProcesses, QString("Processes"));
    tabPerformance = new TabPerformance(processData, parent);
    addTab(tabPerformance, QString("Performance"));
}
