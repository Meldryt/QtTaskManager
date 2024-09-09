#include "TabPerformance.h"

#include <QGridLayout>
#include <QPainter>
#include <QPoint>
#include <QHeaderView>
#include <QGraphicsLayout>
#include <QElapsedTimer>

TabPerformance::TabPerformance(QWidget *parent) : QWidget{parent}
{
    qDebug() << __FUNCTION__;

    m_listWidget = new QListWidget(this);
    m_listWidget->addItem("CPU");
    m_listWidget->addItem("GPU");
    m_listWidget->addItem("RAM");
    m_listWidget->addItem("Network");
    m_listWidget->setFixedWidth(70);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(m_listWidget,0,0);
    setLayout(layout);

    initCpuWidgets();
    initGpuWidgets();
    initMemoryWidgets();
    initNetworkWidgets();

    layout->addWidget(m_cpuWidget, 0, 1);
    layout->addWidget(m_gpuWidget, 0, 2);
    layout->addWidget(m_memoryWidget, 0, 3);
    layout->addWidget(m_networkWidget, 0, 4);

    connect(m_listWidget,&QListWidget::itemSelectionChanged, this, &TabPerformance::showSelectionWidget);
    m_listWidget->setCurrentRow(0);

    showSelectionWidget();
}

TabPerformance::~TabPerformance()
{
    qDebug() << __FUNCTION__;
}

void TabPerformance::initCpuWidgets()
{
    m_cpuStackedWidget = new QStackedWidget(this);
    m_cpuStackedWidget->setCurrentIndex(0);

    m_cpuWidget = new QWidget(this);

    m_cpuTableWidget = new QTableWidget(m_cpuWidget);
    m_cpuTableWidget->setRowCount(CpuGraphTitles.size());
    m_cpuTableWidget->setColumnCount(2);
    m_cpuTableWidget->horizontalHeader()->setStretchLastSection(true);
    m_cpuTableWidget->horizontalHeader()->hide();
    m_cpuTableWidget->verticalHeader()->hide();

    for (int i = 0; i < CpuGraphTitles.size(); ++i)
    {
        m_cpuTableWidget->setItem(i, 0, new QTableWidgetItem(CpuGraphTitles.at(i)));
        m_cpuTableWidget->setItem(i, 1, new QTableWidgetItem());
    }

    m_cpuGroupBoxActiveGraph = new QGroupBox(m_cpuWidget);
    m_cpuLabelActiveGraph = new QLabel("Active Graph:", m_cpuWidget);
    m_cpuComboBoxActiveGraph = new QComboBox(m_cpuWidget);
    m_cpuComboBoxActiveGraph->addItems(CpuGraphTitles);
    m_cpuComboBoxActiveGraph->setCurrentIndex(0);

    QGridLayout* cpuGroupBoxLayout = new QGridLayout(m_cpuWidget);
    cpuGroupBoxLayout->addWidget(m_cpuLabelActiveGraph, 0, 0);
    cpuGroupBoxLayout->addWidget(m_cpuComboBoxActiveGraph, 0, 1);
    m_cpuGroupBoxActiveGraph->setLayout(cpuGroupBoxLayout);

    QObject::connect(m_cpuComboBoxActiveGraph, &QComboBox::currentIndexChanged,
        m_cpuStackedWidget, &QStackedWidget::setCurrentIndex);

    QGridLayout* cpuWidgetLayout = new QGridLayout(m_cpuWidget);
    cpuWidgetLayout->addWidget(m_cpuGroupBoxActiveGraph, 0, 1);
    cpuWidgetLayout->addWidget(m_cpuTableWidget, 1, 0);
    cpuWidgetLayout->addWidget(m_cpuStackedWidget, 1, 1);
    cpuWidgetLayout->setRowStretch(0, 1);
    cpuWidgetLayout->setRowStretch(1, 10);
    cpuWidgetLayout->setColumnStretch(0, 1);
    cpuWidgetLayout->setColumnStretch(1, 2);
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
    m_gpuTableWidget->setColumnCount(2);
    m_gpuTableWidget->horizontalHeader()->setStretchLastSection(true);
    m_gpuTableWidget->horizontalHeader()->hide();
    m_gpuTableWidget->verticalHeader()->hide();

    for (int i = 0; i < GpuGraphTitles.size(); ++i)
    {
        m_gpuTableWidget->setItem(i, 0, new QTableWidgetItem(GpuGraphTitles.at(i)));
        m_gpuTableWidget->setItem(i, 1, new QTableWidgetItem());
    }

    m_gpuGroupBoxActiveGraph = new QGroupBox(m_gpuWidget);
    m_gpuLabelActiveGraph = new QLabel("Active Graph:", m_gpuWidget);
    m_gpuComboBoxActiveGraph = new QComboBox(m_gpuWidget);
    m_gpuComboBoxActiveGraph->addItems(GpuGraphTitles);
    m_gpuComboBoxActiveGraph->setCurrentIndex(0);

    QGridLayout* gpuGroupBoxLayout = new QGridLayout(m_gpuWidget);
    gpuGroupBoxLayout->addWidget(m_gpuLabelActiveGraph, 0, 0);
    gpuGroupBoxLayout->addWidget(m_gpuComboBoxActiveGraph, 0, 1);
    m_gpuGroupBoxActiveGraph->setLayout(gpuGroupBoxLayout);

    QObject::connect(m_gpuComboBoxActiveGraph, &QComboBox::currentIndexChanged,
        m_gpuStackedWidget, &QStackedWidget::setCurrentIndex);

    QGridLayout* gpuWidgetLayout = new QGridLayout(m_gpuWidget);
    gpuWidgetLayout->addWidget(m_gpuGroupBoxActiveGraph, 0, 1);
    gpuWidgetLayout->addWidget(m_gpuTableWidget, 1, 0);
    gpuWidgetLayout->addWidget(m_gpuStackedWidget, 1, 1);
    gpuWidgetLayout->setRowStretch(0, 1);
    gpuWidgetLayout->setRowStretch(1, 10);
    gpuWidgetLayout->setColumnStretch(0, 1);
    gpuWidgetLayout->setColumnStretch(1, 2);
    m_gpuWidget->setLayout(gpuWidgetLayout);

    initGpuGraphs();
}

