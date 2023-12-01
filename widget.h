#pragma once
#include <QWidget>
#include <QSerialPort>
#include <QtCharts/QtCharts>
#include <QtCharts/QSplineSeries>
#include <QChartView>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QTimer>
#include <QFile>
#include "serialthread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE



class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void init_Widget();
    void init_Chart();


private slots:
    void on_OpenPortBtn_released();

//    void on_SendDataBtn_released();

    void on_RefreshPortBtn_released();

    void Refresh_Chart();   // 刷新画布

    void onNewDataArrived(QStringList NewData); // 接受新数据

    void on_checkBox_1_stateChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

    void on_ClearChartBtn_released();

signals:


private:
    Ui::Widget *ui;

    QTimer *updatetimer;    // 更新画布计时器

    SerialThread *serial;


    QChart *chart;
    QSplineSeries *series, *series1;
    QValueAxis *axisX;
    QValueAxis *axisY;
    double number = 0;   // 数据点个数

    QFile *file;    // 数据输出的目标文件

};
