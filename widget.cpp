#include "widget.h"
#include "ui_widget.h"
#include<QSerialPortInfo>
#include<QDebug>
#include<QMessageBox>
#include <iostream>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , serial(nullptr)
{
    ui->setupUi(this);
    init_Widget();
    init_Chart();
}

Widget::~Widget()
{
    if(serial != nullptr)
        delete serial;
    file->close();
    delete ui;
}

// 初始化窗口
void Widget::init_Widget()
{
    // 初始化串口
    auto PortsInfo = QSerialPortInfo::availablePorts();
    for(auto &info: PortsInfo)
    {
        ui->PortCmb->addItem(info.portName()+" "+info.description(), info.portName());
    }

    // 初始化波特率
    auto SB = QSerialPortInfo::standardBaudRates();
    for (auto &sb : SB)
    {
        ui->BaudRateCmb->addItem(QString::number(sb), sb);
    }
    ui->BaudRateCmb->setCurrentText("115200");
    // 停止位
    ui->StopBitsCmb->addItem("1", QSerialPort::OneStop);
    ui->StopBitsCmb->addItem("1.5", QSerialPort::OneAndHalfStop);
    ui->StopBitsCmb->addItem("2", QSerialPort::TwoStop);
    ui->StopBitsCmb->setCurrentIndex(0);
    // 数据位
    ui->DataBitsCmb->addItem("5", QSerialPort::Data5);
    ui->DataBitsCmb->addItem("6", QSerialPort::Data6);
    ui->DataBitsCmb->addItem("7", QSerialPort::Data7);
    ui->DataBitsCmb->addItem("8", QSerialPort::Data8);
    ui->DataBitsCmb->setCurrentText("8");
    // 校验位
    ui->ParityCmb->addItem("无None", QSerialPort::NoParity);
    ui->ParityCmb->addItem("奇Odd", QSerialPort::OddParity);
    ui->ParityCmb->addItem("偶Even", QSerialPort::EvenParity);
    ui->ParityCmb->setCurrentIndex(0);

//    ui->SendEdit->setPlainText("发送数据");
    ui->RecvEdit->setPlainText("接收数据");


    file = new QFile("D:/Qt-project/out.txt");
    file->open(QIODevice::ReadWrite);
    if(!file->isOpen())
        qDebug()<< "文件打开失败";



//    connect(this, SIGNAL(NewData(QStringList)), this, SLOT(onNewDataArrived(QStringList)));


}


// 初始化画布
void Widget::init_Chart()
{
    updatetimer = new QTimer(this);
    connect(updatetimer, SIGNAL(timeout()), this, SLOT(Refresh_Chart()));
    chart = new QChart();

    series = new QSplineSeries();
    series1 = new QSplineSeries();

    axisX = new QValueAxis();
    axisY = new QValueAxis();


    series->setPointLabelsVisible(false);    // 坐标不可见
    series1->setPointLabelsVisible(false);

    series->setColor(QColor(Qt::red));
    series->setColor(QColor(Qt::blue));


    chart->legend()->hide();  // 隐藏图标
    chart->addSeries(series);   // 添加画笔
//    chart->addSeries(series1);


    axisX->setTickCount(10);
    axisX->setRange(0,300);
    axisY->setTickCount(5);
    axisY->setMin(0);
    axisY->setMax(550);

    axisX->setTitleText("时间");  // 设置坐标名称
    axisY->setTitleText("V伏");

    axisX->setLinePenColor(QColor(Qt::darkRed));
    axisY->setLinePenColor(QColor(Qt::darkRed));       //设置坐标轴颜色
    axisY->setGridLineVisible(false);   // 设置y轴网格不显示

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);
//    series1->attachAxis(axisX);
//    series1->attachAxis(axisY);

    ui->ChartWidget->setChart(chart);
    ui->ChartWidget->setRenderHint(QPainter::Antialiasing);
}


// 打开串口
void Widget::on_OpenPortBtn_released()
{
    if(serial!=nullptr)
    {
        delete serial;
        serial = nullptr;
        ui->OpenPortBtn->setText("打开串口");
        updatetimer->stop();
        return;
    }
    // 获取串口
    auto PortName = ui->PortCmb->currentData().toString();
    // 波特率
    auto BaudRate = ui->BaudRateCmb->currentData().value<QSerialPort::BaudRate>();
    // 停止位
    auto StopBit = ui->StopBitsCmb->currentData().value<QSerialPort::StopBits>();
    // 校验位
    auto Parity = ui->ParityCmb->currentData().value<QSerialPort::Parity>();
    // 数据位
    auto DataBit = ui->DataBitsCmb->currentData().value<QSerialPort::DataBits>();

    PortInfo pi = {PortName, BaudRate, Parity, DataBit, StopBit};
    serial = new SerialThread(pi);

    connect(serial, SIGNAL(NewData(QStringList)), this, SLOT(onNewDataArrived(QStringList)));   // 将串口线程的信号与主线程槽链接

    ui->OpenPortBtn->setText("关闭串口");
    updatetimer->start(20);
}


// 刷新串口
void Widget::on_RefreshPortBtn_released()
{
    ui->PortCmb->clear();
    auto PortsInfo = QSerialPortInfo::availablePorts();
    for(auto &info : PortsInfo)
    {
        ui->PortCmb->addItem(info.portName()+":"+info.description(), info.portName());
    }
}


// 更新画布
void Widget::Refresh_Chart()
{
    chart->axisX()->setRange(number-300, number);
    chart->update();
}


// 接受新来的数据
void Widget::onNewDataArrived(QStringList NewData)
{
    volatile bool you_shall_NOT_PASS = false;
    while(you_shall_NOT_PASS){}
    you_shall_NOT_PASS = true;

    if(series->count() > 300)
    {
        series->remove(0);
        series1->remove(0);
    }
    series->append(number, NewData[0].toDouble());
    series1->append(number, NewData[1].toDouble());
    number += 1;

    // 显示到ui
    ui->RecvEdit->appendPlainText(NewData[0]+","+NewData[1]);

    // 写入文件
    QString str = "";
    for(auto &nd : NewData){
        str.append(nd);
        str.append(',');
    }
    str.remove(str.length()-1, 1);
    str.append("\n");
    file->write(str.toUtf8());

    you_shall_NOT_PASS = false;
}




// 发送数据
//void Widget::on_SendDataBtn_released()
//{
//    auto dataSend = ui->SendEdit->toPlainText();
//    dataSend += QString("\r\n");
//    qDebug() << dataSend;

//    auto num = serial->m_SerialPort->write(dataSend.toLocal8Bit());
//    qDebug() << num;
//    ui->SendEdit->setPlainText("");
//}



// 勾选/取消勾选通道1
void Widget::on_checkBox_1_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        chart->addSeries(series);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }
    else
        chart->removeSeries(series);
}

// 勾选/取消勾选通道2
void Widget::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        chart->addSeries(series1);
        series1->attachAxis(axisX);
        series1->attachAxis(axisY);
    }
    else
        chart->removeSeries(series1);
}

// 清除画布信息
void Widget::on_ClearChartBtn_released()
{
    series->clear();
    series1->clear();
    number = 0;
}

