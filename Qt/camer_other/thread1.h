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

    Thread1(QMutex &m, Widget *widget) : m(m), w(widget) {}

    void run(){
        while(1){
            QString s;
            QStringList l;
            m.lock();
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
            m.unlock();
        }
        QThread::msleep(100);
    }

signals:
    void newPort(const QString &portName);
    void samePort(const QStringList &listName);

private:


    QMutex &m;
    Widget *w;

};

#endif // THREAD1_H
