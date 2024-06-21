#include "TabPerformance.h"

#include <QGridLayout>
#include <QPainter>
#include <QPoint>
#include <QHeaderView>

TabPerformance::TabPerformance(QWidget *parent) : QWidget{parent}
{
    m_listWidget = new QListWidget(this);
    m_listWidget->addItem("CPU");
    m_listWidget->addItem("GPU");
    m_listWidget->addItem("RAM");
    m_listWidget->addItem("Disk");
    m_listWidget->addItem("Network");
    m_listWidget->setFixedWidth(100);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_listWidget,0,0);
    setLayout(layout);

    {
        m_memoryLineSeries = new QLineSeries();

        m_memoryChart = new QChart();
        m_memoryChart->legend()->hide();
        m_memoryChart->addSeries(m_memoryLineSeries);
        m_memoryChart->createDefaultAxes();
        m_memoryChart->axes(Qt::Horizontal).back()->setRange(0, 60);
        m_memoryChart->axes(Qt::Vertical).back()->setRange(0.0, 100.0);
        //chart->setTitle("Simple line chart example");

        m_memoryChartView = new QChartView(m_memoryChart);
        m_memoryChartView->setRenderHint(QPainter::Antialiasing);

        m_memoryWidget = new QWidget(this);

        QGridLayout* memoryWidgetLayout = new QGridLayout(m_memoryWidget);
        memoryWidgetLayout->addWidget(m_memoryChartView, 0, 0);
        m_memoryWidget->setLayout(memoryWidgetLayout);
    }

    initCpuWidgets();
    initGpuWidgets();

    layout->addWidget(m_cpuWidget, 0, 1);
    layout->addWidget(m_gpuWidget, 0, 2);
    layout->addWidget(m_memoryWidget, 0, 3);

    connect(m_listWidget,&QListWidget::itemSelectionChanged, this, &TabPerformance::showSelectionWidget);
    m_listWidget->setCurrentRow(0);

    showSelectionWidget();

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &TabPerformance::process);
    timer->start();
}

void TabPerformance::initCpuWidgets()
{
    m_cpuStackedWidget = new QStackedWidget(this);
    m_cpuStackedWidget->setCurrentIndex(0);

    m_cpuWidget = new QWidget(this);

    m_cpuTableWidget = new QTableWidget(m_cpuWidget);
    m_cpuTableWidget->setRowCount(CpuGraphTitles.size());
    m_cpuTableWidget->setColumnCount(1);
    m_cpuTableWidget->horizontalHeader()->setStretchLastSection(true);

    for (int i = 0; i < CpuGraphTitles.size(); ++i)
    {
        m_cpuTableWidget->setItem(i, 0, new QTableWidgetItem(CpuGraphTitles.at(i) + ": "));
    }

    m_cpuComboBoxActiveGraph = new QComboBox(m_cpuWidget);
    m_cpuComboBoxActiveGraph->addItems(CpuGraphTitles);
    m_cpuComboBoxActiveGraph->setCurrentIndex(0);

    QObject::connect(m_cpuComboBoxActiveGraph, &QComboBox::currentIndexChanged,
        m_cpuStackedWidget, &QStackedWidget::setCurrentIndex);

    QGridLayout* cpuWidgetLayout = new QGridLayout(m_cpuWidget);
    cpuWidgetLayout->addWidget(m_cpuComboBoxActiveGraph, 0, 0);
    cpuWidgetLayout->addWidget(m_cpuStackedWidget, 1, 0);
    cpuWidgetLayout->addWidget(m_cpuTableWidget, 1, 1);
    cpuWidgetLayout->setRowStretch(0, 1);
    cpuWidgetLayout->setRowStretch(1, 3);
    cpuWidgetLayout->setColumnStretch(0, 2);
    cpuWidgetLayout->setColumnStretch(1, 1);
    m_cpuWidget->setLayout(cpuWidgetLayout);

    initCpuGraphs();
}

