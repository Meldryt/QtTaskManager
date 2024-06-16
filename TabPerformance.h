#pragma once

#include <QWidget>
#include <QListWidget>
//#include <QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QTimer>
#include <QLabel>
#include <QComboBox>
#include <QTableWidget>
#include <QStackedWidget>

#include <Globals.h>

class TabPerformance : public QWidget
{
public:
    explicit TabPerformance(QWidget *parent = nullptr);

private:
    void initGraphs();

    void process();
    void processCPU();
    void processGPU();

    QListWidget* listWidget{nullptr};

    QWidget* cpuWidget{nullptr};
    QChartView* cpuChartView{nullptr};
    QChart* cpuChart{nullptr};
    QLineSeries* cpuLineSeries{nullptr};

    QWidget* m_gpuWidget{nullptr};
    QTableWidget* m_tableWidget{ nullptr };
    QStackedWidget* m_stackedWidget{ nullptr };

    struct GraphInfo
    {
        QChartView* gpuChartView{ nullptr };
        QChart* gpuChart{ nullptr };
        QLineSeries* gpuLineSeries{ nullptr };
        int currentY;
    };

    std::map<int, GraphInfo> m_gpuGraphs;

    QComboBox* m_comboBoxActiveGraph{ nullptr };

    const QStringList GpuGraphTitles
    {
        "GraphicsUsage",
        "GraphicsClock",
        "MemoryClock",
        "AsicPower",
        "Voltage",
        "Temperature",
        "Temperature(Hotspot)",
        "FanSpeed",
    };

    QTimer* timer{nullptr};

    double cpuTotalLoad{0.0};
    std::map<int, QString> m_gpuTableInfos;

public slots:
    void slotGpuDynamicInfo(const Globals::GpuDynamicInfo& dynamicInfo);
    void slotCPUTotalLoad(const double& val);

private slots:
    void showSelectionWidget();
};

