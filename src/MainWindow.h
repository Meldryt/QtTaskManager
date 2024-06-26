#pragma once

#include <QMainWindow>
#include "SystemManager.h"

#include "GpuBenchmarkWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui{nullptr};
    SystemManager* systemManager{nullptr};

    QWindow* m_benchmarkWindow{ nullptr };
    GpuBenchmarkWidget* m_gpuBenchmarkWidget{ nullptr };
};
