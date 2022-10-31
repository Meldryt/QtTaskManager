#pragma once

#include "ProcessDatabase.h"

#include <QWidget>
#include <QListWidget>
//#include <QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QTimer>
#include <QLabel>

class TabPerformance : public QWidget
{
public:
    explicit TabPerformance(ProcessDatabase* database, QWidget *parent = nullptr);

private:
    void process();
    void processCPU();
    void processGPU();

    ProcessDatabase* processDatabase{nullptr};
    QListWidget* listWidget{nullptr};

    QWidget* cpuWidget{nullptr};
    QChartView* cpuChartView{nullptr};
    QChart* cpuChart{nullptr};
    QLineSeries* cpuLineSeries{nullptr};

    QWidget* gpuWidget{nullptr};
    QChartView* gpuChartView{nullptr};
    QChart* gpuChart{nullptr};
    QLineSeries* gpuLineSeries{nullptr};
    QLabel* gpuTempLabel{nullptr};

    QTimer* timer{nullptr};
    uint8_t count{60};

private slots:
    void showSelectionWidget();
};

