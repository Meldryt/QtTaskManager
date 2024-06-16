#include "TabPerformance.h"

#include <QGridLayout>
#include <QPainter>
#include <QPoint>
#include <QHeaderView>

TabPerformance::TabPerformance(QWidget *parent) : QWidget{parent}
{
    listWidget = new QListWidget(this);
    listWidget->addItem("CPU");
    listWidget->addItem("GPU");
    listWidget->addItem("RAM");
    listWidget->addItem("Disk");
    listWidget->addItem("Network");
    listWidget->setFixedWidth(100);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(listWidget,0,0);
    setLayout(layout);

    {
        cpuLineSeries = new QLineSeries();

        cpuChart = new QChart();
        cpuChart->legend()->hide();
        cpuChart->addSeries(cpuLineSeries);
        cpuChart->createDefaultAxes();
        cpuChart->axes(Qt::Horizontal).back()->setRange(0,60);
        cpuChart->axes(Qt::Vertical).back()->setRange(0.0,100.0);
        //chart->setTitle("Simple line chart example");

        cpuChartView = new QChartView(cpuChart);
        cpuChartView->setRenderHint(QPainter::Antialiasing);

        cpuWidget = new QWidget(this);

        QGridLayout *cpuWidgetLayout = new QGridLayout(cpuWidget);
        cpuWidgetLayout->addWidget(cpuChartView,0,0);
        cpuWidget->setLayout(cpuWidgetLayout);
    }

    m_stackedWidget = new QStackedWidget(this);

    initGraphs();

    m_gpuWidget = new QWidget(this);

    m_tableWidget = new QTableWidget(m_gpuWidget);
    m_tableWidget->setRowCount(GpuGraphTitles.size());
    m_tableWidget->setColumnCount(1);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);

    for (int i = 0; i < GpuGraphTitles.size(); ++i)
    {
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(GpuGraphTitles.at(i) + ": "));
    }

    m_comboBoxActiveGraph = new QComboBox(m_gpuWidget);
    m_comboBoxActiveGraph->addItems(GpuGraphTitles);
    m_comboBoxActiveGraph->setCurrentIndex(0);

    QObject::connect(m_comboBoxActiveGraph, &QComboBox::currentIndexChanged,
        m_stackedWidget, &QStackedWidget::setCurrentIndex);

    QGridLayout* gpuWidgetLayout = new QGridLayout(m_gpuWidget);
    gpuWidgetLayout->addWidget(m_comboBoxActiveGraph, 0, 0);
    gpuWidgetLayout->addWidget(m_stackedWidget, 1, 0);
    gpuWidgetLayout->addWidget(m_tableWidget, 1, 1);
    gpuWidgetLayout->setRowStretch(0,1);
    gpuWidgetLayout->setRowStretch(1,3);
    m_gpuWidget->setLayout(gpuWidgetLayout);

    layout->addWidget(cpuWidget,0,1);
    layout->addWidget(m_gpuWidget,0,2);

    connect(listWidget,&QListWidget::itemSelectionChanged, this, &TabPerformance::showSelectionWidget);
    listWidget->setCurrentRow(0);

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &TabPerformance::process);
    timer->start();
}

void TabPerformance::initGraphs()
{
    for (int i = 0; i < GpuGraphTitles.size(); ++i)
    {
        m_gpuGraphs[i].gpuLineSeries = new QLineSeries();
        m_gpuGraphs[i].gpuChart = new QChart();
        m_gpuGraphs[i].gpuChart->legend()->hide();
        m_gpuGraphs[i].gpuChart->addSeries(m_gpuGraphs[i].gpuLineSeries);
        m_gpuGraphs[i].gpuChart->createDefaultAxes();

        m_gpuGraphs[i].gpuChartView = new QChartView(m_gpuGraphs[i].gpuChart);
        m_gpuGraphs[i].gpuChartView->setRenderHint(QPainter::Antialiasing);

        m_stackedWidget->addWidget(m_gpuGraphs[i].gpuChartView);
    }

    m_stackedWidget->setCurrentIndex(0);

    //m_gpuGraphs[0].gpuChart->setTitle("GraphicsUsage");
    m_gpuGraphs[0].gpuChart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[0].gpuChart->axes(Qt::Vertical).back()->setRange(0, 100);

    //m_gpuGraphs[1].gpuChart->setTitle("GraphicsClock");
    m_gpuGraphs[1].gpuChart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[1].gpuChart->axes(Qt::Vertical).back()->setRange(0, 5000);
    
    //m_gpuGraphs[2].gpuChart->setTitle("MemoryClock");
    m_gpuGraphs[2].gpuChart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[2].gpuChart->axes(Qt::Vertical).back()->setRange(0, 4000);

    //m_gpuGraphs[3].gpuChart->setTitle("AsicPower");
    m_gpuGraphs[3].gpuChart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[3].gpuChart->axes(Qt::Vertical).back()->setRange(0, 600);

    //m_gpuGraphs[4].gpuChart->setTitle("Voltage");
    m_gpuGraphs[4].gpuChart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[4].gpuChart->axes(Qt::Vertical).back()->setRange(0, 2000);

    //m_gpuGraphs[5].gpuChart->setTitle("Temperature");
    m_gpuGraphs[5].gpuChart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[5].gpuChart->axes(Qt::Vertical).back()->setRange(0, 200);

    //m_gpuGraphs[6].gpuChart->setTitle("Temperature(Hotspot)");
    m_gpuGraphs[6].gpuChart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[6].gpuChart->axes(Qt::Vertical).back()->setRange(0, 200);

    //m_gpuGraphs[7].gpuChart->setTitle("FanSpeed");
    m_gpuGraphs[7].gpuChart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_gpuGraphs[7].gpuChart->axes(Qt::Vertical).back()->setRange(0, 10000);
}