void TabPerformance::initGpuWidgets()
{
    m_gpuStackedWidget = new QStackedWidget(this);
    m_gpuStackedWidget->setCurrentIndex(0);

    m_gpuWidget = new QWidget(this);

    m_gpuTableWidget = new QTableWidget(m_gpuWidget);
    m_gpuTableWidget->setRowCount(GpuGraphTitles.size());
    m_gpuTableWidget->setColumnCount(1);
    m_gpuTableWidget->horizontalHeader()->setStretchLastSection(true);

    for (int i = 0; i < GpuGraphTitles.size(); ++i)
    {
        m_gpuTableWidget->setItem(i, 0, new QTableWidgetItem(GpuGraphTitles.at(i) + ": "));
    }

    m_gpuComboBoxActiveGraph = new QComboBox(m_gpuWidget);
    m_gpuComboBoxActiveGraph->addItems(GpuGraphTitles);
    m_gpuComboBoxActiveGraph->setCurrentIndex(0);

    QObject::connect(m_gpuComboBoxActiveGraph, &QComboBox::currentIndexChanged,
        m_gpuStackedWidget, &QStackedWidget::setCurrentIndex);

    QGridLayout* gpuWidgetLayout = new QGridLayout(m_gpuWidget);
    gpuWidgetLayout->addWidget(m_gpuComboBoxActiveGraph, 0, 0);
    gpuWidgetLayout->addWidget(m_gpuStackedWidget, 1, 0);
    gpuWidgetLayout->addWidget(m_gpuTableWidget, 1, 1);
    gpuWidgetLayout->setRowStretch(0, 1);
    gpuWidgetLayout->setRowStretch(1, 3);
    gpuWidgetLayout->setColumnStretch(0, 2);
    gpuWidgetLayout->setColumnStretch(1, 1);
    m_gpuWidget->setLayout(gpuWidgetLayout);

    initGpuGraphs();
}

void TabPerformance::initCpuGraphs()
{
    for (int i = 0; i < CpuGraphTitles.size(); ++i)
    {
        m_cpuGraphs[i].chart = new QChart();
        m_cpuGraphs[i].chart->legend()->hide();

        if (i < CpuCoreUsagesGraphIndex)
        {
            m_cpuGraphs[i].lineSeries = new QLineSeries();
            m_cpuGraphs[i].chart->addSeries(m_cpuGraphs[i].lineSeries);
            m_cpuGraphs[i].chart->createDefaultAxes();
        }

        m_cpuGraphs[i].chartView = new QChartView(m_cpuGraphs[i].chart);
        m_cpuGraphs[i].chartView->setRenderHint(QPainter::Antialiasing);
        m_cpuStackedWidget->addWidget(m_cpuGraphs[i].chartView);
    }

    int index = 0;

    //CpuUsage
    m_cpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_cpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 100);
    ++index;

    //MaxFrequency
    m_cpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_cpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 5000);
    ++index;

    //CpuPower
    m_cpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_cpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 300);
    ++index;

    //CpuSocPower
    m_cpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_cpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 300);
    ++index;

    //Voltage
    m_cpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_cpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 2000);
    ++index;

    //Temperature
    m_cpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_cpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 200);
    ++index;

    //FanSpeed
    m_cpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_cpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 10000);
}

void TabPerformance::initGpuGraphs()
{
    for (int i = 0; i < GpuGraphTitles.size(); ++i)
    {
        m_gpuGraphs[i].chart = new QChart();
        m_gpuGraphs[i].chart->legend()->hide();

        m_gpuGraphs[i].lineSeries = new QLineSeries();
        m_gpuGraphs[i].chart->addSeries(m_gpuGraphs[i].lineSeries);
        m_gpuGraphs[i].chart->createDefaultAxes();

        m_gpuGraphs[i].chartView = new QChartView(m_gpuGraphs[i].chart);
        m_gpuGraphs[i].chartView->setRenderHint(QPainter::Antialiasing);
        m_gpuStackedWidget->addWidget(m_gpuGraphs[i].chartView);
    }

    int index = 0;

    //GraphicsUsage
    m_gpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 100);
    ++index;

    //GraphicsClock
    m_gpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 5000);
    ++index;

    //MemoryClock
    m_gpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 4000);
    ++index;

    //AsicPower
    m_gpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 600);
    ++index;

    //Voltage
    m_gpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 2000);
    ++index;

    //Temperature
    m_gpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 200);
    ++index;

    //Temperature(Hotspot)
    m_gpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 200);
    ++index;

    //FanSpeed
    m_gpuGraphs[index].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[index].chart->axes(Qt::Vertical).back()->setRange(0, 10000);
}

