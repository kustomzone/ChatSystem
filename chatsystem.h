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
#include <QtDebug>
#include <QFileDialog>
#include <QColorDialog>
#include <QTextCharFormat>

class QUdpSocket;
class TcpServer;

namespace Ui {
class ChatSystem;
}

//发送信息类型，分别为：发送信息，用户加入，用户离开，传输文件名，拒绝接收文件
enum MessageType{Message, UserJoin, UserLeave, FileName, Refuse};


class ChatSystem : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChatSystem(qint16 chatPort, qint16 filePort, QWidget *parent = 0);
    ~ChatSystem();

protected:
    void userJoin(QString userName, QString localHostName, QString ipAddress);
    void userLeave(QString userName, QString localHostName, QString time);
    void sendMessage(MessageType type, QString serverAddress="");
    void hasPendingFile(QString userName, QString serverAddress, QString clientAddress, QString fileName);

    bool saveFile(const QString& fileName);
    void closeEvent(QCloseEvent *);

    QString getIP();
    QString getUserName();
    QString getMessage();

private:
    Ui::ChatSystem *ui;
    QUdpSocket *udpSocket;
    qint16 chatPort;
    qint16 filePort;

    QString fileName;
    TcpServer *server;

    QColor color;

private slots:
    void processPendingDatagrams();
    void getFileName(QString);
    void currentFormatChanged(const QTextCharFormat &format);

    void on_sendBtn_clicked();
    void on_exitBtn_clicked();
    void on_sendToolBtn_clicked();
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_sizeComboBox_currentIndexChanged(const QString &arg1);
    void on_boldToolBtn_clicked(bool checked);
    void on_italicToolBtn_clicked(bool checked);
    void on_underlineToolBtn_clicked(bool checked);
    void on_colorToolBtn_clicked();

    void on_saveToolBtn_clicked();
    void on_clearToolBtn_clicked();
};

#endif // CHATSYSTEM_H
