#ifndef THREAD1_H
#define THREAD1_H
#include "widget.h"

#include <QThread>
#include <QSerialPortInfo>
#include <QMutex>
#include <QStringList>
#include <QList>

using namespace std;

class Widget;

class Thread1 : public QThread
{
    Q_OBJECT
public:
    // 构造函数：接收互斥锁和Widget指针（用于信号连接）
    Thread1(QMutex &m, Widget *widget) : m(m), w(widget) {}

    void run(){
        while(1){
            QString s;
            QStringList l;
            m.lock();// 加锁（若有共享数据需要保护）
            s = w->forPorts();
            l = w->removePorts();
            if(!s.isEmpty()){
                emit newPort(s);
                qDebug()<<"传递串口"<<s;
            }
            if(!l.isEmpty()){
                emit samePort(l);
                qDebug()<<"传递列表"<<l;
            }
            m.unlock();  // 解锁
        }
        QThread::msleep(100);  // 例如间隔100ms检查一次
    }

signals:
    void newPort(const QString &portName);
    void samePort(const QStringList &listName);

private:


    QMutex &m;
    Widget *w;

};

#endif // THREAD1_H