void TabPerformance::process()
{
    processCpu();
    processGpu();
    processMemory();
}

void TabPerformance::processCpu()
{
    for (int i = 0; i < m_cpuGraphs.size() - 4; ++i)
    {
        QList<QPointF> points = m_cpuGraphs[i].lineSeries->points();
        if (!points.empty())
        {
            m_cpuGraphs[i].lineSeries->clear();
            for (uint8_t j = 0; j < points.size(); ++j)
            {
                points[j].setX(points.at(j).x() - 1);
            }
            if (points.first().x() < 0)
            {
                points.removeFirst();
            }
            m_cpuGraphs[i].lineSeries->append(points);
        }

        double x = 60;
        m_cpuGraphs[i].lineSeries->append(x, m_cpuGraphs[i].currentY);
    }

    for (int multiLineIndex = 0; multiLineIndex < m_cpuMultiLines.size(); ++multiLineIndex)
    {
        for (int lineSeriesIndex = 0; lineSeriesIndex < m_cpuMultiLines[multiLineIndex].size(); ++lineSeriesIndex)
        {
            QLineSeries* lineSeries = m_cpuMultiLines[multiLineIndex][lineSeriesIndex].lineSeries;
            QList<QPointF> points = lineSeries->points();
            if (!points.empty())
            {
                lineSeries->clear();
                for (uint8_t j = 0; j < points.size(); ++j)
                {
                    points[j].setX(points.at(j).x() - 1);
                }
                if (points.first().x() < 0)
                {
                    points.removeFirst();
                }
                lineSeries->append(points);
            }

            double x = 60;
            int currentY = m_cpuMultiLines[multiLineIndex][lineSeriesIndex].currentY;
            lineSeries->append(x, currentY);
        }
    }

    m_cpuGraphs[m_cpuComboBoxActiveGraph->currentIndex()].chartView->repaint();

    for (int i = 0; i < m_cpuTableInfos.size(); ++i)
    {
        m_cpuTableWidget->item(i, 0)->setText(m_cpuTableInfos[i]);
    }
}

void TabPerformance::processGpu()
{
    for (int i = 0; i < m_gpuGraphs.size(); ++i)
    {
        QList<QPointF> points = m_gpuGraphs[i].lineSeries->points();
        if (!points.empty())
        {
            m_gpuGraphs[i].lineSeries->clear();
            for (uint8_t j = 0; j < points.size(); ++j)
            {
                points[j].setX(points.at(j).x() - 1);
            }
            if (points.first().x() < 0)
            {
                points.removeFirst();
            }
            m_gpuGraphs[i].lineSeries->append(points);
        }

        double x = 60;
        m_gpuGraphs[i].lineSeries->append(x, m_gpuGraphs[i].currentY);
    }

    m_gpuGraphs[m_gpuComboBoxActiveGraph->currentIndex()].chartView->repaint();

    for (int i = 0; i < m_gpuTableInfos.size(); ++i)
    {
        m_gpuTableWidget->item(i, 0)->setText(m_gpuTableInfos[i]);
    }
}

void TabPerformance::processMemory()
{
    QList<QPointF> points = m_memoryLineSeries->points();
    if (!points.empty())
    {
        m_memoryLineSeries->clear();
        for (uint8_t i = 0; i < points.size(); ++i)
        {
            points[i].setX(points.at(i).x() - 1);
        }
        if (points.first().x() < 0)
        {
            points.removeFirst();
        }
        m_memoryLineSeries->append(points);
    }

    double x = 60;
    double y = m_memoryUsedSize;
    m_memoryLineSeries->append(x, y);
    m_memoryChartView->repaint();
}

