#pragma once

#include "ProcessDatabase.h"

#include <QWidget>
#include <QListWidget>
//#include <QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QTimer>

class TabPerformance : public QWidget
{
public:
    explicit TabPerformance(ProcessDatabase* database, QWidget *parent = nullptr);

private:
    void process();

    ProcessDatabase* processDatabase{nullptr};
    QListWidget* listWidget{nullptr};
    QWidget* graphWidget{nullptr};
    QLineSeries* lineSeries{nullptr};
    QChart* chart{nullptr};
    QChartView*
    chartView{nullptr};
    QTimer* timer{nullptr};
    uint8_t count{60};
};