void TabPerformance::process()
{
    processCPU();
    processGPU();
}

void TabPerformance::processCPU()
{
    QList<QPointF> points = cpuLineSeries->points();
    if(!points.empty())
    {
        cpuLineSeries->clear();
        for(uint8_t i=0;i<points.size();++i)
        {
            points[i].setX(points.at(i).x()-1);
        }
        if(points.first().x()<0)
        {
            points.removeFirst();
        }
        cpuLineSeries->append(points);
    }

    double x = 60;
    double y = cpuTotalLoad;
    cpuLineSeries->append(x,y);
    cpuChartView->repaint();
}

void TabPerformance::processGPU()
{
    for (int i = 0; i < m_gpuGraphs.size(); ++i)
    {
        QList<QPointF> points = m_gpuGraphs[i].gpuLineSeries->points();
        if (!points.empty())
        {
            m_gpuGraphs[i].gpuLineSeries->clear();
            for (uint8_t i = 0; i < points.size(); ++i)
            {
                points[i].setX(points.at(i).x() - 1);
            }
            if (points.first().x() < 0)
            {
                points.removeFirst();
            }
            m_gpuGraphs[i].gpuLineSeries->append(points);
        }

        double x = 60;
        m_gpuGraphs[i].gpuLineSeries->append(x, m_gpuGraphs[i].currentY);
    }

    m_gpuGraphs[m_comboBoxActiveGraph->currentIndex()].gpuChartView->repaint();

    for (int i = 0; i < m_gpuTableInfos.size(); ++i)
    {
        m_tableWidget->item(i, 0)->setText(m_gpuTableInfos[i]);
    }

    //gpuTempLabel->setText("Temperature: " + QString::number(gpuTemperature) + " °C");
}

void TabPerformance::slotCPUTotalLoad(const double& val)
{
    cpuTotalLoad = val;
}

void TabPerformance::slotGpuDynamicInfo(const Globals::GpuDynamicInfo& dynamicInfo)
{
    int graphIndex = 0;

    m_gpuGraphs[graphIndex++].currentY = dynamicInfo.gpuGraphicsUsage;
    m_gpuGraphs[graphIndex++].currentY = dynamicInfo.gpuGraphicsClock;
    m_gpuGraphs[graphIndex++].currentY = dynamicInfo.gpuMemoryClock;
    m_gpuGraphs[graphIndex++].currentY = dynamicInfo.gpuAsicPower;
    m_gpuGraphs[graphIndex++].currentY = dynamicInfo.gpuGraphicsVoltage;
    m_gpuGraphs[graphIndex++].currentY = dynamicInfo.gpuTemperature;
    m_gpuGraphs[graphIndex++].currentY = dynamicInfo.gpuTemperatureHotspot;
    m_gpuGraphs[graphIndex++].currentY = dynamicInfo.gpuFanSpeed;

    for (int i = 0; i < m_gpuGraphs.size(); ++i)
    {
        m_gpuTableInfos[i] = GpuGraphTitles.at(i) + ": " + QString::number(m_gpuGraphs[i].currentY);
    }
}

void TabPerformance::showSelectionWidget()
{
    cpuWidget->hide();
    m_gpuWidget->hide();

    switch(listWidget->currentRow())
    {
    case 0:
        cpuWidget->show();
        break;
    case 1:
        m_gpuWidget->show();
        break;
    case 2:
        break;
    default:
        break;
    }
}