void TabPerformance::slotCpuDynamicInfo(const Globals::CpuDynamicInfo& dynamicInfo)
{
    int index = 0;

    m_cpuGraphs[index++].currentY = dynamicInfo.cpuTotalUsage;
    m_cpuGraphs[index++].currentY = dynamicInfo.cpuMaxFrequency;
    m_cpuGraphs[index++].currentY = dynamicInfo.cpuPower;
    m_cpuGraphs[index++].currentY = dynamicInfo.cpuSocPower;
    m_cpuGraphs[index++].currentY = 0;// dynamicInfo.cpuVoltage;
    m_cpuGraphs[index++].currentY = dynamicInfo.cpuTemperature;
    m_cpuGraphs[index++].currentY = 0;// dynamicInfo.cpuFanSpeed;
    m_cpuGraphs[index++].currentY = 0;// dynamicInfo.cpuCoreUsages
    m_cpuGraphs[index++].currentY = 0;// dynamicInfo.cpuCoreFrequencies
    m_cpuGraphs[index++].currentY = 0;// dynamicInfo.cpuThreadUsages
    m_cpuGraphs[index++].currentY = 0;// dynamicInfo.cpuThreadFrequencies

    for (int i = 0; i < m_cpuGraphs.size(); ++i)
    {
        m_cpuTableInfos[i] = CpuGraphTitles.at(i) + ": " + QString::number(m_cpuGraphs[i].currentY);
    }

    if (m_cpuMultiLines.empty())
    {
        m_cpuMultiLines.resize(4);

        for (int multiLineIndex = 0; multiLineIndex < m_cpuMultiLines.size(); ++multiLineIndex)
        {
            if (m_cpuMultiLines[multiLineIndex].empty())
            {
                QChart* chart = m_cpuGraphs[CpuCoreUsagesGraphIndex + multiLineIndex].chart;
                chart->removeAllSeries();
            }
        }

        for (int i = 0; i < dynamicInfo.cpuCoreUsages.size(); ++i)
        {
            LineSeriesInfo lineSeriesInfo;
            lineSeriesInfo.lineSeries = new QLineSeries();
            lineSeriesInfo.currentY = 0;
            m_cpuGraphs[CpuCoreUsagesGraphIndex].chart->addSeries(lineSeriesInfo.lineSeries);
            m_cpuMultiLines[0].push_back(lineSeriesInfo);
        }

        for (int i = 0; i < dynamicInfo.cpuCoreFrequencies.size(); ++i)
        {
            LineSeriesInfo lineSeriesInfo;
            lineSeriesInfo.lineSeries = new QLineSeries();
            lineSeriesInfo.currentY = 0;
            m_cpuGraphs[CpuCoreFrequenciesGraphIndex].chart->addSeries(lineSeriesInfo.lineSeries);
            m_cpuMultiLines[1].push_back(lineSeriesInfo);
        }

        for (int i = 0; i < dynamicInfo.cpuThreadUsages.size(); ++i)
        {
            LineSeriesInfo lineSeriesInfo;
            lineSeriesInfo.lineSeries = new QLineSeries();
            lineSeriesInfo.currentY = 0;
            m_cpuGraphs[CpuThreadUsagesGraphIndex].chart->addSeries(lineSeriesInfo.lineSeries);
            m_cpuMultiLines[2].push_back(lineSeriesInfo);
        }

        for (int i = 0; i < dynamicInfo.cpuThreadFrequencies.size(); ++i)
        {
            LineSeriesInfo lineSeriesInfo;
            lineSeriesInfo.lineSeries = new QLineSeries();
            lineSeriesInfo.currentY = 0;
            m_cpuGraphs[CpuThreadFrequenciesGraphIndex].chart->addSeries(lineSeriesInfo.lineSeries);
            m_cpuMultiLines[3].push_back(lineSeriesInfo);
        }
        m_cpuGraphs[CpuCoreUsagesGraphIndex].chart->createDefaultAxes();
        m_cpuGraphs[CpuCoreFrequenciesGraphIndex].chart->createDefaultAxes();
        m_cpuGraphs[CpuThreadUsagesGraphIndex].chart->createDefaultAxes();
        m_cpuGraphs[CpuThreadFrequenciesGraphIndex].chart->createDefaultAxes();

        m_cpuGraphs[CpuCoreUsagesGraphIndex].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
        m_cpuGraphs[CpuCoreUsagesGraphIndex].chart->axes(Qt::Vertical).back()->setRange(0, 100);

        m_cpuGraphs[CpuCoreFrequenciesGraphIndex].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
        m_cpuGraphs[CpuCoreFrequenciesGraphIndex].chart->axes(Qt::Vertical).back()->setRange(0, 5000);

        m_cpuGraphs[CpuThreadUsagesGraphIndex].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
        m_cpuGraphs[CpuThreadUsagesGraphIndex].chart->axes(Qt::Vertical).back()->setRange(0, 100);

        m_cpuGraphs[CpuThreadFrequenciesGraphIndex].chart->axes(Qt::Horizontal).back()->setRange(0, 60);
        m_cpuGraphs[CpuThreadFrequenciesGraphIndex].chart->axes(Qt::Vertical).back()->setRange(0, 5000);
    }


    for (int i = 0; i < m_cpuMultiLines[0].size(); ++i)
    {
        m_cpuMultiLines[0][i].currentY = dynamicInfo.cpuCoreUsages[i];
    }

    for (int i = 0; i < m_cpuMultiLines[1].size(); ++i)
    {
        m_cpuMultiLines[1][i].currentY = dynamicInfo.cpuCoreFrequencies[i];
    }

    for (int i = 0; i < m_cpuMultiLines[2].size(); ++i)
    {
        m_cpuMultiLines[2][i].currentY = dynamicInfo.cpuThreadUsages[i];
    }

    for (int i = 0; i < m_cpuMultiLines[3].size(); ++i)
    {
        m_cpuMultiLines[3][i].currentY = dynamicInfo.cpuThreadFrequencies[i];
    }
}

