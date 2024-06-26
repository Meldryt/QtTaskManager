#pragma once

#include <QWidget>
#include <QPushButton>
#include <QTimer>

#include "GlWidget.h"

class GpuBenchmarkWidget : public QWidget
{
public:
    explicit GpuBenchmarkWidget(QWidget *parent = nullptr);
    
private:
    //void process();

    QPushButton* m_buttonStart{nullptr};
    QTimer* m_timer{ nullptr };
    GlWidget* m_glWidget{ nullptr };

public slots:
    void startBenchmark();
    void stopBenchmark();

private slots:


};
