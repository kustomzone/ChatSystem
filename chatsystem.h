#ifndef CHATSYSTEM_H
#define CHATSYSTEM_H

#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QProcess>
#include <QDateTime>
#include <QtNetwork>
#include <QUdpSocket>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QScrollBar>

namespace Ui {
class ChatSystem;
}

//发送信息类型，分别为：发送信息，用户加入，用户离开，传输文件名，拒绝接收文件
enum MessageType{Message, UserJoin, UserLeave, FileName, Refuse};


class ChatSystem : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChatSystem(QWidget *parent = 0);
    ~ChatSystem();

protected:
    void userJoin(QString userName, QString localHostName, QString ipAddress);
    void userLeave(QString userName, QString localHostName, QString time);
    void sendMessage(MessageType type, QString serverAddress="");

    QString getIP();
    QString getUserName();
    QString getMessage();

private:
    Ui::ChatSystem *ui;
    QUdpSocket *udpSocket;
    qint16 port;

private slots:
    void processPendingDatagrams();
    void on_sendBtn_clicked();
    void on_exitBtn_clicked();
};

#endif // CHATSYSTEM_H
