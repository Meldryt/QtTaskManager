#include "TabPerformance.h"

#include <QGridLayout>
#include <QPainter>
#include <QPoint>

TabPerformance::TabPerformance(ProcessDatabase* database, QWidget *parent)
    : QWidget{parent}, processDatabase(database)
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

    {
        gpuLineSeries = new QLineSeries();

        gpuChart = new QChart();
        gpuChart->legend()->hide();
        gpuChart->addSeries(gpuLineSeries);
        gpuChart->createDefaultAxes();
        gpuChart->axes(Qt::Horizontal).back()->setRange(0,60);
        gpuChart->axes(Qt::Vertical).back()->setRange(0,100);
        //chart->setTitle("Simple line chart example");

        gpuChartView = new QChartView(gpuChart);
        gpuChartView->setRenderHint(QPainter::Antialiasing);

        gpuWidget = new QWidget(this);
        gpuTempLabel= new QLabel(gpuWidget);

        QGridLayout *gpuWidgetLayout = new QGridLayout(gpuWidget);
        gpuWidgetLayout->addWidget(gpuChartView,0,0);
        gpuWidgetLayout->addWidget(gpuTempLabel,0,1);
        gpuWidget->setLayout(gpuWidgetLayout);
    }

    layout->addWidget(cpuWidget,0,1);
    layout->addWidget(gpuWidget,0,2);

    connect(listWidget,&QListWidget::itemSelectionChanged, this, &TabPerformance::showSelectionWidget);
    listWidget->setCurrentRow(0);

    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, &TabPerformance::process);
    timer->start();
}

void TabPerformance::process()
{
    if(!processDatabase)
    {
        return;
    }

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
    double y = processDatabase->getTotalInfo().totalCPULoad;
    cpuLineSeries->append(x,y);
    cpuChartView->repaint();
}

void TabPerformance::processGPU()
{
    QList<QPointF> points = gpuLineSeries->points();
    if(!points.empty())
    {
        gpuLineSeries->clear();
        for(uint8_t i=0;i<points.size();++i)
        {
            points[i].setX(points.at(i).x()-1);
        }
        if(points.first().x()<0)
        {
            points.removeFirst();
        }
        gpuLineSeries->append(points);
    }

    double x = 60;
    uint8_t y = processDatabase->getTotalInfo().totalGPULoad;
    gpuLineSeries->append(x,y);
    gpuChartView->repaint();

    uint8_t gpuTemperature = processDatabase->getTotalInfo().gpuTemperature;
    gpuTempLabel->setText("Temperature: " + QString::number(gpuTemperature) + " °C");
}

void TabPerformance::showSelectionWidget()
{
    cpuWidget->hide();
    gpuWidget->hide();

    switch(listWidget->currentRow())
    {
    case 0:
        cpuWidget->show();
        break;
    case 1:
        gpuWidget->show();
        break;
    case 2:
        break;
    default:
        break;
    }
}
