#pragma once

#include <QWidget>
#include <QTimer>
#include <QPushButton>

#include "GlWidget.h"

class TabBenchmark : public QWidget
{
public:
    explicit TabBenchmark(QWidget *parent = nullptr);

public slots:
    void slotOpenBenchmarkWindow();

    QTimer* m_timer{nullptr};

    QPushButton* m_buttonStartBenchmark{ nullptr };
    QWindow* m_benchmarkWindow{ nullptr };
    GlWidget* m_glWidget{ nullptr };
};
