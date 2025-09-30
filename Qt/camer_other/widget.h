#ifndef WIDGET_H
#define WIDGET_H

#include <QStringList>
#include <QList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMenu>
#include <QMutex>
#include <QWidget>
#include <QTextCodec>
#include <QTcpServer>
#include <QTcpSocket>

#include <QMediaCaptureSession>
#include <QCamera>
#include <QVideoWidget>
#include <QVideoSink>
#include <QImage>
#include <QImageCapture>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QFileDialog>
#include <QComboBox>
#include <QPainter>

#include <QVBoxLayout>

#include <cstring>
#include <QBuffer>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    const QString forPorts();

    const QStringList removePorts();

    void newConnection();

    void cameraStart();

    void getVideoWidgetFrame(QVideoWidget* videoWidget, QImage& frame);

    void sendImage(QTcpSocket* socket, const QImage& image);


private slots:
    void on_pushButton_2_clicked();

    bool connectSerial(const QString &portName);

    void readSerialData() ;

    void disconnectSerial();

    void on_pushButton_3_clicked();

    void recvData();

    void on_pushButton_4_clicked();

private:
    Ui::Widget *ui;

    QSerialPort *serialport;
    QMenu *menu;
    QMenu *menu2;
    QStringList ports;
    QMutex *m;

    QTcpServer *server;
    QTcpSocket *socket;

    QMediaCaptureSession *captureSession;
    QCamera *camera;
    QVideoWidget *videoWidget;
    QImageCapture *imageCapture;
    void initUI();
    QString m_rxBuffer;

    QList<QCameraDevice> cameras;

    QVideoWidget *m_videoWidget = nullptr;
    QMediaCaptureSession *m_session = nullptr;
    QVideoSink *m_videoSink = nullptr;
};
#endif // WIDGET_H
