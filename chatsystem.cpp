#include "chatsystem.h"
#include "ui_chatsystem.h"
#include "tcpclient.h"
#include "tcpserver.h"

ChatSystem::ChatSystem(qint16 chatPort, qint16 filePort, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChatSystem)
{
    ui->setupUi(this);

    // 获取聊天端口、文件端口
    this->chatPort = chatPort;
    this->filePort = filePort;

    // 绑定 UDP Socket，使用 UDP 进行消息传输
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(chatPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    // 用户加入消息
    sendMessage(UserJoin);

    // 使用 TCP 进行文件传输
    server = new TcpServer(this->filePort);
    connect(server, SIGNAL(sendFileName(QString)), this, SLOT(getFileName(QString)));

    connect(ui->messageTextEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentFormatChanged(const QTextCharFormat)));

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
    QString clientAddress;
    int row;

    // 消息共同的数据流的顺序为：消息类型、用户名、用户主机名
    out << type << getUserName() << localHostName;

    switch (type) {
    case Message:
        if(ui->messageTextEdit->toPlainText() == "")
        {
            QMessageBox::warning(0, QStringLiteral("警告"), QStringLiteral("发送内容不能为空"), QMessageBox::Ok);
            return;
        }
        // 消息数据流的顺序为：消息类型、用户名、用户主机名、IP地址、消息内容
        out << address << getMessage();
        ui->messageBrowser->verticalScrollBar()->setValue(ui->messageBrowser->verticalScrollBar()->maximum());
        break;

    case UserJoin:
        out << address;
        break;

    case UserLeave:
        break;

    case FileName:
        row = ui->userTableWidget->currentRow();
        clientAddress = ui->userTableWidget->item(row, 2)->text();
        out << address << clientAddress << fileName;
        break;

    case Refuse:
        out << serverAddress;
        break;

    }
    udpSocket->writeDatagram(data, data.length(), QHostAddress::Broadcast, chatPort);
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
        QString userName,localHostName,ipAddress,message, clientAddress, serverAddress, fileName;
        // 定义时间
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        switch (messageType) {
        case Message:
            // 消息类型
            in >> userName >> localHostName >> ipAddress >> message;
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
            in >> userName >> localHostName >> ipAddress;
            in >> clientAddress >> fileName;
            hasPendingFile(userName, ipAddress, clientAddress, fileName);
            break;
        case Refuse:
            // 拒绝接受文件类型
            in >> userName >> localHostName;
            in >> serverAddress;
            ipAddress = getIP();

            if(ipAddress == serverAddress)
            {
                server->refused();
            }
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
    QString msg = ui->messageTextEdit->toHtml();
    ui->messageTextEdit->clear();
    ui->messageTextEdit->setFocus();
    return msg;
}

// 获取要发送的文件名
void ChatSystem::getFileName(QString name)
{
    fileName = name;
    sendMessage(FileName);
}

// 发送文件按钮
void ChatSystem::on_sendToolBtn_clicked()
{
    if(ui->userTableWidget->selectedItems().isEmpty())
    {
        QMessageBox::warning(0, QStringLiteral("选择用户"), QStringLiteral("请先从用户列表选择要传送的用户！"), QMessageBox::Ok);
        return;
    }
    server->show();
    server->initServer();
}

// 是否接收文件
void ChatSystem::hasPendingFile(QString userName, QString serverAddress,
                                QString clientAddress, QString fileName)
{
    QString ipAddress = getIP();
    if(ipAddress == clientAddress)
    {
        int btn = QMessageBox::information(this,QStringLiteral("接受文件"),
                                           QStringLiteral("来自%1(%2)的文件：%3,是否接收？")
                                           .arg(userName).arg(serverAddress).arg(fileName),
                                           QMessageBox::Yes,QMessageBox::No);
        if (btn == QMessageBox::Yes) {
            QString name = QFileDialog::getSaveFileName(0,QStringLiteral("保存文件"),fileName);
            if(!name.isEmpty())
            {
                TcpClient *client = new TcpClient(this->filePort);
                client->setFileName(name);
                client->setHostAddress(QHostAddress(serverAddress));
                client->show();
            }
        } else {
            sendMessage(Refuse, serverAddress);
        }
    }
}

// 更改字体族
void ChatSystem::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->messageTextEdit->setCurrentFont(f);
    ui->messageTextEdit->setFocus();
}

// 更改字体大小
void ChatSystem::on_sizeComboBox_currentIndexChanged(const QString &arg1)
{
    ui->messageTextEdit->setFontPointSize(arg1.toDouble());
    ui->messageTextEdit->setFocus();
}

// 加粗
void ChatSystem::on_boldToolBtn_clicked(bool checked)
{
    if(checked)
        ui->messageTextEdit->setFontWeight(QFont::Bold);
    else
        ui->messageTextEdit->setFontWeight(QFont::Normal);

    ui->messageTextEdit->setFocus();
}

// 倾斜
void ChatSystem::on_italicToolBtn_clicked(bool checked)
{
    ui->messageTextEdit->setFontItalic(checked);
    ui->messageTextEdit->setFocus();
}

// 下划线
void ChatSystem::on_underlineToolBtn_clicked(bool checked)
{
    ui->messageTextEdit->setFontUnderline(checked);
    ui->messageTextEdit->setFocus();
}

// 颜色
void ChatSystem::on_colorToolBtn_clicked()
{
    color = QColorDialog::getColor(color, this);
    if (color.isValid()) {
        ui->messageTextEdit->setTextColor(color);
        ui->messageTextEdit->setFocus();
    }
}

// 字体
void ChatSystem::currentFormatChanged(const QTextCharFormat &format)
{
    ui->fontComboBox->setCurrentFont(format.font());

    // 如果字体大小出错(因为我们最小的字体为9)，使用12
    if (format.fontPointSize() < 9) {
        ui->sizeComboBox->setCurrentIndex(3);
    } else {
        ui->sizeComboBox->setCurrentIndex( ui->sizeComboBox
                                           ->findText(QString::number(format.fontPointSize())));
    }
    ui->boldToolBtn->setChecked(format.font().bold());
    ui->italicToolBtn->setChecked(format.font().italic());
    ui->underlineToolBtn->setChecked(format.font().underline());
    color = format.foreground().color();
}

// 保存聊天记录
void ChatSystem::on_saveToolBtn_clicked()
{
    if (ui->messageBrowser->document()->isEmpty()) {
        QMessageBox::warning(0, QStringLiteral("警告"), QStringLiteral("聊天记录为空，无法保存！"), QMessageBox::Ok);
    } else {
        QString fileName = QFileDialog::getSaveFileName(this, QStringLiteral("保存聊天记录"), QStringLiteral("聊天记录"), QStringLiteral("文本(*.txt);;All File(*.*)"));
        if(!fileName.isEmpty())
            saveFile(fileName);
    }
}

// 保存聊天记录
bool ChatSystem::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, QStringLiteral("保存文件"),
                             QStringLiteral("无法保存文件 %1:\n %2").arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out << ui->messageBrowser->toPlainText();

    return true;
}

// 清空聊天记录
void ChatSystem::on_clearToolBtn_clicked()
{
    ui->messageBrowser->clear();
}

// 发送
void ChatSystem::on_sendBtn_clicked()
{
    sendMessage(Message);
}

// 退出
void ChatSystem::on_exitBtn_clicked()
{
    close();
}

// 关闭事件
void ChatSystem::closeEvent(QCloseEvent *e)
{
    sendMessage(UserLeave);
    QWidget::closeEvent(e);
}
