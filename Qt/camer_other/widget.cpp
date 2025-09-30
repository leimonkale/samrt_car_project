#include "widget.h"
#include "ui_widget.h"
#include "thread1.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    serialport = new QSerialPort();
    menu = new QMenu(ui->pushButton);

    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){
        menu->addAction(info.portName(), this, [this, portName = info.portName()]() {
            this->connectSerial(portName); 
        });
        qDebug()<<info.portName();
    }

    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){
        ports.append(info.portName());
    }

    ui->pushButton->setMenu(menu);

    m = new QMutex;
    Thread1 *thread = new Thread1(*m,this);
    // 连接信号（子线程发现新串口时，主线程更新UI）
    connect(thread, &Thread1::newPort,this,[&](const QString &portName){
       menu->addAction(portName,this,[this,portName](){
            this->connectSerial(portName);
        });
    });

    connect(thread, &Thread1::samePort,this,[&](const QStringList &listName){
        // 1. 检查列表空值，避免异常
        if (listName.isEmpty()) return;

        // 2. 取出需要删除的目标名称（listName 中唯一元素）
        QString targetName = listName.first();

        // 3. 先删除菜单中对应的动作
        foreach (QAction *action, menu->actions()) {
            if (action->text() == targetName) {
                menu->removeAction(action);
                action->deleteLater(); 
                break; 
            }
        }
        ports.removeOne(targetName);
    });

    thread->start(); // 启动线程connect(,)

    server = new QTcpServer(this);

    connect (server, &QTcpServer::newConnection, this, &Widget::newConnection);

    initUI();
}

Widget::~Widget()
{
    delete ui;
}

const QString Widget::forPorts()
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if(!ports.contains(info.portName())){
            ports.append(info.portName());
            return info.portName();
        }
    }
    return "";
}

const QStringList Widget::removePorts()
{
    QStringList saveList = ports;
    QStringList emptyList;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if(ports.contains(info.portName())){
            saveList.removeOne(info.portName());
        }
    }
    if(!saveList.isEmpty()){
        return saveList;
    }
    return emptyList;
}

void Widget::on_pushButton_2_clicked()
{
    disconnectSerial();
}

bool Widget::connectSerial(const QString &portName) {
    
    if (serialport->isOpen()) {
        serialport->close();
    }

    // 设置串口名称
    serialport->setPortName(portName);

    //  配置串口参数
    serialport->setBaudRate(QSerialPort::Baud115200);
    serialport->setDataBits(QSerialPort::Data8);
    serialport->setParity(QSerialPort::NoParity);
    serialport->setStopBits(QSerialPort::OneStop);
    serialport->setFlowControl(QSerialPort::NoFlowControl);

    // 打开串口（读写模式）
    if (serialport->open(QIODevice::ReadWrite)) {
        qDebug() << "串口连接成功：" << portName;
        
        ui->pushButton_2->setEnabled(true);
        connect(serialport, &QSerialPort::readyRead, this, &Widget::readSerialData);
        return true;
    } else {
        
        qDebug() << "串口连接失败：" << serialport->errorString();
        return false;
    }
}

void Widget::readSerialData() {
    
    QByteArray data = serialport->readAll();
    QTextCodec *gbkCodec = QTextCodec::codecForName("GBK");
    if (!gbkCodec) {
        
        return;
    }
    //  将GBK字节数组转换为Unicode字符串
    QString unicodeStr = gbkCodec->toUnicode(data);

    // Unicode字符串转换为UTF-8编码的字节数组
    QByteArray utf8Data = unicodeStr.toUtf8();

    m_rxBuffer += unicodeStr;
    
    while (m_rxBuffer.contains('\n')) {

        int endIndex = m_rxBuffer.indexOf('\n');
        QString completeData = m_rxBuffer.left(endIndex + 1);  

        ui->textEdit->append(completeData);  
        qDebug() << "收到完整数据：" << completeData;

        if (!completeData.isEmpty() && completeData.at(0) == QStringLiteral("左")) {
            // 首字符是“左”，按UTF-8编码发送
            QByteArray sendData = completeData.toUtf8();
            if (socket && socket->state() == QTcpSocket::ConnectedState) {
                qint64 bytesWritten = socket->write(sendData);
                if (bytesWritten == -1) {
                    qDebug() << "Socket写入失败:" << socket->errorString();
                } else {
                    qDebug() << "[符合条件-发送] 成功写入" << bytesWritten << "字节，内容:" << completeData;
                }
            } else {
                qDebug() << "[符合条件-未发送] Socket未连接，无法发送数据:" << completeData;
            }
        } else {

            qDebug() << "[不符合条件-不发送] 首字符非“左”，数据:" << completeData;
        }
        //  从缓冲区中移除已处理的部分，保留剩余数据
        m_rxBuffer.remove(0, endIndex + 1);
    }
}

