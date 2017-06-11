#include "chatsystem.h"
#include "ui_chatsystem.h"

ChatSystem::ChatSystem(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChatSystem)
{
    ui->setupUi(this);

    udpSocket = new QUdpSocket(this);
    port = 45454;
    udpSocket->bind(port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    sendMessage(UserJoin);
}

ChatSystem::~ChatSystem()
{
    delete ui;
}

// 发送UDP消息
void ChatSystem::sendMessage(MessageType type, QString serverAddress)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString localHostName = QHostInfo::localHostName();
    QString address = getIP();
    out << type << getUserName() << localHostName;

    switch (type) {
    case Message:
        if(ui->messageTextEdit->toPlainText() == "")
        {
            QMessageBox::warning(0, QStringLiteral("警告"), QStringLiteral("发送内容不能为空"), QMessageBox::Ok);
            return;
        }
        out << address << getMessage();
        ui->messageBrowser->verticalScrollBar()->setValue(ui->messageBrowser->verticalScrollBar()->maximum());
        break;
    case UserJoin:
        out << address;
        break;
    case UserLeave:
        break;

    case FileName:
          break;

    case Refuse:
          break;
    }
    udpSocket->writeDatagram(data, data.length(), QHostAddress::Broadcast, port);
}

// 接收UDP消息
void ChatSystem::processPendingDatagrams()
{
    while(udpSocket->hasPendingDatagrams())
    {
        // 定义数据报
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        // 读取数据报
        udpSocket->readDatagram(datagram.data(), datagram.size());
        // 定义数据流
        QDataStream in(&datagram, QIODevice::ReadOnly);
        int messageType;
        in >> messageType;
        // 用户名，主机名，IP地址，消息
        QString userName,localHostName,ipAddress,message;
        // 定义时间
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        switch (messageType) {
        case Message:
            // 消息类型
            in >> userName >> localHostName >> ipAddress;
            ui->messageBrowser->setTextColor(Qt::blue);
            ui->messageBrowser->setCurrentFont(QFont("Times New Roman", 12));
            ui->messageBrowser->append("[ " + userName + " ]" + time);
            ui->messageBrowser->append(message);
            break;
        case UserJoin:
            // 用户加入类型
            in >> userName >> localHostName >> ipAddress;
            userJoin(userName, localHostName, ipAddress);
            break;
        case UserLeave:
            // 用户离开类型
            in >>userName >>localHostName;
            userLeave(userName,localHostName,time);
            break;
        case FileName:
            // 文件传输类型，获取文件名
            break;
        case Refuse:
            // 拒绝接受文件类型
            break;

        }
    }
}

// 处理新用户加入
void ChatSystem::userJoin(QString userName, QString localHostName, QString ipAddress)
{
    bool isEmpty = ui->userTableWidget->findItems(localHostName, Qt::MatchExactly).isEmpty();
    if (isEmpty) {
        QTableWidgetItem *user = new QTableWidgetItem(userName);
        QTableWidgetItem *host = new QTableWidgetItem(localHostName);
        QTableWidgetItem *ip = new QTableWidgetItem(ipAddress);

        ui->userTableWidget->insertRow(0);
        ui->userTableWidget->setItem(0,0,user);
        ui->userTableWidget->setItem(0,1,host);
        ui->userTableWidget->setItem(0,2,ip);
        ui->messageBrowser->setTextColor(Qt::gray);
        ui->messageBrowser->setCurrentFont(QFont("Times New Roman",10));
        ui->messageBrowser->append(QStringLiteral("%1 在线！").arg(userName));
        ui->userNumLabel->setText(QStringLiteral("在线人数：%1").arg(ui->userTableWidget->rowCount()));

        sendMessage(UserJoin);
    }
}


void ChatSystem::userLeave(QString userName, QString localHostName, QString time)
{
    // 获取退出的用户所在的行数
    int rowNum = ui->userTableWidget->findItems(localHostName, Qt::MatchExactly).first()->row();
    // 在userTableWidget中移除退出的用户
    ui->userTableWidget->removeRow(rowNum);
    // 设置颜色为灰色
    ui->messageBrowser->setTextColor(Qt::gray);
    // 设置字体
    ui->messageBrowser->setCurrentFont(QFont("Times New Roman", 10));
    // 离开信息
    ui->messageBrowser->append(QStringLiteral("%1 于 %2 离开！").arg(userName).arg(time));
    // 在线人数
    ui->userNumLabel->setText(QStringLiteral("在线人数：%1").arg(ui->userTableWidget->rowCount()));
}

// 获取ip地址
QString ChatSystem::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    return 0;
}

// 获得用户名
QString ChatSystem::getUserName()
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    // 获得系统信息
    QStringList environment = QProcess::systemEnvironment();
    //找到并返回用户名
    foreach (QString string, envVariables) {
        int index = environment.indexOf(QRegExp(string));
        if(index != -1)
        {
            QStringList stringList = environment.at(index).split('=');
            if(stringList.size() == 2)
            {
                return stringList.at(1);
                break;
            }
        }
    }
    return "unknow";

}

// 获得要发送的消息
QString ChatSystem::getMessage()
{
    QString msg = ui->messageTextEdit->toHtml()/*toPlainText()*/;
    ui->messageTextEdit->clear();
    ui->messageTextEdit->setFocus();
    return msg;
}

// 发送
void ChatSystem::on_sendBtn_clicked()
{
    sendMessage(Message);
}

// 退出
void ChatSystem::on_exitBtn_clicked()
{
//    QApplication* app;
//    app->exit(0);
    exit(0);
}
