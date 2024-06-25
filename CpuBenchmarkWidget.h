#include <QWidget>
#include <QPushButton>
#include <QTimer>

class CpuBenchmarkWidget : public QWidget
{
public:
    explicit CpuBenchmarkWidget(QWidget *parent = nullptr);
    
private:
    //void process();

    QPushButton* m_buttonStart{nullptr};
    QTimer* m_timer{ nullptr };

public slots:
    void startBenchmark();
    void stopBenchmark();

private slots:


};