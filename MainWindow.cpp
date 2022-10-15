#include "MainWindow.h"
#include "./ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tabWidget = new TabWidget(this);
    setCentralWidget(tabWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

