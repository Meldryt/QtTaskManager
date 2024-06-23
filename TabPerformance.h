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
#include <QGroupBox>
#include <QGraphicsLayout>

#include <Globals.h>

class TabPerformance : public QWidget
{
public:
    explicit TabPerformance(QWidget *parent = nullptr);

private:
    void initCpuWidgets();
    void initGpuWidgets();
    void initMemoryWidgets();
    void initCpuGraphs();
    void initGpuGraphs();

    void updateCpuMultiGraphs(const Globals::CpuDynamicInfo& dynamicInfo);

    void process();
    void processCpu();
    void processGpu();
    void processMemory();

    struct GraphInfo
    {
        QChartView* chartView{ nullptr };
        QChart* chart{ nullptr };
        std::vector<QLineSeries*> lineSeries;

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
                this->chart->addSeries(this->lineSeries.at(0));
                this->chart->createDefaultAxes();

                this->chart->axes(Qt::Horizontal).back()->setRange(0, axisMin);
                this->chart->axes(Qt::Vertical).back()->setRange(0, axisMax);

                this->values.resize(1);
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
        "MaxFrequency",
        "CpuPower",
        "CpuSocPower",
        "Voltage",
        "Temperature",
        "FanSpeed",
        "CoreUsages",
        "CoreFrequencies",
        "ThreadUsages",
        "ThreadFrequencies",
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

    const QStringList GpuGraphTitles
    {
        "GraphicsUsage",
        "GraphicsClock",
        "MemoryClock",
        "AsicPower",
        "Voltage",
        "Temperature",
        "HotspotTemperature",
        "FanSpeed",
    };

    QTimer* timer{nullptr};

    double m_cpuTotalLoad{0.0};

    //memory
    QWidget* m_memoryWidget{ nullptr };
    QChartView* m_memoryChartView{ nullptr };
    QChart* m_memoryChart{ nullptr };
    QLineSeries* m_memoryLineSeries{ nullptr };

    uint32_t m_memoryTotalSize{ 0 };
    uint32_t m_memoryUsedSize{ 0 };  

public slots:
    void slotCpuDynamicInfo(const Globals::CpuDynamicInfo& dynamicInfo);
    void slotGpuDynamicInfo(const Globals::GpuDynamicInfo& dynamicInfo);
    void slotTotalMemory(const uint32_t& val);
    void slotUsedMemory(const uint32_t& val);

private slots:
    void showSelectionWidget();
};

