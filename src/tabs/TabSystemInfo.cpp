#include "TabSystemInfo.h"

#include <QGridLayout>
#include <QHeaderView>

#include "../Globals.h"

TabSystemInfo::TabSystemInfo(QWidget *parent) : QWidget{parent}
{
    qDebug() << __FUNCTION__;

    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setRowCount(SystemParameterNames.size());
    m_tableWidget->setColumnCount(2);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_tableWidget->horizontalHeader()->hide();
    m_tableWidget->verticalHeader()->hide();
    m_tableWidget->setColumnWidth(0, 400);

    for (int i = 0; i < SystemParameterNames.size(); ++i)
    {
        m_tableWidget->setItem(i, 0, new QTableWidgetItem(SystemParameterNames.at(i)));
        m_tableWidget->setItem(i, 1, new QTableWidgetItem());
    }

    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(m_tableWidget, 0, 0);

    setLayout(layout);
}

TabSystemInfo::~TabSystemInfo()
{
    qDebug() << __FUNCTION__;
}

void TabSystemInfo::process()
{

}

void TabSystemInfo::slotOSInfo(const QMap<uint8_t,QVariant>& staticInfo)
{
    m_tableWidget->item(0, 1)->setText(staticInfo[Globals::SysInfoAttr::Key_SysInfo_OS_Name].toString());
    m_tableWidget->item(1, 1)->setText(staticInfo[Globals::SysInfoAttr::Key_SysInfo_OS_CodeName].toString());
    m_tableWidget->item(2, 1)->setText(staticInfo[Globals::SysInfoAttr::Key_SysInfo_OS_Version].toString());
}
