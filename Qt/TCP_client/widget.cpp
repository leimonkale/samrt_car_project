#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    connect (socket, &QTcpSocket::connected, this, &Widget::netConnected);

    connect (socket, &QTcpSocket::disconnected, this, &Widget::netDisConnected);

    connect(socket, &QTcpSocket::errorOccurred, this, [&](QTcpSocket::SocketError err){
        qDebug()<<err;
        ui->textEdit->append(socket->errorString());
    });

    connect(socket,&QTcpSocket::readyRead,this,&Widget::recvData);
}

void Widget::netConnected()
{
    ui->label->setStyleSheet("background-color: green; border-radius: 25px;");
    // 阻塞程序等待5秒，5秒内连接服务器成功就返回true，失败超时返回false
    // if(!socket->waitForConnected(5000)){
    //     ui->textEdit->append(socket->errorString());
    // }
}
void Widget::netDisConnected()
{
    ui->label->setStyleSheet("background-color: red; border-radius: 25px;");
}


Widget::~Widget()
{
    delete ui;
}

void Widget::parseSensorData(const QString &data, QString &direction, QString &tem, QString &hum) {
    direction.clear();
    tem.clear();
    hum.clear();

    if (data.isEmpty()) {
        qDebug() << "输入数据为空";
        return;
    }

    int dirStart = data.indexOf("方向：");
    if (dirStart != -1) {
        dirStart += 3;
        int dirEnd = data.indexOf(",", dirStart);
        if (dirEnd != -1) {
            direction = data.mid(dirStart, dirEnd - dirStart).trimmed();
        }
    }

    int temStart = data.indexOf("tem：");
    if (temStart != -1) {
        temStart += 4; 
        int temEnd = data.indexOf(",", temStart);
        if (temEnd != -1) {
            tem = data.mid(temStart, temEnd - temStart).trimmed();
        }
    }

    // 提取湿度（格式："hum;60"）
    int humStart = data.indexOf("hum：");
    if (humStart != -1) {
        humStart += 4; 
        int humEnd = data.indexOf("\n", humStart);
        if (humEnd != -1) {
            hum = data.mid(humStart, humEnd - humStart).trimmed();
        } else {
          
            hum = data.mid(humStart).trimmed();
        }
    }

    qDebug() << "解析结果 - 方向：" << direction << "温度：" << tem << "湿度：" << hum;
}

bool Widget::receiveImage(QTcpSocket *socket, QImage &image)
{
    if (socket->state() != QAbstractSocket::ConnectedState) {
        return false;
    }

    // 先接收图片大小（4字节）
    qint32 imageSize = 0;
    qint64 bytesRead = socket->read((char*)&imageSize, sizeof(qint32));

    // 检查是否读取到完整的大小信息
    if (bytesRead != sizeof(qint32) || imageSize <= 0) {
        return false;
    }

    // 接收完整的图片数据
    QByteArray imageData;
    while (imageData.size() < imageSize) {
        // 等待数据到达
        if (!socket->waitForReadyRead(3000)) {
            return false;
        }
        imageData.append(socket->readAll());
    }
    image.loadFromData(imageData);
    return !image.isNull();
}

void Widget::on_pushButton_clicked()
{
    socket->connectToHost("192.168.137.1",8888);
}


void Widget::on_pushButton_2_clicked()
{
    socket->disconnectFromHost();
}


void Widget::on_pushButton_3_clicked()
{
    socket->write(ui->textEdit_2->toPlainText().toUtf8());
}

void Widget::recvData()
{

        QImage picture;
        receiveImage(socket, picture);
        ui->label_5->setPixmap(QPixmap::fromImage(picture));
        flag = 0;

    qDebug()<<socket->bytesAvailable();
    // if(socket->bytesAvailable() < 32)
    //     return;
    QByteArray data = socket->readAll();
    // QByteArray data = socket->read(30);
    parseSensorData(data,direction,tem,tum);
    ui->textEdit->append("recv->"+data);
    ui->lineEdit_3->setText(tem);
    ui->lineEdit_4->setText(tum);
    ui->lineEdit_5->setText(direction);

}



void Widget::on_pushButton_4_clicked()
{
    socket->write("2");
}


void Widget::on_pushButton_5_clicked()
{
    socket->write("3");
}


void Widget::on_pushButton_7_clicked()
{
    socket->write("4");
}


void Widget::on_pushButton_10_clicked()
{
    socket->write("1");
}


void Widget::on_pushButton_8_clicked()
{
    socket->write("5");
}


void Widget::on_pushButton_9_clicked()
{
    socket->write("6");
}


void Widget::on_pushButton_6_clicked()
{
    socket->write("2556");
}

