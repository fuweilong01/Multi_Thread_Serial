#include "serialthread.h"
#include <QDebug>

SerialThread::SerialThread(PortInfo &portInfo, QObject *parent) : QObject(parent)
{
    m_thread = new QThread();
    m_SerialPort = new QSerialPort();

    this->moveToThread(m_thread);
    m_SerialPort->moveToThread(m_thread);

    m_SerialPort->setPortName(portInfo.PortName);
    m_SerialPort->setBaudRate(portInfo.BaudRate);
    m_SerialPort->setParity(portInfo.parity);
    m_SerialPort->setDataBits(portInfo.dataBits);
    m_SerialPort->setStopBits(portInfo.stopBits);
    if(!m_SerialPort->open(QIODevice::ReadWrite))
        qDebug() << "打开串口失败";

    connect(m_SerialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    m_thread->start();
}

SerialThread::~SerialThread()
{
    m_SerialPort->close();
    m_SerialPort->deleteLater();
    m_thread->quit();
    m_thread->wait();
    m_thread->deleteLater();
    qDebug() << "线程关闭";
}



// 读串口
void SerialThread::onReadyRead()
{
    if(m_SerialPort->bytesAvailable())   // 如果有数据可读
    {
        QByteArray dataRecv = m_SerialPort->readAll();

        if(!dataRecv.isEmpty())     // 如果读到的数据不为空
        {
            char *temp = dataRecv.data();
            for(int i=0; i<dataRecv.size(); i++)
            {
                switch (state) {
                case WAIT_START :
                    if(temp[i] == START_MSG){    // 如果读到开始字符，转换读数据模式
                        state = IN_MESSAGE;
                        RecvStr.clear();
                        break;
                    }
                    break;
                case IN_MESSAGE :
                    if(temp[i] == END_MSG){     // 如果读到结束字符，转换模式并发出信号
                        state = WAIT_START;
                        QStringList DataList = RecvStr.split(',');

//                        ui->RecvEdit->insertPlainText(RecvStr);
                        qDebug() << DataList;

                        emit NewData(DataList);
                        RecvStr.clear();
                        break;
                    }else if(isdigit(temp[i]) || isspace(temp[i]) || temp[i]==',' || temp[i]=='.'){
                            RecvStr.append(temp[i]);
                    }
                    break;
                default:
                    break;
                }
            }
        }

    }

}