void TabPerformance::initMemoryWidgets()
{
    m_memoryLineSeries = new QLineSeries();
    m_memoryLineSeries->setUseOpenGL(true);

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

void TabPerformance::initNetworkWidgets()
{
    m_networkLineSeries = new QLineSeries();
    m_networkLineSeries->setUseOpenGL(true);

    m_networkChart = new QChart();
    m_networkChart->legend()->hide();
    m_networkChart->addSeries(m_networkLineSeries);
    m_networkChart->createDefaultAxes();
    m_networkChart->axes(Qt::Horizontal).back()->setRange(0, 60);
    m_networkChart->axes(Qt::Vertical).back()->setRange(0.0, 1000.0);
    //chart->setTitle("Simple line chart example");

    m_networkChartView = new QChartView(m_networkChart);
    m_networkChartView->setRenderHint(QPainter::Antialiasing);

    m_networkWidget = new QWidget(this);

    QGridLayout* networkWidgetLayout = new QGridLayout(m_networkWidget);
    networkWidgetLayout->addWidget(m_networkChartView, 0, 0);
    m_networkWidget->setLayout(networkWidgetLayout);
}

void TabPerformance::initCpuGraphs()
{
    int index = 0;

    m_cpuGraphs[index++] = new GraphInfo(QString("%"), 60, 100); //CpuUsage
    m_cpuGraphs[index++] = new GraphInfo(QString("MHz"), 60, 5000); //MaxFrequency
    m_cpuGraphs[index++] = new GraphInfo(QString("W"), 60, 150); //CpuPower
    m_cpuGraphs[index++] = new GraphInfo(QString("W"), 60, 150); //CpuSocPower
    m_cpuGraphs[index++] = new GraphInfo(QString("mV"), 60, 2000); //Voltage
    m_cpuGraphs[index++] = new GraphInfo(QString::fromLatin1("C"), 60, 150); //Temperature
    m_cpuGraphs[index++] = new GraphInfo(QString("RPM"), 60, 10000); //FanSpeed
    m_cpuGraphs[index++] = new GraphInfo(QString("%"), 60, 100, true); //CoreUsages
    m_cpuGraphs[index++] = new GraphInfo(QString("MHz"), 60, 5000, true); //CoreFrequencies
    m_cpuGraphs[index++] = new GraphInfo(QString("%"), 60, 100, true); //ThreadUsages
    m_cpuGraphs[index++] = new GraphInfo(QString("MHz"), 60, 5000, true); //ThreadFrequencies

    for (int i = 0; i < CpuGraphTitles.size(); ++i)
    {
        m_cpuDynamicMax[i] = 0.0;
        m_cpuStackedWidget->addWidget(m_cpuGraphs[i]->chartView);
    }
}

void TabPerformance::initGpuGraphs()
{
    int index = 0;

    m_gpuGraphs[index++] = new GraphInfo(QString("%"), 60, 100);                //GPU Usage
    m_gpuGraphs[index++] = new GraphInfo(QString("%"), 60, 100);                //VRAM Usage
    m_gpuGraphs[index++] = new GraphInfo(QString("MHz"), 60, 5000);             //GPU Clockspeed
    m_gpuGraphs[index++] = new GraphInfo(QString("MHz"), 60, 4000);             //VRAM Clockspeed
    m_gpuGraphs[index++] = new GraphInfo(QString("MB"), 60, 3000);              //VRAM Used
    m_gpuGraphs[index++] = new GraphInfo(QString("W"), 60, 500);                //GPU Power
    m_gpuGraphs[index++] = new GraphInfo(QString("W"), 60, 500);                //TotalBoardPower
    m_gpuGraphs[index++] = new GraphInfo(QString("mV"), 60, 2000);              //GPU Voltage
    m_gpuGraphs[index++] = new GraphInfo(QString::fromLatin1("C"), 60, 150);   //Temperature
    m_gpuGraphs[index++] = new GraphInfo(QString::fromLatin1("C"), 60, 150);   //Hotspot Temperature
    m_gpuGraphs[index++] = new GraphInfo(QString::fromLatin1("RPM"), 60, 10000);//Fan Speed
    m_gpuGraphs[index++] = new GraphInfo(QString("%"), 60, 100);                //Fan Usage

    for (int i = 0; i < GpuGraphTitles.size(); ++i)
    {
        m_gpuDynamicMax[i] = 0.0;
        m_gpuStackedWidget->addWidget(m_gpuGraphs[i]->chartView);
    }
}

void TabPerformance::process()
{
    setUpdatesEnabled(false);

    processCpu();
    processGpu();
    processMemory();
    processNetwork();

    setUpdatesEnabled(true);
}

void TabPerformance::processCpu()
{
    for (int i = 0; i < m_cpuGraphs.size(); ++i)
    {
        GraphInfo*& graphInfo = m_cpuGraphs[i];
        graphInfo->chartView->setUpdatesEnabled(false);

        for (int lineSeriesIndex = 0; lineSeriesIndex < graphInfo->lineSeries.size(); ++lineSeriesIndex)
        {
            QLineSeries* lineSeries = graphInfo->lineSeries.at(lineSeriesIndex);
            QVector<QPointF>& points = graphInfo->points.at(lineSeriesIndex);
            const size_t pointSize = points.size();

            if (!points.empty())
            {
                for (uint8_t j = 0; j < pointSize; ++j)
                {
                    points[j].setX(points[j].x() - 1);
                }
                if (points.first().x() < 0)
                {
                    points.removeFirst();
                }
            }

            double currentY = 0.0;
            if (!graphInfo->values.empty())
            {
                currentY = std::round(graphInfo->values.at(lineSeriesIndex));
            }
            points.append({ 60.0, currentY });
            lineSeries->replace(points);
        }

        graphInfo->chartView->setUpdatesEnabled(true);
    }

    //m_cpuGraphs[m_cpuComboBoxActiveGraph->currentIndex()]->chartView->repaint();

    for (int i = 0; i < m_cpuTableInfos.size(); ++i)
    {
        m_cpuTableWidget->item(i, 1)->setText(m_cpuTableInfos[i]);
    }
}

void TabPerformance::processGpu()
{
    for (int i = 0; i < m_gpuGraphs.size(); ++i)
    {
        GraphInfo* graphInfo = m_gpuGraphs[i];
        graphInfo->chartView->setUpdatesEnabled(false);

        for (int lineSeriesIndex = 0; lineSeriesIndex < graphInfo->lineSeries.size(); ++lineSeriesIndex)
        {
            QLineSeries* lineSeries = graphInfo->lineSeries.at(lineSeriesIndex);
            QVector<QPointF>& points = graphInfo->points.at(lineSeriesIndex);
            const size_t pointSize = points.size();

            if (!points.empty())
            {
                for (uint8_t j = 0; j < pointSize; ++j)
                {
                    points[j].setX(points[j].x() - 1);
                }
                if (points.first().x() < 0)
                {
                    points.removeFirst();
                }
            }

            double currentY = 0.0;
            if (!graphInfo->values.empty())
            {
                currentY = std::round(graphInfo->values.at(lineSeriesIndex));
            }
            points.append({ 60.0, currentY });
            lineSeries->replace(points);
        }

        graphInfo->chartView->setUpdatesEnabled(true);
    }

    //m_gpuGraphs[m_gpuComboBoxActiveGraph->currentIndex()]->chartView->repaint();

    for (int i = 0; i < m_gpuTableInfos.size(); ++i)
    {
        m_gpuTableWidget->item(i, 1)->setText(m_gpuTableInfos[i]);
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

void TabPerformance::processNetwork()
{
    QList<QPointF> points = m_networkLineSeries->points();
    if (!points.empty())
    {
        m_networkLineSeries->clear();
        for (uint8_t i = 0; i < points.size(); ++i)
        {
            points[i].setX(points.at(i).x() - 1);
        }
        if (points.first().x() < 0)
        {
            points.removeFirst();
        }
        m_networkLineSeries->append(points);
    }

    double x = 60;
    double y = m_networkUsedSpeed;
    m_networkLineSeries->append(x, y);
    m_networkChartView->repaint();
}

void TabPerformance::updateCpuMultiGraphs(const QMap<uint8_t, QVariant>& dynamicInfo)
{
    int index = CpuMultiGraphsStartIndex;
    std::vector<int> initGraphs;

    std::vector<double> cpuCoreUsages = dynamicInfo[Globals::Key_Cpu_CoreUsages].value<std::vector<double>>();
    if (m_cpuGraphs[index]->lineSeries.empty() && !cpuCoreUsages.empty())
    {
        m_cpuGraphs[index]->lineSeries.resize(cpuCoreUsages.size());
        m_cpuGraphs[index]->points.resize(m_cpuGraphs[index]->lineSeries.size());
        m_cpuGraphs[index]->values.clear();
        initGraphs.push_back(index);
    }
    m_cpuGraphs[index]->values = cpuCoreUsages;
    ++index;

    std::vector<double> cpuCoreFrequencies = dynamicInfo[Globals::Key_Cpu_CoreFrequencies].value<std::vector<double>>();
    if (m_cpuGraphs[index]->lineSeries.empty() && !cpuCoreFrequencies.empty())
    {
        m_cpuGraphs[index]->lineSeries.resize(cpuCoreFrequencies.size());
        m_cpuGraphs[index]->points.resize(m_cpuGraphs[index]->lineSeries.size());
        m_cpuGraphs[index]->values.clear();
        initGraphs.push_back(index);
    }
    m_cpuGraphs[index]->values = cpuCoreFrequencies;
    ++index;

    std::vector<double> cpuThreadUsages = dynamicInfo[Globals::Key_Cpu_ThreadUsages].value<std::vector<double>>();
    if (m_cpuGraphs[index]->lineSeries.empty() && !cpuThreadUsages.empty())
    {
        m_cpuGraphs[index]->lineSeries.resize(cpuThreadUsages.size());
        m_cpuGraphs[index]->points.resize(m_cpuGraphs[index]->lineSeries.size());
        m_cpuGraphs[index]->values.clear();
        initGraphs.push_back(index);
    }
    m_cpuGraphs[index]->values = cpuThreadUsages;
    ++index;

    std::vector<double> cpuThreadFrequencies = dynamicInfo[Globals::Key_Cpu_ThreadFrequencies].value<std::vector<double>>();
    if (m_cpuGraphs[index]->lineSeries.empty() && !cpuThreadFrequencies.empty())
    {
        m_cpuGraphs[index]->lineSeries.resize(cpuThreadFrequencies.size());
        m_cpuGraphs[index]->points.resize(m_cpuGraphs[index]->lineSeries.size());
        m_cpuGraphs[index]->values.clear();
        initGraphs.push_back(index);
    }
    m_cpuGraphs[index]->values = cpuThreadFrequencies;
    
    for (int i = 0; i < initGraphs.size(); ++i)
    {
        int graphIndex = initGraphs.at(i);
        for (int j = 0; j < m_cpuGraphs[graphIndex]->lineSeries.size(); ++j)
        {
            m_cpuGraphs[graphIndex]->lineSeries[j] = new QLineSeries();
            m_cpuGraphs[graphIndex]->lineSeries[j]->setUseOpenGL(true);
            m_cpuGraphs[graphIndex]->chart->addSeries(m_cpuGraphs[graphIndex]->lineSeries[j]);
        }
        m_cpuGraphs[graphIndex]->chart->createDefaultAxes();
        m_cpuGraphs[graphIndex]->chart->axes(Qt::Horizontal).back()->setRange(0, m_cpuGraphs[graphIndex]->axisMin);
        m_cpuGraphs[graphIndex]->chart->axes(Qt::Vertical).back()->setRange(0, m_cpuGraphs[graphIndex]->axisMax);
    }
}

void TabPerformance::slotCpuDynamicInfo(const QMap<uint8_t, QVariant>& dynamicInfo)
{
    int index = 0;

    m_cpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Cpu_TotalUsage].value<double>();
    m_cpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CurrentMaxFrequency].value<double>();
    m_cpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Power].value<double>();
    m_cpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Cpu_SocPower].value<double>();
    m_cpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Cpu_CoreVoltage].value<double>();
    m_cpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Temperature].value<double>();
    m_cpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Cpu_Fanspeed].value<double>();

    updateCpuMultiGraphs(dynamicInfo);

    for (int i = 0; i < m_cpuGraphs.size(); ++i)
    {
        if (m_cpuGraphs[i]->values.size() == 1)
        {
            if (m_cpuDynamicMax[i] < m_cpuGraphs[i]->values[0])
            {
                m_cpuDynamicMax[i] = m_cpuGraphs[i]->values[0];
                m_cpuGraphs[i]->axisMax = std::ceil(m_cpuDynamicMax[i]);
                m_cpuGraphs[i]->chart->axes(Qt::Vertical).back()->setMax(m_cpuGraphs[i]->axisMax);
            }
            m_cpuTableInfos[i] = QString::number(m_cpuGraphs[i]->values[0], 'f', 2) + " " + m_cpuGraphs[i]->unit;
        }
        else
        {
            m_cpuTableInfos[i] = "";
            const int oldAxisMax = m_cpuGraphs[i]->axisMax;

            for (int j = 0; j < m_cpuGraphs[i]->values.size(); ++j)
            {
                if (m_cpuDynamicMax[i] < m_cpuGraphs[i]->values[j])
                {
                    m_cpuDynamicMax[i] = m_cpuGraphs[i]->values[j];
                    m_cpuGraphs[i]->axisMax = std::ceil(m_cpuDynamicMax[i]);
                }
                m_cpuTableInfos[i] += QString::number(static_cast<int>(std::round(m_cpuGraphs[i]->values[j]))) + " | ";
            }

            if (m_cpuGraphs[i]->axisMax > oldAxisMax)
            {
                m_cpuGraphs[i]->chart->axes(Qt::Vertical).back()->setMax(m_cpuGraphs[i]->axisMax);
            }
        }  
    }
}