void Widget::disconnectSerial() {
    // 检查串口是否已打开，避免重复关闭
    if (!serialport->isOpen()) {
        //ui->statusLabel->setText("未连接串口");
        return;
    }

    // 记录当前连接的串口名称
    QString currentPort = serialport->portName();

    //  关闭串口
    serialport->close();

    
    disconnect(serialport, &QSerialPort::readyRead, this, &Widget::readSerialData);

    // 更新 UI 状态
    // ui->statusLabel->setText("已断开：" + currentPort);
    // menu->setEnabled(true);
    ui->pushButton_2->setEnabled(false); 
    // ui->connectButton->setEnabled(true); 

    qDebug() << "串口已断开：" << currentPort;
}


void Widget::on_pushButton_3_clicked()
{
    QString message = ui->lineEdit->text();
    if (!message.isEmpty()) {
        // 将QString转换为QByteArray
        QByteArray data = message.toUtf8();
        // 向串口发送数据
        qint64 bytesWritten = serialport->write(data);
        if (bytesWritten == -1) {
            qDebug() << "发送失败: " << serialport->errorString();
        } else {
            qDebug() << "发送成功，发送字节数: " << bytesWritten;
        }
    }
}

void Widget::newConnection()
{
    // 从队列取出请求，建立通信套接字
    socket = server->nextPendingConnection ();
    if (!socket->isValid ()){
        ui->textEdit_2->append ("接受客户端：" + server->errorString ());
        return;
    }
    qDebug () << socket->peerAddress ().toString ();
    qDebug () << socket->peerPort ();
    ui->textEdit_2->append ( socket->peerAddress ().toString () + "->" + QString::number (socket->peerPort ()));
    connect(socket,&QTcpSocket::readyRead,this,&Widget::recvData);
}


// void Widget::on_pushButton_3_clicked()
//  {
//      socket->write(ui->textEdit_2->toPlainText().toUtf8());

//  }

void Widget::recvData()
{
    QByteArray data = socket->readAll();
    ui->textEdit_2->append("recv->" + data);
    if(!strcmp(data,"2556")){

        QImage pictrue;
        getVideoWidgetFrame(videoWidget, pictrue);
        sendImage(socket, pictrue);
    }else {
        serialport->write(data);
    }
}


void Widget::on_pushButton_4_clicked()
{
    qDebug() <<server->maxPendingConnections();
    if(server->listen(QHostAddress::AnyIPv4,8888)){
        //if(server->listen(QHostAddress::AnyIPv4,ui->lineEdit_2->text().toInt())){
        ui->textEdit_2->append("服务器启动失败"+server->errorString());
    }
    else{
        //ui->label->setStyleSheet("background:green;border-radius:25px;");
        ui->textEdit_2->append("服务器启动成功");
    }
}

void Widget::initUI()
{
    menu2 = new QMenu(ui->pushButton);
    cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &camera : cameras) {
        menu2->addAction(camera.description(),this,[this](){cameraStart();});
        //cameraCombo->addItem(camera.description());
    }
    ui->pushButton_5->setMenu(menu2);
}

void Widget::cameraStart(){

        
        videoWidget = new QVideoWidget(this);
        videoWidget->setMinimumSize(64, 48);
        ui->verticalLayout->addWidget(videoWidget);

       
        captureSession = new QMediaCaptureSession(this);
        camera = new QCamera(cameras.first(), this); 
        captureSession->setCamera(camera);
        captureSession->setVideoOutput(videoWidget); 
        
        camera->start();
}

void Widget::getVideoWidgetFrame(QVideoWidget *videoWidget, QImage &frame)
{
    frame = QImage();
    if (!videoWidget || !videoWidget->isVisible()) {
        return;
    }

    // 创建与视频部件大小相同的图像
    frame = QImage(videoWidget->size(), QImage::Format_ARGB32);
    frame.fill(Qt::transparent);

    // 绘制视频部件内容到图像
    QPainter painter(&frame);
    videoWidget->render(&painter);
    painter.end();
}

void Widget::sendImage(QTcpSocket *socket, const QImage &image)
{

    if (socket->state() != QAbstractSocket::ConnectedState) {
       
        return;
    }
    // 将QImage转换为字节数组（使用PNG格式压缩）
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");

    // 先发送图片数据大小
    qint32 imageSize = byteArray.size();
    socket->write((const char*)&imageSize, sizeof(qint32));
    socket->flush();

    // 再发送图片数据
    socket->write(byteArray);
    socket->flush();
}
