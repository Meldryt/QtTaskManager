#pragma once

#include <QWidget>
#include <QListWidget>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QLabel>
#include <QComboBox>
#include <QTableWidget>
#include <QStackedWidget>
#include <QGroupBox>
#include <QGraphicsLayout>

#include "../Globals.h"

class TabPerformance : public QWidget
{
public:
    explicit TabPerformance(QWidget *parent = nullptr);
    ~TabPerformance();

    void process();

private:
    void initCpuWidgets();
    void initGpuWidgets();
    void initMemoryWidgets();
    void initNetworkWidgets();

    void initCpuGraphs();
    void initGpuGraphs();

    void updateCpuMultiGraphs(const QMap<uint8_t, QVariant>& dynamicInfo);

    void processCpu();
    void processGpu();
    void processMemory();
    void processNetwork();

    struct GraphInfo
    {
        QChartView* chartView{ nullptr };
        QChart* chart{ nullptr };
        std::vector<QLineSeries*> lineSeries;
        std::vector<QVector<QPointF>> points;

        std::vector<double> values{ 0.0 };
        
        QString unit{ "" };
        int axisMin{ 0 };
        int axisMax{ 100 };
        bool multiLines{ false };

        GraphInfo()
        {
            this->chart = new QChart();
            this->chart->legend()->hide();
            this->chart->layout()->setContentsMargins(2, 2, 2, 2);

            this->chartView = new QChartView(chart);
            this->chartView->setRenderHint(QPainter::Antialiasing);
        }

        GraphInfo(QString _unit, int _axisMin, int _axisMax, bool _multiLines = false) : GraphInfo()
        {
            this->unit = _unit;
            this->axisMin = _axisMin;
            this->axisMax = _axisMax;
            this->multiLines = _multiLines;

            if (!multiLines)
            {
                this->lineSeries.push_back(new QLineSeries());
                this->lineSeries.back()->setUseOpenGL(true);
                this->chart->addSeries(this->lineSeries.at(0));
                this->chart->createDefaultAxes();

                this->chart->axes(Qt::Horizontal).back()->setRange(0, axisMin);
                this->chart->axes(Qt::Vertical).back()->setRange(0, axisMax);

                this->values.resize(1);

                this->points.push_back(QVector<QPointF>());
            }
        }
    };

    QListWidget* m_listWidget{nullptr};

    //cpu
    QWidget* m_cpuWidget{nullptr};
    QTableWidget* m_cpuTableWidget{ nullptr };
    QStackedWidget* m_cpuStackedWidget{ nullptr };
    QGroupBox* m_cpuGroupBoxActiveGraph{ nullptr };
    QLabel* m_cpuLabelActiveGraph{ nullptr };
    QComboBox* m_cpuComboBoxActiveGraph{ nullptr };
    std::map<int, GraphInfo*> m_cpuGraphs;
    std::map<int, QString> m_cpuTableInfos;
    
    const QStringList CpuGraphTitles
    {
        "CpuUsage",
        "CurrentMaxFrequency",
        "CpuPower",
        "CpuSocPower",
        "CoreVoltage",
        "Temperature",
        "FanSpeed",
        "CoreUsages",
        "CoreFrequencies",
        "ThreadUsages",
        "ThreadFrequencies",
    };

    const QStringList GpuGraphTitles
    {
        "GPU Usage",
        "VRAM Usage",
        "GPU Clockspeed",
        "VRAM Clockspeed",
        "VRAM Used",
        "GPU Power",
        "TotalBoardPower",
        "GPU Voltage",
        "Temperature",
        "Hotspot Temperature",
        "Fan Speed",
        "Fan Usage",
    };

    const uint8_t CpuMultiGraphsStartIndex = 7;

    //gpu
    QWidget* m_gpuWidget{nullptr};
    QTableWidget* m_gpuTableWidget{ nullptr };
    QStackedWidget* m_gpuStackedWidget{ nullptr };
    QGroupBox* m_gpuGroupBoxActiveGraph{ nullptr };
    QLabel* m_gpuLabelActiveGraph{ nullptr };
    QComboBox* m_gpuComboBoxActiveGraph{ nullptr };
    std::map<int, GraphInfo*> m_gpuGraphs;
    std::map<int, QString> m_gpuTableInfos;

    double m_cpuTotalLoad{0.0};

    //memory
    QWidget* m_memoryWidget{ nullptr };
    QChartView* m_memoryChartView{ nullptr };
    QChart* m_memoryChart{ nullptr };
    QLineSeries* m_memoryLineSeries{ nullptr };

    uint32_t m_memoryTotalSize{ 0 };
    uint32_t m_memoryUsedSize{ 0 };

    //network
    QWidget* m_networkWidget{ nullptr };
    QChartView* m_networkChartView{ nullptr };
    QChart* m_networkChart{ nullptr };
    QLineSeries* m_networkLineSeries{ nullptr };

    uint32_t m_networkTotalSpeed{ 0 };
    uint32_t m_networkUsedSpeed{ 0 };
    uint32_t m_networkCurrentMaxSpeed{ 0 };

public slots:
    void slotCpuDynamicInfo(const QMap<uint8_t, QVariant>& dynamicInfo);
    void slotGpuDynamicInfo(const QMap<uint8_t, QVariant>& dynamicInfo);
    void slotTotalMemory(const uint32_t& val);
    void slotUsedMemory(const uint32_t& val);
    void slotTotalNetworkSpeed(const uint32_t& val);
    void slotUsedNetworkSpeed(const uint32_t& val);

private slots:
    void showSelectionWidget();
};

