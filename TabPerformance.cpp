#include "TabPerformance.h"

#include <QGridLayout>
#include <QPainter>
#include <QPoint>

TabPerformance::TabPerformance(ProcessDatabase* database, QWidget *parent)
    : processDatabase(database), QWidget{parent}
{
    listWidget = new QListWidget(this);
    listWidget->addItem("CPU");
    listWidget->addItem("GPU");
    listWidget->addItem("RAM");
    listWidget->addItem("Disk");
    listWidget->addItem("Network");
    listWidget->setFixedWidth(300);
    graphWidget = new QWidget(this);
    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(listWidget,0,0);
    //layout->addWidget(graphWidget,0,1);

    listWidget->item(0)->setSelected(true);

    lineSeries = new QLineSeries();

    chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(lineSeries);
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).back()->setRange(0,60);
    chart->axes(Qt::Vertical).back()->setRange(0.0,100.0);
    //chart->setTitle("Simple line chart example");

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(chartView,0,1);

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

    QList<QPointF> points = lineSeries->points();
    if(!points.empty())
    {
        lineSeries->clear();
        for(uint8_t i=0;i<points.size();++i)
        {
            points[i].setX(points.at(i).x()-1);
        }
        if(points.first().x()<0)
        {
            points.removeFirst();
        }
        lineSeries->append(points);
    }

    double x = 60;
    double y = processDatabase->getTotalInfo().totalCPULoad;
    lineSeries->append(x,y);

    chartView->repaint();
    //chart->update();
    //chartView->update();
}
