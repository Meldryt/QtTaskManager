#pragma once

#include "ProcessDatabase.h"

#include <QWidget>
#include <QListWidget>
#include <QLabel>

class TabHardware : public QWidget
{
public:
    explicit TabHardware(ProcessDatabase* database, QWidget *parent = nullptr);

private:
    void process();
    void processCPU();
    void processGPU();
    void processRAM();

    ProcessDatabase* processDatabase{nullptr};
    QListWidget* listWidget{nullptr};

    QWidget* cpuWidget{nullptr};
    QLabel* cpuInfoLabel{nullptr};

    QWidget* gpuWidget{nullptr};
    QLabel* gpuInfoLabel{nullptr};

    QWidget* ramWidget{nullptr};
    QLabel* ramInfoLabel{nullptr};

    QTimer* timer{nullptr};

private slots:
    void showSelectionWidget();
};
