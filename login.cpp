#include "login.h"
#include "ui_login.h"
#include "chatsystem.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

qint16 Login::getChatPort()
{
    return (qint16)ui->chatPortSpinBox->value();
}

qint16 Login::getFilePort()
{
    return (qint16)ui->filePortSpinBox->value();
}

void Login::login(qint16 chatPort, qint16 filePort)
{
    this->hide();

    ChatSystem *cs = new ChatSystem(chatPort, filePort);
    cs->show();
}

void Login::on_loginPushButton_clicked()
{
    qint16 chatPort = getChatPort();
    qint16 filePort = getFilePort();

    // 判断聊天端口与文件端口是否相同
    if(chatPort != filePort)
        // 不同则登录
        login(chatPort, filePort);
    else
        QMessageBox::warning(0, QStringLiteral("警告"), QStringLiteral("聊天端口与文件端口不能相同！"), QMessageBox::Ok);
}

void Login::on_closePushButton_clicked()
{
    close();
}
