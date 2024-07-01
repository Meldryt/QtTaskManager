#pragma once

#include <QWidget>
#include <QPushButton>

#include "GlWidget.h"
#include "GlWindow.h"

class TabBenchmark : public QWidget
{
public:
    explicit TabBenchmark(QWidget *parent = nullptr);

public slots:
    void slotOpenBenchmarkWindow();

    QPushButton* m_buttonStartBenchmark{ nullptr };
    QWindow* m_benchmarkWindow{ nullptr };
    GlWidget* m_glWidget{ nullptr };
    GlWindow* m_glWindow{ nullptr };
};
