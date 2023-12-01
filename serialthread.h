#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QObject>
#include <QSerialPort>
#include <QThread>

#define START_MSG       'Z'
#define END_MSG         '#'

#define WAIT_START      1
#define IN_MESSAGE      2
#define UNDEFINED       3


typedef struct
{
    QString PortName;
    qint32 BaudRate;
    QSerialPort::Parity parity;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
}PortInfo;

class SerialThread : public QObject
{
    Q_OBJECT
public:
    explicit SerialThread(PortInfo &portInfo, QObject *parent = nullptr);
    ~SerialThread();

//    void Open(PortInfo &portInfo);  // 打开串口
//    void Close();                   // 关闭串口

    int state = WAIT_START; // 等待接受开始符号
    QSerialPort *m_SerialPort;
    QThread *m_thread;
    QString RecvStr;    // 接收到的数据字符串

public slots:
    void onReadyRead();     // 读数据
signals:
    void NewData(QStringList data);
};

#endif // SERIALTHREAD_H