void TabPerformance::slotGpuDynamicInfo(const QMap<uint8_t, QVariant>& dynamicInfo)
{
    int index = 0;

    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Usage].value<double>();
    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamUsage].value<double>();
    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_ClockSpeed].value<double>();
    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamClockSpeed].value<double>();
    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_VRamUsed].value<double>();
    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Power].value<double>();
    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_TotalBoardPower].value<double>();
    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Voltage].value<double>();
    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_Temperature].value<double>();
    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_HotSpotTemperature].value<double>();
    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_FanSpeed].value<double>();
    m_gpuGraphs[index++]->values[0] = dynamicInfo[Globals::SysInfoAttr::Key_Gpu_FanSpeedUsage].value<double>();

    for (int i = 0; i < m_gpuGraphs.size(); ++i)
    {
        if (m_gpuGraphs[i]->values.size() == 1)
        {
            if (m_gpuDynamicMax[i] < m_gpuGraphs[i]->values[0])
            {
                m_gpuDynamicMax[i] = m_gpuGraphs[i]->values[0];
                m_gpuGraphs[i]->axisMax = std::ceil(m_gpuDynamicMax[i]);
                m_gpuGraphs[i]->chart->axes(Qt::Vertical).back()->setMax(m_gpuGraphs[i]->axisMax);
            }
            m_gpuTableInfos[i] = QString::number(m_gpuGraphs[i]->values[0], 'f', 2) + " " + m_gpuGraphs[i]->unit;
        }
        else
        {
            m_gpuTableInfos[i] = "";
            const int oldAxisMax = m_gpuGraphs[i]->axisMax;

            for (int j = 0; j < m_gpuGraphs[i]->values.size(); ++j)
            {
                if (m_gpuDynamicMax[i] < m_gpuGraphs[i]->values[j])
                {
                    m_gpuDynamicMax[i] = m_gpuGraphs[i]->values[j];
                    m_gpuGraphs[i]->axisMax = std::ceil(m_gpuDynamicMax[i]);
                }
                m_gpuTableInfos[i] += QString::number(static_cast<int>(std::round(m_gpuGraphs[i]->values[j]))) + " | ";
            }

            if (m_gpuGraphs[i]->axisMax > oldAxisMax)
            {
                m_gpuGraphs[i]->chart->axes(Qt::Vertical).back()->setMax(m_gpuGraphs[i]->axisMax);
            }
        }
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

void TabPerformance::slotTotalNetworkSpeed(const uint32_t& val)
{
    m_networkTotalSpeed = val;
    m_networkChart->axes(Qt::Vertical).back()->setRange(0.0, m_networkTotalSpeed);
}

void TabPerformance::slotUsedNetworkSpeed(const uint32_t& val)
{
    m_networkUsedSpeed = val;

    if (m_networkUsedSpeed > m_networkCurrentMaxSpeed)
    {
        m_networkCurrentMaxSpeed = m_networkUsedSpeed;
        m_networkChart->axes(Qt::Vertical).back()->setRange(0.0, m_networkCurrentMaxSpeed);
    }
}

void TabPerformance::showSelectionWidget()
{
    m_cpuWidget->hide();
    m_gpuWidget->hide();
    m_memoryWidget->hide();
    m_networkWidget->hide();

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
    case 3:
        m_networkWidget->show();
        break;
    default:
        break;
    }
}