void TabPerformance::slotGpuDynamicInfo(const Globals::GpuDynamicInfo& dynamicInfo)
{
    int index = 0;

    m_gpuGraphs[index++].currentY = dynamicInfo.gpuGraphicsUsage;
    m_gpuGraphs[index++].currentY = dynamicInfo.gpuGraphicsClock;
    m_gpuGraphs[index++].currentY = dynamicInfo.gpuMemoryClock;
    m_gpuGraphs[index++].currentY = dynamicInfo.gpuAsicPower;
    m_gpuGraphs[index++].currentY = dynamicInfo.gpuGraphicsVoltage;
    m_gpuGraphs[index++].currentY = dynamicInfo.gpuTemperature;
    m_gpuGraphs[index++].currentY = dynamicInfo.gpuTemperatureHotspot;
    m_gpuGraphs[index++].currentY = dynamicInfo.gpuFanSpeed;

    for (int i = 0; i < m_gpuGraphs.size(); ++i)
    {
        m_gpuTableInfos[i] = GpuGraphTitles.at(i) + ": " + QString::number(m_gpuGraphs[i].currentY);
    }
}

void TabPerformance::slotTotalMemory(const uint32_t& val)
{
    m_memoryTotalSize = val;
    m_memoryChart->axes(Qt::Vertical).back()->setRange(0.0, m_memoryTotalSize);
}

void TabPerformance::slotUsedMemory(const uint32_t& val)
{
    m_memoryUsedSize = val;
}

void TabPerformance::showSelectionWidget()
{
    m_cpuWidget->hide();
    m_gpuWidget->hide();
    m_memoryWidget->hide();

    switch(m_listWidget->currentRow())
    {
    case 0:
        m_cpuWidget->show();
        break;
    case 1:
        m_gpuWidget->show();
        break;
    case 2:
        m_memoryWidget->show();
        break;
    default:
        break;
    }
}
