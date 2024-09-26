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
    void initMemoryGraphs();
    void initNetworkGraphs(const QStringList& list);

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
                this->lineSeries.back()->setUseOpenGL(false);
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
    std::map<int, double> m_cpuDynamicMax;

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
    std::map<int, double> m_gpuDynamicMax;

    const QStringList MemoryGraphTitles
    {
        "Memory Size Usage"
    };

    //memory
    QWidget* m_memoryWidget{ nullptr };
    QTableWidget* m_memoryTableWidget{ nullptr };
    QStackedWidget* m_memoryStackedWidget{ nullptr };
    QGroupBox* m_memoryGroupBoxActiveGraph{ nullptr };
    QLabel* m_memoryLabelActiveGraph{ nullptr };
    QComboBox* m_memoryComboBoxActiveGraph{ nullptr };
    std::map<int, GraphInfo*> m_memoryGraphs;
    std::map<int, QString> m_memoryTableInfos;
    std::map<int, double> m_memoryDynamicMax;

    uint32_t m_memoryTotalSize{ 0 };
    uint32_t m_memoryUsedSize{ 0 };

    const QStringList NetworkGraphTitles
    {
        "Network Download",
        "Network Upload",
    };

    const uint8_t MaxNetworkCount = 10;

    //network
    QWidget* m_networkWidget{ nullptr };
    QTableWidget* m_networkTableWidget{ nullptr };
    QStackedWidget* m_networkStackedWidget{ nullptr };
    //QGroupBox* m_networkGroupBoxActiveNetworks{ nullptr };
    QComboBox* m_networkComboBoxActiveNetworks{ nullptr };
    //QGroupBox* m_networkGroupBoxActiveGraph{ nullptr };
    QComboBox* m_networkComboBoxActiveGraph{ nullptr };
    struct NetworkPerfInfo
    {
        std::map<int, GraphInfo*> graph;
        std::map<int, QString> tableInfos;
        std::map<int, double> dynamicMax;
    };
    std::vector<NetworkPerfInfo> m_networkPerfInfos;

public slots:
    void slotCpuDynamicInfo(const QMap<uint8_t, QVariant>& dynamicInfo);
    void slotGpuDynamicInfo(const QMap<uint8_t, QVariant>& dynamicInfo);
    void slotTotalMemory(const uint32_t& val);
    void slotUsedMemory(const uint32_t& val);
    void slotNetworkDynamicInfo(const QMap<uint8_t, QVariant>& dynamicInfo);
    void slotActiveNetworkChanged(const int& index);

private slots:
    void showSelectionWidget();
};

