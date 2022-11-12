#pragma once

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
    explicit TabPerformance(QWidget *parent = nullptr);

private:
    void process();
    void processCPU();
    void processGPU();

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

    double cpuTotalLoad{0.0};
    uint8_t gpuTotalLoad{0};
    uint8_t gpuTemperature{0};

public slots:
    void slotCPUTotalLoad(const double& val);
    void slotGPUTotalLoad(const uint8_t& val);
    void slotGPUTemperature(const uint8_t& val);

private slots:
    void showSelectionWidget();
};

