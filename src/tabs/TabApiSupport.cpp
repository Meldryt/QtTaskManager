#include "TabApiSupport.h"

#include <QGridLayout>
#include <QPainter>
#include <QPoint>
#include <QHeaderView>

#include "../Globals.h"

TabApiSupport::TabApiSupport(QWidget *parent) : QWidget{parent}
{
    qDebug() << __FUNCTION__;

    m_listWidget = new QListWidget(this);
    m_listWidget->addItem("ADLX");
    m_listWidget->addItem("NVAPI");
    m_listWidget->addItem("NVML");
    m_listWidget->addItem("RyzenMasterSDK");
    m_listWidget->addItem("WMI");
    m_listWidget->setFixedWidth(100);

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(m_listWidget, 0, 0);

    for (uint8_t i = 0; i < ApiType_End; ++i)
    {
        QTableWidget* tableWidget = new QTableWidget(this);
        tableWidget->setRowCount(0);
        tableWidget->setColumnCount(2);
        tableWidget->horizontalHeader()->setStretchLastSection(true);
        tableWidget->horizontalHeader()->hide();
        tableWidget->verticalHeader()->hide();
        tableWidget->setColumnWidth(0, 400);
        m_apiFunctionsWidgets[i]= tableWidget;
        layout->addWidget(m_apiFunctionsWidgets[i], 0, i+1);
    }

    setLayout(layout);

    connect(m_listWidget,&QListWidget::itemSelectionChanged, this, &TabApiSupport::showSelectionWidget);
    m_listWidget->setCurrentRow(0);
}

TabApiSupport::~TabApiSupport()
{
    qDebug() << __FUNCTION__;
}

void TabApiSupport::process()
{

}

void TabApiSupport::slotApiSupportStaticInfo(const QMap<uint8_t,QVariant>& staticInfo)
{
    uint8_t widgetIdx = 0;
    for (uint8_t statusSupportIdx = Globals::SysInfoAttr::Key_Api_Functions_StatusSupport_Adlx; statusSupportIdx <= Globals::SysInfoAttr::Key_Api_Functions_StatusSupport_Wmi; ++statusSupportIdx)
    {
        if ((staticInfo.find(statusSupportIdx) != staticInfo.end()) && staticInfo[statusSupportIdx].canConvert<QMap<QString, bool>>())
        {
            QMap<QString, bool> functionsSupportStatus = staticInfo[statusSupportIdx].value<QMap<QString, bool>>();
            m_apiFunctionsWidgets[widgetIdx]->setRowCount(functionsSupportStatus.size());

            const uint8_t errorMessageIdx = Globals::SysInfoAttr::Key_Api_Functions_ErrorMessage_Adlx + widgetIdx;
            QMap<QString, QString> functionsErrorMessage;
            if ((staticInfo.find(errorMessageIdx) != staticInfo.end()) && staticInfo[errorMessageIdx].canConvert<QMap<QString, QString>>())
            {
                functionsErrorMessage = staticInfo[errorMessageIdx].value<QMap<QString, QString>>();
            }
            
            uint8_t i = 0;
            for (auto&& mapElem : functionsSupportStatus.asKeyValueRange())
            {
                QString errorStatus = "";
                if (!mapElem.second && functionsErrorMessage.find(mapElem.first) != functionsErrorMessage.end())
                {
                    errorStatus = " ( " + functionsErrorMessage[mapElem.first] + " )";
                }
                m_apiFunctionsWidgets[widgetIdx]->setItem(i, 0, new QTableWidgetItem(mapElem.first));
                m_apiFunctionsWidgets[widgetIdx]->setItem(i, 1, new QTableWidgetItem((mapElem.second ? "Yes" : "No") + errorStatus));
                ++i;
            }
        }
        ++widgetIdx;
    }


    //staticInfo[Key_ApiSupport_Adlx];
    //staticInfo[Key_ApiSupport_Nvapi];
    //staticInfo[Key_ApiSupport_Nvml];
    //staticInfo[Key_ApiSupport_RyzenMaster];
    //staticInfo[Key_ApiSupport_Wmi];
}

void TabApiSupport::showSelectionWidget()
{
    for (auto&& widget : m_apiFunctionsWidgets)
    {
        widget->hide();
    }

    m_apiFunctionsWidgets[m_listWidget->currentRow()]->show();
}